/*  oled_control.c */
#include "oled_control.h"
#include "font5x8.h"

/*   led_type:
    00 = 2 color led    
    01 = 1602 led with port extension
    02/03 = OLED 128x32  normal/inverted display
    04/05 = OLED 128x64  normal/inverted display */
static char e_c_led_addr, e_c_led_type;

/*************************************************/
/*   Soft_I2C routines.   
     The only global entry point is Soft_I2C_Init */

void Soft_I2C_Init()
{
   e_c_led_addr = eeprom_read(EEPROM_DISPLAY_I2C_ADDR); //  4E,7E for 1602   or 78, 7A for OLED
   e_c_led_type = eeprom_read(EEPROM_DISPLAY_TYPE);
   if (e_c_led_type == 0)
   {
      Soft_I2C_Scl_Direction = 0;
      Soft_I2C_Sda_Direction = 0;
      GREEN_LED = LED_OFF;  /*  turn off led's */
      RED_LED = LED_OFF;
   }
   else
   {
      Soft_I2C_Scl_Direction = 1;
      Soft_I2C_Sda_Direction = 1;
      Soft_I2C_Scl = 0;
      Soft_I2C_Sda = 0;
   }
}

static void Soft_I2C_Start()
{
   if (e_c_led_type != 0)
   {
      Soft_I2C_Scl_Direction = 1;
      Delay_5_us();
      Soft_I2C_Sda_Direction = 1;
      Delay_5_us();
      Soft_I2C_Sda_Direction = 0;
      Delay_5_us();
      Soft_I2C_Scl_Direction = 0;
      Delay_5_us();
   }
}
static void Soft_I2C_Write(char d)
{
   char i;
   if (e_c_led_type != 0)
   {
      for (i = 0; i <= 7; i++)
      {
         if ((d & 0x80) == 0)
            Soft_I2C_Sda_Direction = 0;
         else
            Soft_I2C_Sda_Direction = 1;
         Delay_5_us();
         Soft_I2C_Scl_Direction = 1;
         Delay_5_us();
         Soft_I2C_Scl_Direction = 0;
         Delay_5_us();
         d = (char)(d << 1);
      }
      //
      Soft_I2C_Sda_Direction = 1; //ACK
      Delay_5_us();
      Soft_I2C_Scl_Direction = 1;
      Delay_5_us();
      Soft_I2C_Scl_Direction = 0;
      Delay_5_us();
   }
}

static void Soft_I2C_Stop()
{
   if (e_c_led_type != 0)
   {
      Soft_I2C_Sda_Direction = 0;
      Delay_5_us();
      Soft_I2C_Scl_Direction = 1;
      Delay_5_us();
      Soft_I2C_Sda_Direction = 1;
      Delay_5_us();
   }
}

/********************************************************/
/*   private LED variables  */

static char e_c_shift_line;
static char e_c_oled_shift;
static char m_b_type1_RS, m_b_type1_onoff; 

/*  private LED routines.   */

static void led_send(char nibble)
{
   char sdata = (char)(nibble << 4);;

   if (m_b_type1_onoff)
      sdata = sdata + 8;
   /* if (E)  always true */
      sdata = sdata + 4;
   /*  if (RW)   RW always false
      sdata = sdata + 2;   */
   if (m_b_type1_RS)
      sdata = sdata + 1;
   //
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(sdata);    //
   Soft_I2C_Stop();
   Delay_ms(1);
   sdata = sdata - 4; // E = 0
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(sdata);    //
   Soft_I2C_Stop();
   Delay_ms(1);
}

static void send_oled_command(char oled_command)
{
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(128);      // 128 - command, 192 - data
   Soft_I2C_Write(oled_command);
   Soft_I2C_Stop();
}

static void set_oled_addressing(char pagenum, char c_start)
{
   char a, b, c;
   c = c_start + e_c_oled_shift;
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr);       // device addres
   Soft_I2C_Write(0);              // 0 - continious mode, command; 64 - Co, data
   Soft_I2C_Write(0xB0 + pagenum); // set page number
   //
   if (c <= 15)
   {
      a = c;
      b = 0;
   }
   else
   {
      b = c / 16;
      a = c - b * 16;
   }
   Soft_I2C_Write(a);        // set lower nibble of start address
   Soft_I2C_Write(0x10 + b); // set higher nibble of start address
   //
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(64);       // 0 - continious mode, command; 64 - Co, data
}

