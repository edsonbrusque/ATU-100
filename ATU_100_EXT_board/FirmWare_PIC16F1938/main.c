//   ATU-100 project
//   David Fainitski
//   2016

#include "cross_compiler.h"
#include "uart.h"
#include "main.h"

/*  a few constants */

#define DEFAULT_INITIAL_OLD_VALUE  10000

#define DYSP_CNT_MULT 2.3

// Variables
int g_i_SWR_fixed_old = 0;
char g_work_str[7], g_work_str_2[7];
int g_i_Power = 0, g_i_Power_old = DEFAULT_INITIAL_OLD_VALUE;
int g_i_SWR_old = DEFAULT_INITIAL_OLD_VALUE;
char e_c_ledtype, g_b_Soft_tune = 0;
char g_b_Auto_mode = 0;

char g_b_Bypas_mode = 0;
char g_c_cap_mem = 0, g_c_ind_mem = 0, g_c_SW_mem = 0, g_c_Auto_mem = 0;

char g_b_Restart = 0;
char g_b_Test_mode = 0;
char g_b_lcd_prep_short = 0;
char g_b_L = 1;

char g_b_Loss_mode = 0;

/*  initial eeprom values*/
// PP5OO - Cell 31 changed from 0x10 to 0x11 because of error on transformer winding.
__eeprom unsigned char initial_eeprom[256] = {
    0x78, 0x01, 0x01, 0x15, 0x13, 0x01, 0x00, 0x00, 0x02, 0x00, 0x07, 0x00, 0x07, 0x00, 0x01, 0x00,
    0x00, 0x50, 0x01, 0x10, 0x02, 0x20, 0x04, 0x50, 0x10, 0x00, 0x22, 0x00, 0x45, 0x00, 0xff, 0xff,
    0x00, 0x10, 0x00, 0x22, 0x00, 0x47, 0x01, 0x00, 0x02, 0x20, 0x04, 0x70, 0x10, 0x00, 0xff, 0xff,
    0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x01, 0x00, 0x00,
};

void __interrupt() isr(void) {
    if (TMR0IE && TMR0IF) {
        TMR0 = 256 - (4000000 / 9600 / 2) + 7;
        TMR0IF = 0;
        uartProcessInput();
        uartProcessOutput();
    }
}

void interrupt_init(void) {
    OPTION_REGbits.PS = 0b000; // 1:2 Prescaller
    OPTION_REGbits.PSA = 0; // Prescaller assigned to Timer0
    OPTION_REGbits.T0CS = 0; // Timer increments on instruction clock
    INTCONbits.T0IE = 1; // Enable interrupt on TMR0 overflow
    INTCONbits.PEIE = 1; // Enable peripheral interrupt
    INTCONbits.GIE = 1; // Enable global interrupt
}

void main() {

    if (STATUSbits.nTO == 0)
        g_b_Restart = 1;

    pic_init();

    interrupt_init();

    //
    Delay_ms(300);
    CLRWDT();
    cells_init();

    if (e_c_ledtype == 0) { // 2-colors led  reset  turn both off
        GREEN_LED = LED_OFF;
        RED_LED = LED_OFF;
    }

    //
    Delay_ms(300);
    CLRWDT();

    if (e_c_num_L_q == 5)
        g_c_L_mult = 1;
    else if (e_c_num_L_q == 6)
        g_c_L_mult = 2;
    else if (e_c_num_L_q == 7)
        g_c_L_mult = 4;
    if (e_c_num_C_q == 5)
        g_c_C_mult = 1;
    else if (e_c_num_C_q == 6)
        g_c_C_mult = 2;
    else if (e_c_num_C_q == 7)
        g_c_C_mult = 4;

    Delay_ms(300);
    CLRWDT();
    Delay_ms(300);
    CLRWDT();
    Delay_ms(300);
    CLRWDT();
    Delay_ms(300);
    CLRWDT();
    Delay_ms(300);
    CLRWDT();

    //
    if (g_b_Test_mode == 0) {
        g_c_cap = eeprom_read(EEPROM_LAST_CAP);
        g_c_ind = eeprom_read(EEPROM_LAST_IND);
        g_c_SW = eeprom_read(EEPROM_LAST_SW);
        g_i_swr_a = eeprom_read(EEPROM_LAST_SWR_H) * 256;
        g_i_swr_a += eeprom_read(EEPROM_LAST_SWR_L);
        set_ind(g_c_ind);
        set_cap(g_c_cap);
        set_sw(g_c_SW);
        if (g_b_Restart == 1)
            g_b_lcd_prep_short = 1;
        lcd_prep();
    } else
        Test_init();

    lcd_ind();

    //*******************************

    while (1) {
        CLRWDT();
        lcd_pwr();
        //
        if (g_b_Test_mode == 0)
            button_proc();
        else
            button_proc_test();
        //
//        if (g_b_Test_mode == 0 & g_b_display_onoff == 1) {
//            if (e_c_b_Relay_off) {
//                set_ind(0);
//                set_cap(0);
//                set_sw(0);
//            }
//            g_b_display_onoff = 0;
//        }

        // next While code
    }
}

