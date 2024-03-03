
#include "cross_compiler.h"

#pragma config FOSC = 0x04


#ifdef SIMULATOR

void init_uart(void) {
    TXSTAbits.TXEN = 1;               // enable transmitter
    RCSTAbits.SPEN = 1;               // enable serial port
}
void putch(unsigned char data) {
    while( ! PIR1bits.TXIF)          // wait until the transmitter is ready
        continue;
    TXREG = data;                     // send one character
}

char firstcall = 0;
/*   make these 'global' since having them as parameters puts a 
     real burden on the stack space */
char tempstring[100];
char terminator = 0;
char stringlength = 0;
char* stringptr = NULL;

void debugprint()
{
   char c;
   if (firstcall == 0)
   {
       init_uart();
       firstcall = 1;
   }
   for (c = 0; c < stringlength; c++)
   {
      putch(stringptr[c]);
   }
   if (terminator)
   {
       putch('\n');
   }
}
char mystring[] = "Delay called with ";
void IntToStr(int number, char *output);
#endif

void Delay_ms(const unsigned int time_in_ms)
{
  unsigned int i = time_in_ms;
  CLRWDT();
  
#ifdef SIMULATOR  
  if (time_in_ms > 99)
  {
    PRINTTEXT(mystring)  
                        
    IntToStr((int)(time_in_ms),&tempstring[0]);
    PRINTTEMPSTRINGLINE(6);
  }
#endif

  while (i > 0)
  {
    /*        _delay((unsigned long)((5)*(_XTAL_FREQ/4000.0)));*/
    _delay((unsigned long)(4000));
    CLRWDT();
    i--;
  }
};

unsigned int ADC_Get_Sample(char channel)
{
  ADCON0bits.CHS = channel;
  ADCON0bits.ADGO = 1; /* start A/D */
  Delay_5_us();
  while (ADCON0bits.ADGO == 1)
    ;
  unsigned char lower = ADRESL;
  unsigned char upper = ADRESH;
  unsigned int result = (unsigned int)(upper << 8) + lower;
  return result;
};

/*  routine to detect button presses and debouncing.  input parms:
 port number, port-pin number  time in ms to delay, and value to expect.
 * returns 255 if the pin was in the active state for the given period
 * returns 0 otherwise
 * we use PORTB, pins 0, 1 and 2 */
char Button(volatile unsigned char *port, char pin, char time, char active_state)
{
  char loop = time;
  char value;
  while (loop > 0)
  {
    switch (pin)
    {
    case 0:
      value = PORTBbits.RB0;
      break;
    case 1:
      value = PORTBbits.RB1;
      break;
    default:
      value = PORTBbits.RB2;
      break;
    }
    if (value != active_state)
    {
      return 0;
    }
    Delay_ms(1);
    loop--;
  }
  return 255;
};

unsigned char Bcd2Dec(unsigned char bcdnum)
{
  return ((bcdnum / 16 * 10) + (bcdnum % 16));
};

unsigned char Dec2Bcd(unsigned char decnum)
{
  return ((decnum / 10 * 16) + (decnum % 10));
};

void Vdelay_ms(int time_in_ms)
{
  Delay_ms((const unsigned int)(time_in_ms));
};

void ADC_Init(void){};

/*   Function creates an OUTPUT string out of a signed number (numerical 
 * value of int type).  Output string has fixed width of 6 characters;
 * remaining positions on the left (if any ) are filled with blanks */
void IntToStr(int number, char *output)
{
  char *p = output;
  char loopcounter = 0;
  do
  {
    *p = ' '; /* fill with blanks */
    p++;
    loopcounter++;
  } while (loopcounter < 6);
  p = output + 5; /* point to the last digit */
  if (number >= 0)
  {
    do
    {
      *p = '0' + (number % 10);
      p--;
      number /= 10;
      loopcounter++;
    } while (number != 0);
  }
  else
  { /* i < 0 */
    do
    {
      *p = '0' - (number % 10);
      p--;
      number /= 10;
    } while (number != 0);
    *p = '-';
  }
};