static void oled_init(void)
{ // OLED init
   char i, r;
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(0);        // 0 - continious mode, command; 64 - Co, data
   Soft_I2C_Write(0xAE);     // display OFF
   //
   if (e_c_led_type == 2 | e_c_led_type == 3)
   {                        // 128*32 OLED
      Soft_I2C_Write(0xD5); // clock division
      Soft_I2C_Write(0x80); // ratio
      //
      Soft_I2C_Write(0xA8); //  multiplexer
      Soft_I2C_Write(0x3f); //
      //
      Soft_I2C_Write(0xD3); //  offset
      if (e_c_led_type == 2)
         Soft_I2C_Write(0 + e_c_shift_line); // column shift
      else
         Soft_I2C_Write(31 + e_c_shift_line); // column shift
      //
      Soft_I2C_Write(0x40); // set line, line = 0
      //
      Soft_I2C_Write(0x8D); //  charge pump
      Soft_I2C_Write(0x14); //  0x10 - external, 0x14 - internal
      //
      Soft_I2C_Write(0x81); //  contrast
      Soft_I2C_Write(180);  //   0-255
      //
      Soft_I2C_Write(0xD9); //  pre-charge
      Soft_I2C_Write(0x22); //  0x22 - external, 0xF1 - internal
      //
      Soft_I2C_Write(0x20); //  memory addressing mode
      Soft_I2C_Write(0x02); //  page addressing mode
      //
      Soft_I2C_Write(0x21); // set column range
      Soft_I2C_Write(0);    // column start
      Soft_I2C_Write(127);  // column stop
      //
      Soft_I2C_Write(0x2E); //  stop scrolling
      //
      if (e_c_led_type == 2)
      {
         Soft_I2C_Write(0xA0); //  segment re-map, A0 - normal, A1 - remapped
         //
         Soft_I2C_Write(0xC0); // scan direction, C0 - normal, C8 - remapped
      }
      else
      {
         Soft_I2C_Write(0xA1); //  segment re-map, A0 - normal, A1 - remapped
         //
         Soft_I2C_Write(0xC8); // scan direction, C0 - normal, C8 - remapped
      }
      //
      Soft_I2C_Write(0xDA); //  COM pins configure
      Soft_I2C_Write(0x02); // 12 for x64
      //
      Soft_I2C_Write(0xDB); //  V-COM detect
      Soft_I2C_Write(0x40); //
      //
   }
   else
   {                        // 128*64 OLED
      Soft_I2C_Write(0xD5); // clock division
      Soft_I2C_Write(0x80); // ratio
      //
      Soft_I2C_Write(0xA8); //  multiplexer
      Soft_I2C_Write(63);   //
      //
      Soft_I2C_Write(0xD3);       //  offset
      Soft_I2C_Write(e_c_shift_line); // no offset for x64 !
      //
      Soft_I2C_Write(0x40); // set line, line = 0
      //
      Soft_I2C_Write(0x8D); //  charge pump
      Soft_I2C_Write(0x14); //  0x10 - external, 0x14 - internal
      //
      Soft_I2C_Write(0x81); //  contrast
      Soft_I2C_Write(255);  //   0-255
      //
      Soft_I2C_Write(0xD9); //  pre-charge
      Soft_I2C_Write(0xf1); //  0x22 - external, 0xF1 - internal
      //
      Soft_I2C_Write(0x20); //  memory addressing mode
      Soft_I2C_Write(0x02); //  page addressing mode   02
      //
      Soft_I2C_Write(0x21); // set column range
      Soft_I2C_Write(0);    // column start
      Soft_I2C_Write(127);  // column stop
      //
      Soft_I2C_Write(0x2E); //  stop scrolling
      //
      if (e_c_led_type == 4)
      {
         Soft_I2C_Write(0xA0); //  segment re-map, A0 - normal, A1 - remapped
         //
         Soft_I2C_Write(0xC0); // scan direction, C0 - normal, C8 - remapped
      }
      else
      {
         Soft_I2C_Write(0xA1); //  segment re-map, A0 - normal, A1 - remapped
         //
         Soft_I2C_Write(0xC8); // scan direction, C0 - normal, C8 - remapped
      }
      //
      Soft_I2C_Write(0xDA); //  COM pins configure
      Soft_I2C_Write(0x12); // 12 for x64
      //
      Soft_I2C_Write(0xDB); //  V-COM detect
      Soft_I2C_Write(0x40); //
   }
   Soft_I2C_Write(0xA4); //  display entire ON
   //
   Soft_I2C_Write(0xA6); // 0xA6 - normal, 0xA7 - inverse
   //
   Soft_I2C_Stop();
   //
   // ********clear OLED***********
   //
   Delay_ms(100);
   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(64);       // 0 - continious mode, command; 64 - Co, data
   //
   if (e_c_led_type == 2 | e_c_led_type == 3)
   { // 128*32 OLED
      for (r = 0; r <= 3; r++)
      {
         set_oled_addressing(r, 0); // clear all 4 pages
         for (i = 0; i < 128; i++, Soft_I2C_Write(0))
            ; // clear one page pixels
      }
   }
   else
   { // 128*64 OLED
      for (r = 0; r <= 7; r++)
      {
         set_oled_addressing(r, 0); // clear all 8 pages
         for (i = 0; i < 128; i++, Soft_I2C_Write(0x00))
            ; // clear one page pixels
      }
   }
   //
   Soft_I2C_Stop();
   send_oled_command(0xAF); //  display ON
   //
}