//***************** Routines *****************

void button_proc_test(void) {
    if (Button(&PORTB, TUNE_BUTTON, 50, BUTTON_PRESSED)) { // Tune btn
        Delay_ms(250);
        CLRWDT();
        if (PORTB_TUNE_BUTTON == BUTTON_RELEASED) { // short press button
            if (g_c_SW == 0)
                g_c_SW = 1;
            else
                g_c_SW = 0;
            set_sw(g_c_SW);
            lcd_ind();
        } else { // long press button
            if (g_b_L == 1)
                g_b_L = 0;
            else
                g_b_L = 1;
            if (g_b_L == 1) {
                if (e_c_ledtype == 4 | e_c_ledtype == 5) // 128*64 OLED
                    uart_wr_str(0, 16 + 12 * 8, "l", 1);
                else if (e_c_ledtype != 0) // 1602 LCD & 128*32 OLED
                    uart_wr_str(0, 8, "l", 1);
            } else {
                if (e_c_ledtype == 4 | e_c_ledtype == 5) // 128*64 OLED
                    uart_wr_str(0, 16 + 12 * 8, "c", 1);
                else if (e_c_ledtype != 0) // 1602 LCD & 128*32 OLED
                    uart_wr_str(0, 8, "c", 1);
            }
        }
        while (Button(&PORTB, TUNE_BUTTON, 50, BUTTON_PRESSED)) {
            lcd_pwr();
            CLRWDT();
        }
    } // END Tune btn
    //
    if (Button(&PORTB, BYPASS_BUTTON, 50, BUTTON_PRESSED)) { // BYP button
        CLRWDT();
        while (PORTB_BYPASS_BUTTON == BUTTON_PRESSED) {
            if (g_b_L & (g_c_ind < 32 * g_c_L_mult - 1)) {
                g_c_ind++;
                set_ind(g_c_ind);
            } else if (!g_b_L & (g_c_cap < 32 * g_c_L_mult - 1)) {
                g_c_cap++;
                set_cap(g_c_cap);
            }
            lcd_ind();
            lcd_pwr();
            Delay_ms(30);
            CLRWDT();
        }
    } // end of BYP button
    //
    if (Button(&PORTB, AUTO_BUTTON, 50, BUTTON_PRESSED) & (g_b_Bypas_mode == 0)) { // g_b_Auto_mode button
        CLRWDT();
        while (PORTB_AUTO_BUTTON == BUTTON_PRESSED) {
            if (g_b_L & (g_c_ind > 0)) {
                g_c_ind--;
                set_ind(g_c_ind);
            } else if (!g_b_L & (g_c_cap > 0)) {
                g_c_cap--;
                set_cap(g_c_cap);
            }
            lcd_ind();
            lcd_pwr();
            Delay_ms(30);
            CLRWDT();
        }
    }
    return;
}

void button_proc(void) {
    char uartChar = uartGetChar();

    // RESET
    if (uartChar == 'r') {
        Delay_ms(250);
        CLRWDT();
        show_reset();
        g_b_Bypas_mode = 0;
    }

    // TUNE
    if (uartChar == 't' || g_b_Soft_tune) {
        Delay_ms(250);
        CLRWDT();
        p_Tx = 1; //
        n_Tx = 0; // TX request
        Delay_ms(250); //
        tune_btn_push();
        g_b_Bypas_mode = 0;
        g_b_Soft_tune = 0;
    }


    // BYPASS
    if (uartChar == 'b') {
        CLRWDT();
        if (g_b_Bypas_mode == 0) {
            g_b_Bypas_mode = 1;
            g_c_cap_mem = g_c_cap;
            g_c_ind_mem = g_c_ind;
            g_c_SW_mem = g_c_SW;
            g_c_cap = 0;
            if (e_c_b_L_invert)
                g_c_ind = 255;
            else
                g_c_ind = 0;
            g_c_SW = 1;
            set_ind(g_c_ind);
            set_cap(g_c_cap);
            set_sw(g_c_SW);
            if (g_b_Loss_mode == 0)
                lcd_ind();
            g_c_Auto_mem = g_b_Auto_mode;
            g_b_Auto_mode = 0;
        } else {
            g_b_Bypas_mode = 0;
            g_c_cap = g_c_cap_mem;
            g_c_ind = g_c_ind_mem;
            g_c_SW = g_c_SW_mem;
            set_cap(g_c_cap);
            set_ind(g_c_ind);
            set_sw(g_c_SW);
            if (g_b_Loss_mode == 0)
                lcd_ind();
            g_b_Auto_mode = g_c_Auto_mem;
        }
        if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
            if (g_b_Auto_mode & !g_b_Bypas_mode)
                uart_wr_str(0, 16 + 8 * 12, ".", 1);
            else if ((!g_b_Auto_mode) & g_b_Bypas_mode)
                uart_wr_str(0, 16 + 8 * 12, "_", 1);
            else
                uart_wr_str(0, 16 + 8 * 12, " ", 1);
        } else if (e_c_ledtype != 0) { //  1602 LCD  or 128*32 OLED
            if (g_b_Auto_mode & !g_b_Bypas_mode)
                uart_wr_str(0, 8, ".", 1);
            else if ((!g_b_Auto_mode) & g_b_Bypas_mode)
                uart_wr_str(0, 8, "_", 1);
            else
                uart_wr_str(0, 8, " ", 1);
        }
        CLRWDT();
    }

    // AUTO
    if (uartChar == 'a' && g_b_Bypas_mode == 0) {
        CLRWDT();
        if (g_b_Auto_mode == 0)
            g_b_Auto_mode = 1;
        else
            g_b_Auto_mode = 0;
        eeprom_write(EEPROM_AUTOMATIC_MODE, g_b_Auto_mode);
        if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
            if (g_b_Auto_mode & !g_b_Bypas_mode)
                uart_wr_str(0, 16 + 8 * 12, ".", 1);
            else if ((!g_b_Auto_mode) & g_b_Bypas_mode)
                uart_wr_str(0, 16 + 8 * 12, "_", 1);
            else
                uart_wr_str(0, 16 + 8 * 12, " ", 1);
        } else if (e_c_ledtype != 0) { //  1602 LCD  or 128*32 OLED
            if (g_b_Auto_mode & !g_b_Bypas_mode)
                uart_wr_str(0, 8, ".", 1);
            else if ((!g_b_Auto_mode) & g_b_Bypas_mode)
                uart_wr_str(0, 8, "_", 1);
            else
                uart_wr_str(0, 8, " ", 1);
        }
        CLRWDT();
    }
    return;
}

