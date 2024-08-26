
#include "main.h"
#include "lcd.h"
#include "keypad.h"
#include "avr.h"

void reset_stats();
void update_display();
uint16_t readADC(uint8_t channel);
void initADC();
float adcToVoltage(uint16_t adcValue);
void initTimer1();
ISR(TIMER1_COMPA_vect);

int max_voltage;
int min_voltage;
float average_voltage;
int instant_voltage;
char buf[20];

#define ADC_MAX_VALUE 1023
#define VOLTAGE_REF 5.0
#define ADC_CHANNEL 0

volatile unsigned long sample_count = 0;
volatile unsigned long sample_sum = 0;
volatile int takeStatisticsSamples = 1;

int main() {
    set_input_output_pins();
    lcd_init_8bit();
    starting_msg();
    reset_stats();
    initADC();
    initTimer1(); 

    sei();  

    while(1) {   
        char input = scan_keypad();

        if (input != '#' && input != '\0') {
            if (input == '1') {
                reset_stats();
                takeStatisticsSamples = 0;
            } else if (input == '2') {
                takeStatisticsSamples = 1;
            }
        }

        update_display();
        _delay_ms(500);
    }

    return 0;
}

void reset_stats() {
    lcd_pos(1,11);
    lcd_send_string("----");

    lcd_pos(2,3);
    lcd_send_string("----");
    lcd_pos(2,11);
    lcd_send_string("----");

    max_voltage = -1;
    min_voltage = ADC_MAX_VALUE + 1;
    average_voltage = -1.0;
    sample_count = 0;
    sample_sum = 0;
}

void update_display() {
    lcd_pos(1, 3);
    if (instant_voltage == -1) {
        lcd_send_string("----");
    } else {
        float inst_volt = adcToVoltage(instant_voltage);
        sprintf(buf, "%.2f", inst_volt);
        lcd_send_string(buf);
    }

    lcd_pos(1, 11);
    if (max_voltage == -1) {
        lcd_send_string("----");
    } else {
        float max_volt = adcToVoltage(max_voltage);
        sprintf(buf, "%.2f", max_volt);
        lcd_send_string(buf);
    }

    lcd_pos(2, 3);
    if (min_voltage == ADC_MAX_VALUE + 1) {
        lcd_send_string("----");
    } else {
        float min_volt = adcToVoltage(min_voltage);
        sprintf(buf, "%.2f", min_volt);
        lcd_send_string(buf);
    }

    lcd_pos(2, 11);
    if (average_voltage == -1.0) {
        lcd_send_string("----");
    } else {
        float avg_volt = average_voltage * (VOLTAGE_REF / ADC_MAX_VALUE);
        sprintf(buf, "%.2f", avg_volt);
        lcd_send_string(buf);
    }
}

void initADC() {

    ADMUX = (1 << REFS0);


    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readADC(uint8_t channel) {

    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);


    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    return ADC;
}

float adcToVoltage(uint16_t adcValue) {

    return (adcValue * VOLTAGE_REF) / ADC_MAX_VALUE;
}

void initTimer1() {

    TCCR1B |= (1 << WGM12);

    OCR1A = 3906;

    TIMSK |= (1 << OCIE1A);

    TCCR1B |= (1 << CS12) | (1 << CS10);
}


ISR(TIMER1_COMPA_vect) {
    instant_voltage = readADC(ADC_CHANNEL);

    if (takeStatisticsSamples == 1) {
        sample_count++;
        sample_sum += instant_voltage;

        average_voltage = (float)sample_sum / sample_count;

        if (instant_voltage > max_voltage) {
            max_voltage = instant_voltage;
        }

        if (instant_voltage < min_voltage) {
            min_voltage = instant_voltage;
        }
    }
}

































// #include "main.h"
// #include "lcd.h"
// #include "keypad.h"
// #include "avr.h"

// void reset_stats();
// void update_display();
// int get_sample(int index);




// int max_voltage;
// int min_voltage;
// float average_voltage;
// int instant_voltage;
// char buf[20];
// unsigned long sample_count = 0;
// unsigned long sample_sum = 0;

// #define ADC_MAX_VALUE 1023
// #define VOLTAGE_REF 5.0

// int main() {
//     int index = 0;
   
//     char input;
//     int takeStatisticsSamples = 1;

//     set_input_output_pins();
//     lcd_init_8bit();
//     starting_msg();
//     reset_stats();

//     while(1) {   
//         input = scan_keypad();

//         if (input != '#' && input != '\0') {
//             if (input == '1') {
//                 reset_stats();
//                 takeStatisticsSamples = 0;
//             } else if (input == '2') {
//                 takeStatisticsSamples = 1;
//             }
//         }

//         int new_sample = get_sample(index);
//         instant_voltage = new_sample;

//         if (takeStatisticsSamples == 1) {
//             // Calculate average and set new value
//             sample_count += 1;
//             sample_sum += new_sample;

//             average_voltage = (float)sample_sum / sample_count;

//             // Check if max changes
//             if (new_sample > max_voltage) {
//                 max_voltage = new_sample;
//             }

//             // Check if min changes
//             if (new_sample < min_voltage) {
//                 min_voltage = new_sample;
//             }
//         }