static void oled_wr_str(char col, char page, char str[], char leng)
{ //    128*32 or 128*64 OLED
    char i, h, g;
   charbits w1, w2;

   Soft_I2C_Start();
   Soft_I2C_Write(e_c_led_addr); // device addres
   Soft_I2C_Write(64);       // 0 - continious mode, command; 64 - Co, data
   //
   set_oled_addressing(page, col);
   //
   for (i = 0; i < leng; i++)
   { // write string
      if (str[i] == 0)
         g = 0;
      else
         g = str[i] - 32; // NULL detection
      for (h = 0; h <= 4; h++)
      { // write letter
         w1.bytes = font5x8[g * 5 + h];
         if (page != 2 & page != 4 & page != 6)
         {
            w2.bits.B7 = w1.bits.B3;
            w2.bits.B6 = w1.bits.B3;
            w2.bits.B5 = w1.bits.B2;
            w2.bits.B4 = w1.bits.B2;
            w2.bits.B3 = w1.bits.B1;
            w2.bits.B2 = w1.bits.B1;
            w2.bits.B1 = w1.bits.B0;
            w2.bits.B0 = w1.bits.B0;
         }
         else
         {
            w2.bits.B7 = w1.bits.B2;
            w2.bits.B6 = w1.bits.B2;
            w2.bits.B5 = w1.bits.B1;
            w2.bits.B4 = w1.bits.B1;
            w2.bits.B3 = w1.bits.B0;
            w2.bits.B2 = w1.bits.B0;
            w2.bits.B1 = 0;
            w2.bits.B0 = 0;
         }
         Soft_I2C_Write(w2.bytes);
         if (e_c_led_type == 4 | e_c_led_type == 5)
            Soft_I2C_Write(w2.bytes); // 128*64
      }
      Soft_I2C_Write(0);
      Soft_I2C_Write(0);
   }
   set_oled_addressing(page + 1, col);
   //
   for (i = 0; i < leng; i++)
   { // write string
      if (str[i] == 0)
         g = 0;
      else
         g = str[i] - 32; // NULL detection
      for (h = 0; h <= 4; h++)
      { // write letter
         w1.bytes = font5x8[g * 5 + h];
         if (page != 2 & page != 4 & page != 6)
         {
            w2.bits.B7 = w1.bits.B7;
            w2.bits.B6 = w1.bits.B7;
            w2.bits.B5 = w1.bits.B6;
            w2.bits.B4 = w1.bits.B6;
            w2.bits.B3 = w1.bits.B5;
            w2.bits.B2 = w1.bits.B5;
            w2.bits.B1 = w1.bits.B4;
            w2.bits.B0 = w1.bits.B4;
         }
         else
         {
            w2.bits.B7 = w1.bits.B6;
            w2.bits.B6 = w1.bits.B6;
            w2.bits.B5 = w1.bits.B5;
            w2.bits.B4 = w1.bits.B5;
            w2.bits.B3 = w1.bits.B4;
            w2.bits.B2 = w1.bits.B4;
            w2.bits.B1 = w1.bits.B3;
            w2.bits.B0 = w1.bits.B3;
         }
         Soft_I2C_Write(w2.bytes);
         if (e_c_led_type == 4 | e_c_led_type == 5)
            Soft_I2C_Write(w2.bytes); // 128*64
      }
      Soft_I2C_Write(0);
      Soft_I2C_Write(0);
      //if(e_c_led_type==2 | e_c_led_type==3) Soft_I2C_Write (0);  // 128*32
   }
   Soft_I2C_Stop();
}