void show_reset() {
    atu_reset();
    g_c_SW = 1;
    set_sw(g_c_SW);
    eeprom_write(EEPROM_LAST_CAP, 0);
    eeprom_write(EEPROM_LAST_IND, 0);
    eeprom_write(EEPROM_LAST_SW, 1);
    eeprom_write(EEPROM_LAST_SWR_H, 0);
    eeprom_write(EEPROM_LAST_SWR_L, 0);
    lcd_ind();
    g_b_Loss_mode = 0;
    p_Tx = 0;
    n_Tx = 1;
    g_i_SWR = 0;
    g_i_PWR = 0;
    g_i_SWR_fixed_old = 0;
    if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
        uart_wr_str(2, 16, "RESET   ", 8);
        CLRWDT();
        Delay_ms(600);
        uart_wr_str(2, 16, "SWR=0.00", 8);
        CLRWDT();
    } else if (e_c_ledtype != 0) { // 1602 LCD & 128*32 OLED
        uart_wr_str(1, 0, "RESET   ", 8);
        CLRWDT();
        Delay_ms(600);
        uart_wr_str(1, 0, "SWR=0.00", 8);
        CLRWDT();
    } else {
        GREEN_LED = LED_OFF; /* turn off both LED's */
        RED_LED = LED_OFF;
    }
    g_i_SWR_old = DEFAULT_INITIAL_OLD_VALUE;
    g_i_Power_old = DEFAULT_INITIAL_OLD_VALUE;
    lcd_pwr();
    return;
}

void tune_btn_push() {
    CLRWDT();
    if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
        uart_wr_str(2, 16 + 12 * 4, "TUNE", 4);
    } else if (e_c_ledtype != 0) { // 1602 LCD & 128*32 OLED
        uart_wr_str(1, 4, "TUNE", 4);
    } else {
        GREEN_LED = LED_OFF; /* turn off both LED's */
        RED_LED = LED_OFF;
    }
    tune();
    if (e_c_ledtype == 0) { // real-time 2-colors led work
        if (g_i_SWR <= 150) {
            GREEN_LED = LED_ON; /* Is SWR < 1.5 then turn on green */
            RED_LED = LED_OFF;
        }// Green
        else if (g_i_SWR <= 250) {
            GREEN_LED = LED_ON; /* Is SWR < 2.5 then turn on both = orange */
            RED_LED = LED_ON;
        }// Orange
        else {
            GREEN_LED = LED_OFF;
            RED_LED = LED_ON; /* Is SWR > 2.5 then turn on red */
        } // Red
    } else if (g_b_Loss_mode == 0 | e_c_b_Loss_ind == 0)
        lcd_ind();
    eeprom_write(EEPROM_LAST_CAP, g_c_cap);
    eeprom_write(EEPROM_LAST_IND, g_c_ind);
    eeprom_write(EEPROM_LAST_SW, g_c_SW);
    eeprom_write(EEPROM_LAST_SWR_H, (char) (g_i_swr_a / 256));
    eeprom_write(EEPROM_LAST_SWR_L, (char) (g_i_swr_a % 256));
    g_i_SWR_old = DEFAULT_INITIAL_OLD_VALUE;
    g_i_Power_old = DEFAULT_INITIAL_OLD_VALUE;
    lcd_pwr();
    g_i_SWR_fixed_old = g_i_SWR;
    p_Tx = 0;
    n_Tx = 1;
    CLRWDT();
    return;
}