//         update_display();
//         _delay_ms(1000);
//         index++;
//     }

//     return 0;
// }

// int get_sample(int index) {
//     // Simulated realistic ADC values (0-1023)
//     int nums[] = {
//         512, 523, 534, 545, 556, 567, 578, 589, 578, 567,
//         556, 545, 534, 523, 512, 501, 490, 479, 468, 457,
//         446, 435, 424, 413, 424, 435, 446, 457, 468, 479,
//         490, 501, 512, 523, 534, 545, 556, 567, 578, 589,
//         578, 567, 556, 545, 534, 523, 512
//     };
//     int nums_size = sizeof(nums) / sizeof(nums[0]);
//     return nums[index % nums_size];
// }

// void reset_stats() {
//     lcd_pos(1,11);
//     lcd_send_string("----");

//     lcd_pos(2,3);
//     lcd_send_string("----");
//     lcd_pos(2,11);
//     lcd_send_string("----");

//     max_voltage = -1;
//     min_voltage = ADC_MAX_VALUE + 1;
//     average_voltage = -1.0;
//     sample_count = 0;
//     sample_sum = 0;
// }

// void update_display() {
//     lcd_pos(1,3);
//     if (instant_voltage == -1) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(1,3);
//         sprintf(buf, "%.2f", instant_voltage * (VOLTAGE_REF / ADC_MAX_VALUE));
//         lcd_send_string(buf);
//     }

//     lcd_pos(1,11);
//     if (max_voltage == -1) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(1,11);
//         sprintf(buf, "%.2f", max_voltage * (VOLTAGE_REF / ADC_MAX_VALUE));
//         lcd_send_string(buf);
//     }

//     lcd_pos(2,3);
//     if (min_voltage == ADC_MAX_VALUE + 1) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(2,3);
//         sprintf(buf, "%.2f", min_voltage * (VOLTAGE_REF / ADC_MAX_VALUE));
//         lcd_send_string(buf);
//     }

//     lcd_pos(2,11);
//     if (average_voltage == -1.0) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(2,11);
//         sprintf(buf, "%.2f", average_voltage * (VOLTAGE_REF / ADC_MAX_VALUE));
//         lcd_send_string(buf);
//     }
// }



































// #include "main.h"
// #include "lcd.h"
// #include "keypad.h"
// #include "avr.h"

// void reset_stats();
// void update_display();
// int get_sample(int index);

// int max_voltage;
// int min_voltage;
// float average_voltage;
// int instant_voltage;
// char buf[10];

// int main() {
//     int index = 0;
//     unsigned long sample_count = 0;
//     unsigned long sample_sum = 0;
//     char input;
//     int takeStatisticsSamples = 1;

//     set_input_output_pins();
//     lcd_init_8bit();
//     starting_msg();
//     reset_stats();

//     while(1) {   
//         input = scan_keypad();

//         if (input != '#' && input != '\0') {
//             if (input == '1') {
//                 reset_stats();
//                 takeStatisticsSamples = 0;
//             } else if (input == '2') {
//                 takeStatisticsSamples = 1;
//             }
//         }

//         int new_sample = get_sample(index);
//         sample_count += 1;
//         sample_sum += new_sample;

//         instant_voltage = new_sample;

//         if (takeStatisticsSamples == 1) {
//             // calculate average and set new value
//             average_voltage = (float)sample_sum / sample_count;

//             // check to see if max changes
//             if (new_sample > max_voltage) {
//                 max_voltage = new_sample;
//             }

//             // check to see if min changes
//             if (new_sample < min_voltage) {
//                 min_voltage = new_sample;
//             }
//         }

//         update_display();
//         _delay_ms(1000);
//         index++;
//     }

//     return 0;
// }

// int get_sample(int index) {
//     int nums[] = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1,2,3,4,5,6,7,8,7,6,5,4,3,2,1};
//     return nums[index];
// }

// void reset_stats() {
//     lcd_pos(1,11);
//     lcd_send_string("----");

//     lcd_pos(2,3);
//     lcd_send_string("----");
//     lcd_pos(2,11);
//     lcd_send_string("----");

//     max_voltage = -1;
//     min_voltage = 1024;
//     average_voltage = -1.0;
// }

// void update_display() {
//     lcd_pos(1,3);
//     if (instant_voltage == -1) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(1,3);
//         lcd_send_integer(instant_voltage);
//     }

//     lcd_pos(1,11);
//     if (max_voltage == -1) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(1,11);
//         lcd_send_integer(max_voltage);
//     }

//     lcd_pos(2,3);
//     if (min_voltage == 1024) {
//         lcd_send_string("----");
//     } else {
//         lcd_send_string("    ");
//         lcd_pos(2,3);
//         lcd_send_integer(min_voltage);
//     }

//     lcd_pos(2,11);
//     if (average_voltage == -1.0) {
//         lcd_send_string("----");
//     } else {
//         // Clear buffer before use
//         memset(buf, 0, sizeof(buf));
//         sprintf(buf, "%.2f", average_voltage * (5.0 / 1023.0)); // Convert average to voltage
//         lcd_send_string("    ");
//         lcd_pos(2,11);
//         lcd_send_string(buf);
//     }
// }