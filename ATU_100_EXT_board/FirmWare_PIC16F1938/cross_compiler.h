/// @brief This provides an definitions to support compiling on the MPLAB X compilers
///
#ifndef CROSS_COMPILER_H_INCLUDED
#define CROSS_COMPILER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

    /*  note datasizes for MikroC are:
     byte and short are 8 bits
     int is 16 bits
     long is 32 bits
     float and double are 32 bits
     */
    /*  mplab xc8 compiler sizes are
     char is 8 bits
     short is 16 bits
     int is 16 bits
     long is 32 bits
     float and double are allowed to be 24 bit or 32 bit, but 
     to be standard we use 32 bits
 
 so we need to replace short with char throughout
     */

    /*  variable naming convention:  
     g_  global
     l_  local
     e_  'constants' from eeprom
     
     _i_   integer
     _c_   char
     _b_   boolean 
     */
    /*  add a new structure for the ALL compilers */
    //  bitfield character

    typedef union {
        unsigned char bytes;

        struct {
            unsigned B0 : 1;
            unsigned B1 : 1;
            unsigned B2 : 1;
            unsigned B3 : 1;
            unsigned B4 : 1;
            unsigned B5 : 1;
            unsigned B6 : 1;
            unsigned B7 : 1;
        } bits;
    } charbits;

    /* eeprom cells */
#define EEPROM_LAST_CAP 255
#define EEPROM_LAST_IND 254
#define EEPROM_LAST_SW 253
#define EEPROM_LAST_SWR_H 252
#define EEPROM_LAST_SWR_L 251

#define EEPROM_DISABLE_RELAYS 0x35
#define EEPROM_FEEDER_LOSS 0x34
#define EEPROM_ADDITIONAL_INDICATION 0x33
#define EEPROM_DISPLAY_OFF_TIMER 0x32
#define EEPROM_TANDEM_MATCH 0x31
#define EEPROM_POWER_MEASURE_LEVEL 0x30

#define EEPROM_INVERSE_INDUCTANCE_RELAY 15
#define EEPROM_ENABLE_NONLINEAR_DIODE 14
#define EEPROM_CAP_LINEAR_PITCH 13
#define EEPROM_NUMBER_CAPS 12
#define EEPROM_IND_LINEAR_PITCH 11
#define EEPROM_NUMBER_INDS 10
#define EEPROM_MAX_INIT_SWR 9
#define EEPROM_DISPLAY_OFFSET_LEFT 8
#define EEPROM_DISPLAY_OFFSET_DOWN 7
#define EEPROM_MAX_POWER 6
#define EEPROM_MIN_POWER 5
#define EEPROM_SWR_THRESHOLD 4
#define EEPROM_TIMEOUT_TIME 3
#define EEPROM_AUTOMATIC_MODE 2
#define EEPROM_DISPLAY_TYPE 1
#define EEPROM_DISPLAY_I2C_ADDR 0

    /*  define the port B button bits */

#define TUNE_BUTTON 0
#define AUTO_BUTTON 1
#define BYPASS_BUTTON 2

    /*  define the button states */
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

    /*  define the LED states */
#define LED_ON 0
#define LED_OFF 1

#ifdef SIMULATOR
#define __DEBUG
#endif

#ifdef SIMULATOR
#include <string.h>

    extern char tempstring[100];
    extern char terminator;
    extern char stringlength;
    extern char* stringptr;

    void debugprint(void);

#define PRINTLINELEN(lineptr,length) terminator=1; stringlength=length; stringptr = &lineptr[0]; debugprint();
#define PRINTTEXTLEN(textptr,length) terminator=0; stringlength=length; stringptr = &textptr[0]; debugprint();

#define PRINTLINE(lineptr) terminator=1; stringlength=(char)(strlen(lineptr)); stringptr = &lineptr[0]; debugprint();
#define PRINTTEXT(textptr) terminator=0; stringlength=(char)(strlen(textptr)); stringptr = &textptr[0]; debugprint();