void lcd_prep() {
    CLRWDT();
    if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
        if (g_b_lcd_prep_short == 0) {
            uart_wr_str(0, 22, "ATU-100", 7);
            uart_wr_str(2, 6, "EXT board", 9);
            uart_wr_str(4, 16, "by N7DDC", 8);
            uart_wr_str(6, 4, "FW ver 3.2", 10);
            CLRWDT();
            Delay_ms(600);
            CLRWDT();
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(0, 16, "        ", 8);
            uart_wr_str(2, 4, "          ", 10);
            uart_wr_str(4, 16, "        ", 8);
            uart_wr_str(6, 4, "          ", 10);
        }
        Delay_ms(150);
        if (e_c_b_P_High == 1)
            uart_wr_str(0, 16, "PWR=  0W", 8);
        else
            uart_wr_str(0, 16, "PWR=0.0W", 8);
        uart_wr_str(2, 16, "SWR=0.00", 8);
        if (g_b_Auto_mode)
            uart_wr_str(0, 16 + 8 * 12, ".", 1);
    } else if (e_c_ledtype != 0) { // 1602 LCD & 128*32 OLED
        if (g_b_lcd_prep_short == 0) {
            uart_wr_str(0, 4, "ATU-100", 7);
            uart_wr_str(1, 3, "EXT board", 9);
            CLRWDT();
            Delay_ms(700);
            CLRWDT();
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(0, 4, "by N7DDC", 8);
            uart_wr_str(1, 3, "FW ver 3.2", 10);
            CLRWDT();
            Delay_ms(600);
            CLRWDT();
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(0, 4, "        ", 8);
            uart_wr_str(1, 3, "          ", 10);
        }
        Delay_ms(150);
        if (e_c_b_P_High == 1)
            uart_wr_str(0, 0, "PWR=  0W", 8);
        else
            uart_wr_str(0, 0, "PWR=0.0W", 8);
        uart_wr_str(1, 0, "SWR=0.00", 8);
        if (g_b_Auto_mode)
            uart_wr_str(0, 8, ".", 1);
    }
    CLRWDT();
    lcd_ind();
    return;
}

void lcd_swr(int swr) {
    CLRWDT();
    if (swr != g_i_SWR_old) {
        g_i_SWR_old = swr;
        if (swr == 0) { // Low power
            if (e_c_ledtype == 4 | e_c_ledtype == 5)
                uart_wr_str(2, 16 + 4 * 12, "0.00", 4); // 128*64 OLED
            else if (e_c_ledtype != 0)
                uart_wr_str(1, 4, "0.00", 4); // 1602  & 128*32 OLED
            else if (e_c_ledtype == 0) { // real-time 2-colors led work
                GREEN_LED = LED_OFF; /* turn off both leds */
                RED_LED = LED_OFF;
            }
            g_i_SWR_old = 0;
        } else {
            IntToStr(swr, g_work_str);
            g_work_str_2[0] = g_work_str[3];
            g_work_str_2[1] = '.';
            g_work_str_2[2] = g_work_str[4];
            g_work_str_2[3] = g_work_str[5];
            if (e_c_ledtype == 4 | e_c_ledtype == 5)
                uart_wr_str(2, 16 + 4 * 12, g_work_str_2, 4); // 128*64 OLED
            else if (e_c_ledtype != 0)
                uart_wr_str(1, 4, g_work_str_2, 4); // 1602  & 128*32
            else if (e_c_ledtype == 0) { // real-time 2-colors led work
                if (swr <= 150) {
                    GREEN_LED = LED_ON; /* Is SWR < 1.5 then turn on green */
                    RED_LED = LED_OFF;
                }// Green
                else if (swr <= 250) {
                    GREEN_LED = LED_ON; /* Is SWR < 2.5 then turn on both */
                    RED_LED = LED_ON;
                }// Orange
                else {
                    GREEN_LED = LED_OFF; /* Is SWR > 2.5 then turn on red */
                    RED_LED = LED_ON;
                } // Red
            }
        }
    }
    CLRWDT();
    return;
}

