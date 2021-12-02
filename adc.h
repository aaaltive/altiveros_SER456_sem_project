//
// Created by Armand on 10/20/2021.
//

#ifndef ASSIGNMENT_4_INITADC_H
#define ASSIGNMENT_4_INITADC_H

#define SEL_ADC0 ADMUX &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3))
#define SEL_ADC1 ADMUX |= (1 << MUX0) //Select potentiometer first to clear bits
#define SEL_ADC2 ADMUX |= (1 << MUX1) //Select potentiometer first to clear bits
#define READ_BIT(reg, bit) (reg & (1 << bit))  //reads bit "bit" on register "reg"
#define START_ADC_CONV ADCSRA |= (1 << ADSC)  //starts an ADC conversion
#define WAIT_FOR_CONVERSION while (READ_BIT(ADCSRA, ADSC) != 0)

/*
 * Performs preliminary setup for the ADC to ne initialized
 */
void setupADC();

/*
 * This will set the ADMUX to the appropriate pin, start a conversion, and return the value stored in the ADC register
 */
int readMeasurment(int adcPin);

#endif //ASSIGNMENT_4_INITADC_H
