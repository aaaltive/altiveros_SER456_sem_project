/* Semester Project Alexa Garage Opener
 * Armand Altiveros
 * SER456 Fall2021 */

// ------- Preamble -------- //
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#include <avr/io.h>      /* Defines pins, ports, etc */
#include <util/delay.h>  /* Functions to waste time */
#include "USART.h" /* UART functions */
#include <util/setbaud.h> /* useful library of BAUD rate */
#include <avr/interrupt.h> /* library for using interrupts on ATMega */
#include "pwm.h"
#include "adc.h"


// ------ MACRO definitions ------ //

#define DOOR_OPEN (PIND & (1 << 6))
#define CO_LEVEL_ELEVATED (readMeasurment(0) > 250) //I found 250 to be the value returned by
                                                    // the CO sensor after running a vehicle
                                                    // for 1 minute.

// ------- GLOBAL VARIABLES -------//
int doorState;

// ------- Functions ------- //
void setUpUART(){
    //set BAUD rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    UCSR0B = (1 << TXEN0) | (1 << RXEN0); //Enable USART transmitter / receiver

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);//Set packet size (8 data bits)

    UCSR0C &= ~(1 << USBS0);//Set stop bit length (1 stop bit)


}

// ------Internal Interrupt service routines ------ //

ISR(INT0_vect){ //Interrupt service routine for door open command from Alexa
        printString("I'm in INT0\n\r");
        _delay_ms(10); //wait a short time to check if falling edge was legitimate button press
        if((PIND & (1 << 2)) == 1) { //if PD2 is high, that means that it was not really a button push, ignore interrupt
            return;
        }else{ //it was a legitimate interrupt
            if (DOOR_OPEN != 0) { //if the door is already open, ignore the command
                printString("Garage Door is already open.\n\r");
                return;
            } else {            //else open the garage door
                printString("Activate Door Opener to open.\n\r");
                PORTD |= (1 << 7); //activate door opener
                _delay_ms(2000);
                PORTD &= ~(1 << 7); //deactivate door opener after 2 seconds
                printString("Deactivate Door Opener\n\r");
            }
        }
}

ISR(INT1_vect){ //Interrupt service routine for door close command from Alexa
        printString("i'm in INT1\n\r");
        _delay_ms(10); //wait a short time to check if falling edge was legitimate button press
        if((PIND & (1 << 3)) == 1) { //if PD3 is high, that means that it was not really a button push, ignore interrupt
            return;
        }else{ //it was a legitimate interrupt
            if (DOOR_OPEN == 0) { //if the door is already closed, ignore the command
                printString("Garage Door is already closed.\n\r");
                return;
            } else {            //else close the garage door
                printString("Activate Door Opener to close.\n\r");
                PORTD |= (1 << 7); //activate door opener
                _delay_ms(2000);
                PORTD &= ~(1 << 7); //deactivate door opener after 2 seconds
                printString("Deactivate Door Opener\n\r");
            }
        }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main(void) {

    //compiler instructions to let compiler know what to set the BAUD rate to
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    //Initialise pins at input/output
    DDRD |= 0b10000010; // bit0/1 are RX/TX, bit 2 and 3 are interrupts to open/close garage,
    // bit 6 is the door state, bit 7 is the open/close command
    //Enable Pullup resistor on PIND6 for garage state circuit
    PORTD |= (1 << 6);
    //Initialize and set up interrupt pin
    EIMSK |= (1 << INT0) | (1 << INT1); //initializes Interrupt on pin PD2 (INT0) and PD3 (INT1)
    //select interrupt sense control to be on rising edge
    EICRA |= (1 << ISC01) | (1 << ISC00);
    EICRA |= (1 << ISC11) | (1 << ISC10);
    //global enable for all interrupts initialized
    sei();
    setUpUART(); //calls function to set up UART
    _delay_ms(250); //this delay just prevents garbage printout when restarting the ATMega
    setupADC(); //calls functions to set up ADC

    int cOReading; //variable for the CO level in garage
    char value[4]; //holds a string representation of cOReading

    /* ------ Event loop ------ */
    while (1) {
        _delay_ms(5000);
        doorState = DOOR_OPEN;
        cOReading = readMeasurment(0);
        sprintf(value, "%d", cOReading); //convert int cOReading to string
        printString("No interrupts this cycle, door state: ");
        switch (doorState) {
            case 0: //door closed
                printString("CLOSED.\n\r");
                break;
            default: //door open, I want it to always assume the door is open if it is undefined
                printString("OPEN.\n\r");
                break;
        }
        printString("CO sensor reading: ");
        if(CO_LEVEL_ELEVATED){ //read and compare the CO level with baseline
            printString("ELEVATED! opening door now.\n\r");
            if (DOOR_OPEN != 0) { //if the door is already open, ignore the command
                printString("Garage Door is already open.\n\r");
                return;
            } else {  //else open the garage door
                printString("Activate Door Opener to open.\n\r");
                PORTD |= (1 << 7); //activate door opener
                _delay_ms(2000);
                PORTD &= ~(1 << 7); //deactivate door opener after 2 seconds
                printString("Deactivate Door Opener\n\r");
            }
        } else {
            printString("Within tolerances.\n\r");
        }
    }                      /* End event loop */
    return 0;              /* This line is never reached */
}
#pragma clang diagnostic pop

#pragma clang diagnostic pop
#pragma clang diagnostic pop