void show_pwr(int parm_Power, int parm_SWR) {
    int p_ant, eff;
    double a, b;
    a = 100;
    CLRWDT();
    //
    if (g_b_Test_mode == 0 & e_c_b_Loss_ind == 1 & parm_Power >= 10) {
        if (g_b_Loss_mode == 0) { // prepare
            if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
                if (e_c_b_P_High == 1)
                    uart_wr_str(4, 16, "ANT=  0W", 8);
                else
                    uart_wr_str(4, 16, "ANT=0.0W", 8);
                uart_wr_str(6, 16, "EFF=  0%", 8);
            } else if (e_c_ledtype == 2 | e_c_ledtype == 3) { // 128*32 OLED
                if (e_c_b_P_High == 1)
                    uart_wr_str(0, 9, "ANT=  0W", 8);
                else
                    uart_wr_str(0, 9, "ANT=0.0W", 8);
                uart_wr_str(1, 9, "EFF=  0%", 8);
            } else if (e_c_ledtype == 1) { // 1602 LCD
                if (e_c_b_P_High == 1)
                    uart_wr_str(0, 9, "AN=  0W", 7);
                else
                    uart_wr_str(0, 9, "AN=0.0W", 7);
                uart_wr_str(1, 9, "EFF= 0%", 7);
            }
        }
        g_b_Loss_mode = 1;
    } else {
        if (g_b_Loss_mode == 1)
            lcd_ind();
        g_b_Loss_mode = 0;
    }
    CLRWDT();
    if (parm_Power != g_i_Power_old) {
        g_i_Power_old = parm_Power;
        //
        if (e_c_b_P_High == 0) {
            if (parm_Power >= 100) { // > 10 W
                parm_Power += 5; // rounding to 1 W
                IntToStr(parm_Power, g_work_str);
                g_work_str_2[0] = g_work_str[2];
                g_work_str_2[1] = g_work_str[3];
                g_work_str_2[2] = g_work_str[4];
                g_work_str_2[3] = 'W';
            } else {
                IntToStr(parm_Power, g_work_str);
                if (g_work_str[4] != ' ')
                    g_work_str_2[0] = g_work_str[4];
                else
                    g_work_str_2[0] = '0';
                g_work_str_2[1] = '.';
                if (g_work_str[5] != ' ')
                    g_work_str_2[2] = g_work_str[5];
                else
                    g_work_str_2[2] = '0';
                g_work_str_2[3] = 'W';
            }
        } else { // High g_i_Power
            if (parm_Power < 999) { // 0 - 999 Watt
                IntToStr(parm_Power, g_work_str);
                g_work_str_2[0] = g_work_str[3];
                g_work_str_2[1] = g_work_str[4];
                g_work_str_2[2] = g_work_str[5];
                g_work_str_2[3] = 'W';
            } else {
                IntToStr(parm_Power, g_work_str);
                g_work_str_2[0] = g_work_str[2];
                g_work_str_2[1] = g_work_str[3];
                g_work_str_2[2] = g_work_str[4];
                g_work_str_2[3] = g_work_str[5];
            }
        }
        if (e_c_ledtype == 4 | e_c_ledtype == 5)
            uart_wr_str(0, 16 + 4 * 12, g_work_str_2, 4); // 128*64 OLED
        else if (e_c_ledtype != 0)
            uart_wr_str(0, 4, g_work_str_2, 4); // 1602  & 128*32
        //
        CLRWDT();
        //  Loss indication
        if (g_b_Loss_mode == 1) {
            if (g_c_ind == 0 & g_c_cap == 0)
                g_i_swr_a = parm_SWR;
            a = 1.0 / ((g_i_swr_a / 100.0 + 100.0 / g_i_swr_a) * e_c_tenths_Fid_loss / 10.0 * 0.115 + 1.0); // Fider loss
            b = 4.0 / (2.0 + parm_SWR / 100.0 + 100.0 / parm_SWR); // parm_SWR loss
            if (a >= 1.0)
                a = 1.0;
            if (b >= 1.0)
                b = 1.0;
            p_ant = (int) (parm_Power * a * b);
            eff = (int) (a * b * 100);
            if (eff >= 100)
                eff = 99;
            //
            if (e_c_b_P_High == 0) {
                if (p_ant >= 100) { // > 10 W
                    p_ant += 5; // rounding to 1 W
                    IntToStr(p_ant, g_work_str);
                    g_work_str_2[0] = g_work_str[2];
                    g_work_str_2[1] = g_work_str[3];
                    g_work_str_2[2] = g_work_str[4];
                    g_work_str_2[3] = 'W';
                } else {
                    IntToStr(p_ant, g_work_str);
                    if (g_work_str[4] != ' ')
                        g_work_str_2[0] = g_work_str[4];
                    else
                        g_work_str_2[0] = '0';
                    g_work_str_2[1] = '.';
                    if (g_work_str[5] != ' ')
                        g_work_str_2[2] = g_work_str[5];
                    else
                        g_work_str_2[2] = '0';
                    g_work_str_2[3] = 'W';
                }
            } else { // High g_i_Power
                if (p_ant < 999) { // 0 - 1500 Watts
                    IntToStr(p_ant, g_work_str);
                    g_work_str_2[0] = g_work_str[3];
                    g_work_str_2[1] = g_work_str[4];
                    g_work_str_2[2] = g_work_str[5];
                    g_work_str_2[3] = 'W';
                } else {
                    IntToStr(p_ant, g_work_str);
                    g_work_str_2[0] = g_work_str[2];
                    g_work_str_2[1] = g_work_str[3];
                    g_work_str_2[2] = g_work_str[4];
                    g_work_str_2[3] = g_work_str[5];
                }
            }
            if (e_c_ledtype == 4 | e_c_ledtype == 5)
                uart_wr_str(4, 16 + 4 * 12, g_work_str_2, 4); // 128*64 OLED
            else if (e_c_ledtype == 2 | e_c_ledtype == 3)
                uart_wr_str(0, 13, g_work_str_2, 4); // 128*32
            else if (e_c_ledtype != 0)
                uart_wr_str(0, 12, g_work_str_2, 4); // 1602
            //
            IntToStr(eff, g_work_str);
            g_work_str_2[0] = g_work_str[4];
            g_work_str_2[1] = g_work_str[5];
            if (e_c_ledtype == 4 | e_c_ledtype == 5)
                uart_wr_str(6, 16 + 5 * 12, g_work_str_2, 2);
            else if (e_c_ledtype == 2 | e_c_ledtype == 3)
                uart_wr_str(1, 14, g_work_str_2, 2);
            else if (e_c_ledtype == 1)
                uart_wr_str(1, 13, g_work_str_2, 2);
        }
    }
    CLRWDT();
    return;
}

