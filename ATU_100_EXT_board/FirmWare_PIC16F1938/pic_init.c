// pic_init unit for Micro C PRO
// David Fainitski for ATU-100 project
// PIC1938 Microchip

#include "cross_compiler.h"

void pic_init(void) {
    CLRWDT();
    WDTCONbits.WDTPS4 = 0;
    WDTCONbits.WDTPS3 = 1;
    WDTCONbits.WDTPS2 = 0; // 1 sec WDT
    WDTCONbits.WDTPS1 = 1;
    WDTCONbits.WDTPS0 = 0;
    CLRWDT();

    ANSELAbits.ANSELA = 0;
    ANSELAbits.ANSA0 = 1; // analog inputs
    ANSELAbits.ANSA1 = 1;
    ANSELBbits.ANSELB = 0; // all as digital

    CM1CON0bits.C1ON = 0; // Disable comparators
    CM2CON0bits.C2ON = 0;

    OSCCON = 0b01111010; // 16 MHz oscillator, Internal

    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    /* LATA = 0b01000000;*/ // PORT6 /Tx_req to 1
    LATA = 0b11000000; // A6 and A7 set high
    LATB = 0;
    LATC = 0;
    TRISA = 0b00000011;
    TRISB = 0b00000111;
    TRISC = 0b00000000;

    UART_IN_TRIS = 1;
    UART_OUT_TRIS = 0;

    ADC_Init();

    ADCON1bits.ADPREF0 = 1; // ADC with the internal reference
    ADCON1bits.ADPREF1 = 1;

    /* init the ADC here */
    FVRCON = 0x81; /*enable FVR */
    //  ADCON1bits.ADCS0 = 0; /* use Fosc/32 */
    //  ADCON1bits.ADCS1 = 1;
    //  ADCON1bits.ADCS2 = 0;
    //
    ADCON1 = 0xA3; /* use Fosc/32 */

    ADCON0bits.ADON = 1;
    //
    OPTION_REGbits.nWPUEN = 0;
    WPUBbits.WPUB0 = 1; // PORTB0 Pull-up resistor
    WPUBbits.WPUB1 = 1; // PORTB1 Pull-up resistor
    WPUBbits.WPUB2 = 1; // PORTB2 Pull-up resistor
    //interrupt setting
    INTCONbits.GIE = 0;
}
