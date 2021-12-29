/// @brief This provides an definitions to support compiling on the MPLAB X compilers
///
#ifndef CROSS_COMPILER_H_INCLUDED
#define CROSS_COMPILER_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
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
    typedef union
    {
        unsigned char bytes;
        struct
        {
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
//
//#define Button  RB0;
//#define BYP_button  RB2;
//#define Auto_button  RB1;
    
#define TUNE_BUTTON 0
#define AUTO_BUTTON 1
#define BYPASS_BUTTON 2
    
#define PORTB_TUNE_BUTTON PORTBbits.RB0
#define PORTB_AUTO_BUTTON PORTBbits.RB1
#define PORTB_BYPASS_BUTTON PORTBbits.RB2

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
    

/*  comment the following line out if you are using MikroC compiler */
#define MPLAB_COMPILER
/********************************************************************************/
#ifdef MPLAB_COMPILER

#include "xc.h"

//  the internal oscillator is 16 Mhz
//  the xc8 compiler needs the crystal freq defined here

#define _XTAL_FREQ 16000000

//  the CPU clock is 4 Mhz, so to convert to microseconds, there are
//  4 clocks per usec

#define DELAY_5_US_CLOCK 20

#define Delay_5_us() _delay((unsigned long)(DELAY_5_US_CLOCK));

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

//  the Version 3.2 uses A6 and A7 for the Tx lines to the transmitter
//  And the data and clock lines to the Display uses B6 and B7, (also for the 
//  Red/Green leds.

//  WA1RCT defines things slightly differently.  The debugger uses B6 and B7
//  and does not need the Tx lines to the transmitter.  So I defined the LED
//  data and clock lines as A6 and A7, leaving B6 and B7 for the debugger.
//  Since B1 and B2 are defined as inputs, writing to them does nothing,
//  so we will put n_Tx and p_Tx there, and the LED's also

//  uncomment out the next line to move the LED I2C to A6 and A7
//  #define WA1RCT
    
#ifdef WA1RCT
#define n_Tx LATBbits.LATB1
#define p_Tx LATBbits.LATB2
    
#define GREEN_LED LATBbits.LATB1
#define RED_LED LATBbits.LATB2 

#define Soft_I2C_Scl LATAbits.LATA6
#define Soft_I2C_Sda LATAbits.LATA7    
#define Soft_I2C_Scl_Direction TRISAbits.TRISA6
#define Soft_I2C_Sda_Direction TRISAbits.TRISA7    
    
#else
#define n_Tx LATAbits.LATA6
#define p_Tx LATAbits.LATA7
    
#define GREEN_LED LATBbits.LATB6
#define RED_LED LATBbits.LATB7     
#define Soft_I2C_Scl LATBbits.LATB6
#define Soft_I2C_Sda LATBbits.LATB7 
#define Soft_I2C_Scl_Direction TRISBbits.TRISB6
#define Soft_I2C_Sda_Direction TRISBbits.TRISB7 
#endif
    
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
    
/********************************************************************************/
    /* else mikroC compiler */
#else
 
#define LATBbits.LAT   LATB
#define GREEN_LED LATBbits.LATB6
#define RED_LED LATBbits.LATB7


sbit n_Tx at LATA6_bit;
sbit p_Tx at LATA7_bit;
//
//sbit Button at RB0_bit;
//sbit BYP_button at RB2_bit;
//sbit Auto_button at RB1_bit;
//
sbit Cap_10 at LATC7_bit;
sbit Cap_22 at LATC3_bit;
sbit Cap_47 at LATC6_bit;
sbit Cap_100 at LATC2_bit;
sbit Cap_220 at LATC5_bit;
sbit Cap_470 at LATC1_bit;
sbit Cap_1000 at LATC4_bit;
sbit Cap_sw at LATC0_bit;
//
sbit Ind_005 at LATB3_bit;
sbit Ind_011 at LATA2_bit;
sbit Ind_022 at LATB4_bit;
sbit Ind_045 at LATA3_bit;
sbit Ind_1 at LATB5_bit;
sbit Ind_22 at LATA5_bit;
sbit Ind_45 at LATA4_bit;

void Delay_5_us();
    
sbit Soft_I2C_Scl at LATB6_bit;
sbit Soft_I2C_Sda at LATB7_bit;
sbit Soft_I2C_Scl_Direction at TRISB6_bit;
sbit Soft_I2C_Sda_Direction at TRISB7_bit;

/*  define how we wait for the Fixed voltage Regulator to be stable */

#define WAIT_FOR_FVR while (FVRCON.B6 == 0);

/* #endif which compiler */    
#endif

#ifdef __cplusplus
}
#endif
/*    endif file already included */
#endif