void lcd_pwr() {
    int p = 0;
    char peak_cnt;
    int delta = e_i_tenths_SWR_Auto_delta - 100;
    char cnt;
    int SWR_fixed;
    g_i_PWR = 0;
    CLRWDT();

    // peak detector
    cnt = 120;
    for (peak_cnt = 0; peak_cnt < cnt; peak_cnt++) {
        get_pwr();
        if (g_i_PWR > p) {
            p = g_i_PWR;
            SWR_fixed = g_i_SWR;
        }
        Delay_ms(3);
    }
    CLRWDT();
    if (p >= 100) {
        p = (p + 5) / 10;
        p *= 10;
    } // round to 1 W if more then 100 W
    g_i_Power = p;
    if (g_i_Power < 10)
        SWR_fixed = 0;
    lcd_swr(SWR_fixed);
    //
    if (g_b_Auto_mode & (SWR_fixed >= e_i_tenths_SWR_Auto_delta) & ((SWR_fixed > g_i_SWR_fixed_old & (SWR_fixed - g_i_SWR_fixed_old) > delta) | (SWR_fixed < g_i_SWR_fixed_old & (g_i_SWR_fixed_old - SWR_fixed) > delta) | g_i_SWR_fixed_old == 999))
        g_b_Soft_tune = 1;

    show_pwr(g_i_Power, SWR_fixed);

    CLRWDT();
    if (g_b_Overload == 1) {
        if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
            uart_wr_str(2, 16, "        ", 8);
            Delay_ms(100);
            uart_wr_str(2, 16, "OVERLOAD", 8);
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(2, 16, "        ", 8);
            Delay_ms(300);
            CLRWDT();
            uart_wr_str(2, 16, "OVERLOAD", 8);
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(2, 16, "        ", 8);
            Delay_ms(300);
            CLRWDT();
            uart_wr_str(2, 16, "OVERLOAD", 8);
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(2, 16, "        ", 8);
            Delay_ms(100);
            uart_wr_str(2, 16, "SWR=    ", 8);
        } else if (e_c_ledtype != 0) { // 1602  & 128*32
            uart_wr_str(1, 0, "        ", 8);
            Delay_ms(100);
            uart_wr_str(1, 0, "OVERLOAD", 8);
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(1, 0, "        ", 8);
            Delay_ms(300);
            CLRWDT();
            uart_wr_str(1, 0, "OVERLOAD", 8);
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(1, 0, "        ", 8);
            Delay_ms(300);
            CLRWDT();
            uart_wr_str(1, 0, "OVERLOAD", 8);
            Delay_ms(500);
            CLRWDT();
            uart_wr_str(1, 0, "        ", 8);
            Delay_ms(100);
            uart_wr_str(1, 0, "SW=     ", 8);
        }
        CLRWDT();
        g_i_SWR_old = DEFAULT_INITIAL_OLD_VALUE;
        lcd_swr(SWR_fixed);
    }
    return;
}