#define PRINTTEMPSTRINGLINE(length) terminator=1; stringlength=length; stringptr = &tempstring[0]; debugprint();
#define PRINTTEMPSTRINGTEXT(length) terminator=0; stringlength=length; stringptr = &tempstring[0]; debugprint();

#define PRINTDEBUG() debugprint();

#else
#define PRINTLINELEN(lineptr,length) 
#define PRINTTEXTLEN(textptr,length) 

#define PRINTLINE(lineptr) 
#define PRINTTEXT(textptr) 

#define PRINTTEMPSTRINGLINE(length) 
#define PRINTTEMPSTRINGTEXT(length) 

#define PRINTDEBUG() 
#endif

#include "xc.h"
#include <stdbool.h>

    //  the internal oscillator is 16 Mhz
    //  the xc8 compiler needs the crystal freq defined here

#define _XTAL_FREQ 16000000

    //  the CPU clock is 4 Mhz, so to convert to microseconds, there are
    //  4 clocks per usec

#define DELAY_5_US_CLOCK 20

#define Delay_5_us() _delay((unsigned long)(DELAY_5_US_CLOCK));

#define DELAY_100_US_CLOCK 400

#define Delay_100_us() _delay((unsigned long)(DELAY_100_US_CLOCK));

    void Delay_ms(const unsigned int time_in_ms);

    unsigned int ADC_Get_Sample(char channel);

    char Button(volatile unsigned char *port, char pin, char time, char active_state);

    unsigned char Bcd2Dec(unsigned char bcdnum);

    unsigned char Dec2Bcd(unsigned char decnum);

    void Vdelay_ms(int time_in_ms);

    void ADC_Init(void);

    void IntToStr(int number, char *output);

    /*  forward references*/

    void Test_init(void);

    bool out_dummy; //PP5OO
    bool in_dummy = true; //PP5OO

#define UART_IN_PIN  PORTBbits.RB2 //PP5OO
#define UART_IN_TRIS TRISBbits.TRISB2 //PP5OO

#define UART_OUT_PIN  LATBbits.LATB1 //PP5OO
#define UART_OUT_TRIS TRISBbits.TRISB1 //PP5OO

#define TUNE_BUTTON_PIN  in_dummy //PP5OO
#define TUNE_BUTTON_TRIS out_dummy //PP5OO

#define AUTO_BUTTON_PIN  in_dummy //PP5OO
#define AUTO_BUTTON_TRIS out_dummy //PP5OO

#define BYPASS_BUTTON_PIN  in_dummy //PP5OO
#define BYPASS_BUTTON_TRIS out_dummy //PP5OO

#define PORTB_TUNE_BUTTON in_dummy
#define PORTB_AUTO_BUTTON in_dummy
#define PORTB_BYPASS_BUTTON in_dummy

#define n_Tx out_dummy //PP5OO
#define p_Tx out_dummy //PP5OO

#define GREEN_LED LATBbits.LATB6
#define RED_LED LATBbits.LATB7

#define Soft_I2C_Scl LATBbits.LATB6
#define Soft_I2C_Sda LATBbits.LATB7 
#define Soft_I2C_Scl_Direction TRISBbits.TRISB6
#define Soft_I2C_Sda_Direction TRISBbits.TRISB7 

#define Cap_10 LATCbits.LATC7
#define Cap_22 LATCbits.LATC3
#define Cap_47 LATCbits.LATC6
#define Cap_100 LATCbits.LATC2
#define Cap_220 LATCbits.LATC5
#define Cap_470 LATCbits.LATC1
#define Cap_1000 LATCbits.LATC4
#define Cap_sw LATCbits.LATC0
    //
#define Ind_005 LATBbits.LATB3
#define Ind_011 LATAbits.LATA2
#define Ind_022 LATBbits.LATB4
#define Ind_045 LATAbits.LATA3
#define Ind_1 LATBbits.LATB5
#define Ind_22 LATAbits.LATA5
#define Ind_45 LATAbits.LATA4


    /*  define how we wait for the Fixed voltage Regulator to be stable */
#define WAIT_FOR_FVR    while (FVRCONbits.FVRRDY == 0);


#ifdef __cplusplus
}
#endif
/*    endif file already included */
#endif
