//
// Created by Armand on 10/20/2021.
//

#include "adc.h"
#include <avr/io.h>

void setupADC(){

    /* ------ Set up ADC ------ */
    //set the reference voltage
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << REFS1);

    //set clock prescaler
    ADCSRA |= ((1 << ADPS0) | (1 << ADPS1));
    ADCSRA &= ~(1 << ADPS2);

    //Select the channel in the multiplexor that our sensor is plugged into
    ADMUX &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3));

    //Enable ADC
    ADCSRA |= (1 << ADEN);

}

int readMeasurment(int adcPin){
    switch (adcPin) {
        case 0:
            SEL_ADC0;
            break;
        case 1:
            SEL_ADC0;
            SEL_ADC1;
            break;
        case 2:
            SEL_ADC0;
            SEL_ADC2;
            break;

    }
    START_ADC_CONV;
    WAIT_FOR_CONVERSION;
    return ADC;
}