void lcd_ind() {
    char l_line;
    int l_work_int;
    CLRWDT();
    /*  if (1)
      {  */
    charbits indbits;
    indbits.bytes = g_c_ind;
    l_work_int = 0;
    if (indbits.bits.B0)
        l_work_int += e_i_Ind1;
    if (indbits.bits.B1)
        l_work_int += e_i_Ind2;
    if (indbits.bits.B2)
        l_work_int += e_i_Ind3;
    if (indbits.bits.B3)
        l_work_int += e_i_Ind4;
    if (indbits.bits.B4)
        l_work_int += e_i_Ind5;
    if (indbits.bits.B5)
        l_work_int += e_i_Ind6;
    if (indbits.bits.B6)
        l_work_int += e_i_Ind7;
    if (l_work_int > 9999) { // more then 9999 nH
        l_work_int += 50; // round
        IntToStr(l_work_int, g_work_str);
        g_work_str_2[0] = g_work_str[1];
        g_work_str_2[1] = g_work_str[2];
        g_work_str_2[2] = '.';
        g_work_str_2[3] = g_work_str[3];
    } else {
        IntToStr(l_work_int, g_work_str);
        if (g_work_str[2] != ' ')
            g_work_str_2[0] = g_work_str[2];
        else
            g_work_str_2[0] = '0';
        g_work_str_2[1] = '.';
        if (g_work_str[3] != ' ')
            g_work_str_2[2] = g_work_str[3];
        else
            g_work_str_2[2] = '0';
        if (g_work_str[4] != ' ')
            g_work_str_2[3] = g_work_str[4];
        else
            g_work_str_2[3] = '0';
    }
    if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
        if (g_c_SW == 1)
            l_line = 4;
        else
            l_line = 6;
        uart_wr_str(l_line, 16, "L=", 2);
        uart_wr_str(l_line, 16 + 6 * 12, "uH", 2);
        uart_wr_str(l_line, 16 + 2 * 12, g_work_str_2, 4);
    } else if (e_c_ledtype == 2 | e_c_ledtype == 3) { // 128*32 OLED
        if (g_c_SW == 1)
            l_line = 0;
        else
            l_line = 1;
        uart_wr_str(l_line, 9, "L=", 2);
        uart_wr_str(l_line, 15, "uH", 2);
        uart_wr_str(l_line, 11, g_work_str_2, 4);
    } else if (e_c_ledtype == 1) { //  1602 LCD
        if (g_c_SW == 1)
            l_line = 0;
        else
            l_line = 1;
        uart_wr_str(l_line, 9, "L=", 2);
        uart_wr_str(l_line, 15, "u", 1);
        uart_wr_str(l_line, 11, g_work_str_2, 4);
    }
    /*  }  */
    CLRWDT();
    /*  if (1)
      {  */
    l_work_int = 0;
    charbits capbits;
    capbits.bytes = g_c_cap;
    if (capbits.bits.B0)
        l_work_int += e_i_Cap1;
    if (capbits.bits.B1)
        l_work_int += e_i_Cap2;
    if (capbits.bits.B2)
        l_work_int += e_i_Cap3;
    if (capbits.bits.B3)
        l_work_int += e_i_Cap4;
    if (capbits.bits.B4)
        l_work_int += e_i_Cap5;
    if (capbits.bits.B5)
        l_work_int += e_i_Cap6;
    if (capbits.bits.B6)
        l_work_int += e_i_Cap7;
    IntToStr(l_work_int, g_work_str);
    g_work_str_2[0] = g_work_str[2];
    g_work_str_2[1] = g_work_str[3];
    g_work_str_2[2] = g_work_str[4];
    g_work_str_2[3] = g_work_str[5];
    //
    if (e_c_ledtype == 4 | e_c_ledtype == 5) { // 128*64 OLED
        if (g_c_SW == 1)
            l_line = 6;
        else
            l_line = 4;
        uart_wr_str(l_line, 16, "C=", 2);
        uart_wr_str(l_line, 16 + 6 * 12, "pF", 2);
        uart_wr_str(l_line, 16 + 2 * 12, g_work_str_2, 4);
    } else if (e_c_ledtype == 2 | e_c_ledtype == 3) { // 128*32 OLED
        if (g_c_SW == 1)
            l_line = 1;
        else
            l_line = 0;
        uart_wr_str(l_line, 9, "C=", 2);
        uart_wr_str(l_line, 15, "pF", 2);
        uart_wr_str(l_line, 11, g_work_str_2, 4);
    } else if (e_c_ledtype == 1) { // 1602 LCD
        if (g_c_SW == 1)
            l_line = 1;
        else
            l_line = 0;
        uart_wr_str(l_line, 9, "C=", 2);
        uart_wr_str(l_line, 15, "p", 1);
        uart_wr_str(l_line, 11, g_work_str_2, 4);
    }
    /*   }  */
    CLRWDT();
    return;
}

void Test_init(void) { // g_b_Test_mode mode
    if (e_c_ledtype == 4 | e_c_ledtype == 5) // 128*64 OLED
        uart_wr_str(0, 10, "TEST MODE", 9);
    else if (e_c_ledtype != 0) // 1602 LCD  or 128*32 OLED
        uart_wr_str(0, 3, "TEST MODE", 9);
    CLRWDT();
    Delay_ms(500);
    CLRWDT();
    Delay_ms(500);
    CLRWDT();
    Delay_ms(500);
    CLRWDT();
    Delay_ms(500);
    CLRWDT();
    if (e_c_ledtype == 4 | e_c_ledtype == 5) // 128*64 OLED
        uart_wr_str(0, 10, "         ", 9);
    else if (e_c_ledtype != 0) // 1602 LCD  or 128*32 OLED
        uart_wr_str(0, 3, "         ", 9);
    atu_reset();
    g_c_SW = 1; // C to OUT
    set_sw(g_c_SW);
    eeprom_write(EEPROM_LAST_CAP, g_c_cap);
    eeprom_write(EEPROM_LAST_IND, g_c_ind);
    eeprom_write(EEPROM_LAST_SW, g_c_SW);
    //
    if (e_c_ledtype == 4 | e_c_ledtype == 5) // 128*64 OLED
        uart_wr_str(0, 16 + 12 * 8, "l", 1);
    else if (e_c_ledtype != 0) // 1602 LCD or 128*32 OLED
        uart_wr_str(0, 8, "l", 1);
    //
    g_b_lcd_prep_short = 1;
    lcd_prep();
    return;
}

