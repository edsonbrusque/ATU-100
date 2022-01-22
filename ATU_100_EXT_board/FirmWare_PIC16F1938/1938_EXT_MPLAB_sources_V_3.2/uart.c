#include "cross_compiler.h"

// Method for transmitting value using serial bit banging
#define MYONE 1
#define MYZERO 0
void uart_tx_bit_bang(unsigned char val) {
    unsigned char i;
    UART_OUT_PIN = MYZERO;                         // Start bit
    Delay_100_us();
    for ( i = 8 ; i != 0 ; --i )
    {
        if (val & 0x01)
        {
            UART_OUT_PIN = MYONE;   // Begin with LSB
        }
        else
        {
            UART_OUT_PIN = MYZERO;
        }
        val >>= 1;
        Delay_100_us();
    }
    UART_OUT_PIN = MYONE;                         // Stop bit
    Delay_100_us();
    Delay_100_us();//  add some extra delays after the stop bit
}

//  posstr is:   
//  the thousands digit is the row.  range is 0-6.  for some displays divide it by 2
//  to get the correct row.
//  the 3 ls digits is the column
void uart_wr_str(char posstr[],char str[], char leng)
{
   char i;
    //  print a space then the 4 digit position
   //  replace blanks with zeros in the position
   if (posstr[2] == ' ')
       posstr[2] = '0';
   if (posstr[3] == ' ')
       posstr[3] = '0';
   if (posstr[4] == ' ')
       posstr[4] = '0';
   if (posstr[5] == ' ')
       posstr[5] = '0';
   
   for (i = 2; i < 6; i++)
   {
      uart_tx_bit_bang(posstr[i]);
   }
   uart_tx_bit_bang(':');
   for (i = 0; i < leng; i++)
   { // write string
      if (str[i] == 0)
      {
         break;
      }
      uart_tx_bit_bang(str[i]);
    }
    uart_tx_bit_bang(0x0d);  // send a CR
    uart_tx_bit_bang(0x0a);  // send a LF
}