/**************************************************************/
/*  global LED entry points */
void led_init(void)
{
   e_c_shift_line = eeprom_read(EEPROM_DISPLAY_OFFSET_DOWN);
   e_c_oled_shift = eeprom_read(EEPROM_DISPLAY_OFFSET_LEFT);
   if (e_c_led_type > 1)
      oled_init(); //    OLED
   else if (e_c_led_type == 1)
   { //       1602 LCD display init
      m_b_type1_onoff = 1;
      m_b_type1_RS = 0; // Command
      //
      led_send(3);
      Delay_ms(5);
      led_send(3);
      Delay_ms(1);
      led_send(3);
      led_send(2);
      //
      led_send(2);
      led_send(8); //
      //
      led_send(0);
      led_send(8); // LCD OFF
      //
      led_send(0);
      led_send(1); // LCD clear
      Delay_ms(5);
      //
      led_send(0);
      led_send(4 + 2); // I/D, S
      //
      led_send(0);
      led_send(8 + 4 + 0 + 0); // LCD ON, Cursor, Blink Cursor
   }
}

void dysp_on()
{
   if (e_c_led_type > 1)
      send_oled_command(0xAF); //  display ON
   else if (e_c_led_type == 1)
   {
      m_b_type1_onoff = 1;
      m_b_type1_RS = 0;
      led_send(0);
      led_send(0);
   }
}

void dysp_off()
{
   if (e_c_led_type > 1)
      send_oled_command(0xAE); // display OFF
   else if (e_c_led_type == 1)
   {
      m_b_type1_onoff = 0;
      m_b_type1_RS = 0;
      led_send(0);
      led_send(0);
   }
}

void led_wr_str(char col, char pos, char str[], char leng)
{ //
#ifdef SIMULATOR    
    /*  print in debug mode */ 
    char mycolumn[] = "; col=";
    char mylength[] = "; len=";
    char myrow[] = "row=";
    char myquote[]="\"";
    char myspace[]="; ";
    char myspaces[]="        ";
    PRINTTEXT(myrow)  
    IntToStr((int)(col),&tempstring[0]);
    PRINTTEMPSTRINGTEXT(6);

    PRINTTEXT(mycolumn)  
    IntToStr((int)(pos),&tempstring[0]);
    PRINTTEMPSTRINGTEXT(6);

    PRINTTEXT(mylength)  
    IntToStr((int)(leng),&tempstring[0]);
    PRINTTEMPSTRINGTEXT(6);
     
    PRINTTEXT(myspace)     
             
    int spaces = (pos - 16)/12;
    if ((spaces > 0) && (spaces < 8))
    {
        PRINTTEXTLEN(myspaces,(char)(spaces))
    }

    PRINTTEXT(myquote)  
    PRINTTEXTLEN(str,leng)
    PRINTLINE(myquote)  
#endif
#ifdef UART
   char g_work_str_u[7];
   int position = col * 1000 + pos;
   IntToStr(position,g_work_str_u);
   uart_wr_str(g_work_str_u,str,leng);
#endif            
   char i;
   if (e_c_led_type == 4 | e_c_led_type == 5)
      oled_wr_str(pos, col, str, leng); // 128*64  OLED display
   else if (e_c_led_type == 2 | e_c_led_type == 3)
      oled_wr_str(7 * pos + 4, col * 2, str, leng); // 128*32   OLED display
   else
   { // 1602 LCD
      // Set address
      m_b_type1_RS = 0;
      led_send(8 + col * 4);
      led_send(pos);
      //
      m_b_type1_RS = 1;
      for (i = 0; i < leng; i++)
      { // write string
         // write letter
         led_send(str[i] >> 4);
         led_send(str[i]);
      }
   }
}