void cells_init(void) {
    // Cells init
    CLRWDT();
    //oled_addr = eeprom_read(0); // address
    e_c_ledtype = eeprom_read(EEPROM_DISPLAY_TYPE); // e_c_ledtype of display
    if (eeprom_read(EEPROM_AUTOMATIC_MODE) == 1)
        g_b_Auto_mode = 1;
    e_i_ms_Rel_Del = Bcd2Dec(eeprom_read(EEPROM_TIMEOUT_TIME)); // Relay's Delay
    e_i_tenths_SWR_Auto_delta = Bcd2Dec(eeprom_read(EEPROM_SWR_THRESHOLD)) * 10; // e_i_tenths_SWR_Auto_delta
    e_i_watts_min_for_start = Bcd2Dec(eeprom_read(EEPROM_MIN_POWER)) * 10; // P_min_for_start
    e_i_watts_max_for_start = Bcd2Dec(eeprom_read(EEPROM_MAX_POWER)) * 10; // P_max_for_start
    // 7  - shift down
    // 8 - shift left
    e_i_tenths_init_max_swr = Bcd2Dec(eeprom_read(EEPROM_MAX_INIT_SWR)) * 10; // Max g_i_SWR
    e_c_num_L_q = eeprom_read(EEPROM_NUMBER_INDS);
    e_c_b_L_linear = eeprom_read(EEPROM_IND_LINEAR_PITCH);
    e_c_num_C_q = eeprom_read(EEPROM_NUMBER_CAPS);
    e_c_b_C_linear = eeprom_read(EEPROM_CAP_LINEAR_PITCH);
    e_c_b_D_correction = eeprom_read(EEPROM_ENABLE_NONLINEAR_DIODE);
    e_c_b_L_invert = eeprom_read(EEPROM_INVERSE_INDUCTANCE_RELAY);
    //
    CLRWDT();
    e_i_Ind1 = Bcd2Dec(eeprom_read(16)) * 100 + Bcd2Dec(eeprom_read(17)); // e_i_Ind1
    e_i_Ind2 = Bcd2Dec(eeprom_read(18)) * 100 + Bcd2Dec(eeprom_read(19)); // e_i_Ind2
    e_i_Ind3 = Bcd2Dec(eeprom_read(20)) * 100 + Bcd2Dec(eeprom_read(21)); // e_i_Ind3
    e_i_Ind4 = Bcd2Dec(eeprom_read(22)) * 100 + Bcd2Dec(eeprom_read(23)); // e_i_Ind4
    e_i_Ind5 = Bcd2Dec(eeprom_read(24)) * 100 + Bcd2Dec(eeprom_read(25)); // e_i_Ind5
    e_i_Ind6 = Bcd2Dec(eeprom_read(26)) * 100 + Bcd2Dec(eeprom_read(27)); // e_i_Ind6
    e_i_Ind7 = Bcd2Dec(eeprom_read(28)) * 100 + Bcd2Dec(eeprom_read(29)); // e_i_Ind7
    //
    e_i_Cap1 = Bcd2Dec(eeprom_read(32)) * 100 + Bcd2Dec(eeprom_read(33)); // e_i_Cap1
    e_i_Cap2 = Bcd2Dec(eeprom_read(34)) * 100 + Bcd2Dec(eeprom_read(35)); // e_i_Cap2
    e_i_Cap3 = Bcd2Dec(eeprom_read(36)) * 100 + Bcd2Dec(eeprom_read(37)); // e_i_Cap3
    e_i_Cap4 = Bcd2Dec(eeprom_read(38)) * 100 + Bcd2Dec(eeprom_read(39)); // e_i_Cap4
    e_i_Cap5 = Bcd2Dec(eeprom_read(40)) * 100 + Bcd2Dec(eeprom_read(41)); // e_i_Cap5
    e_i_Cap6 = Bcd2Dec(eeprom_read(42)) * 100 + Bcd2Dec(eeprom_read(43)); // e_i_Cap6
    e_i_Cap7 = Bcd2Dec(eeprom_read(44)) * 100 + Bcd2Dec(eeprom_read(45)); // e_i_Cap7
    //
    e_c_b_P_High = eeprom_read(EEPROM_POWER_MEASURE_LEVEL); // High power
    e_c_K_Mult = Bcd2Dec(eeprom_read(EEPROM_TANDEM_MATCH)); // Tandem Match rate

    e_c_b_Loss_ind = eeprom_read(EEPROM_ADDITIONAL_INDICATION);
    e_c_tenths_Fid_loss = Bcd2Dec(eeprom_read(EEPROM_FEEDER_LOSS));
    e_c_b_Relay_off = Bcd2Dec(eeprom_read(EEPROM_DISABLE_RELAYS));
    CLRWDT();
    return;
}

void show_loss(void) {
    IntToStr(e_c_tenths_Fid_loss, g_work_str);
    if (e_c_tenths_Fid_loss >= 10)
        g_work_str_2[0] = g_work_str[4];
    else
        g_work_str_2[0] = '0';
    g_work_str_2[1] = '.';
    g_work_str_2[2] = g_work_str[5];
    if (e_c_ledtype == 4 | e_c_ledtype == 5)
        uart_wr_str(4, 6, g_work_str_2, 3); // 128*64
    else if (e_c_ledtype != 0)
        uart_wr_str(1, 0, g_work_str_2, 3); // 1602 | 128*32
    return;
}
