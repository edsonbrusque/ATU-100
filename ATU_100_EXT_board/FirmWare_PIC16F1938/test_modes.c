#include "cross_compiler.h"

extern bool in_dummy;

void simulator(void) {
#ifdef SIMULATOR    
    /*  print in debug mode */
    char mystart[] = "Starting!";
    PRINTLINE(mystart)
    eeprom_write(EEPROM_DISPLAY_I2C_ADDR, 0x4E);
    eeprom_write(EEPROM_DISPLAY_TYPE, 4);
    eeprom_write(EEPROM_AUTOMATIC_MODE, 0);
    eeprom_write(EEPROM_TIMEOUT_TIME, 0x15);
    eeprom_write(EEPROM_SWR_THRESHOLD, 0x13);
    eeprom_write(EEPROM_MIN_POWER, 5);
    eeprom_write(EEPROM_MAX_POWER, 0);
    eeprom_write(EEPROM_DISPLAY_OFFSET_DOWN, 2);
    eeprom_write(EEPROM_DISPLAY_OFFSET_LEFT, 3);
    eeprom_write(EEPROM_MAX_INIT_SWR, 0);
    eeprom_write(EEPROM_NUMBER_INDS, 7);
    eeprom_write(EEPROM_IND_LINEAR_PITCH, 0);
    eeprom_write(EEPROM_NUMBER_CAPS, 7);
    eeprom_write(EEPROM_CAP_LINEAR_PITCH, 0);
    eeprom_write(EEPROM_ENABLE_NONLINEAR_DIODE, 1);
    eeprom_write(EEPROM_INVERSE_INDUCTANCE_RELAY, 0);

    eeprom_write(0x10, 0);
    eeprom_write(0x11, 0x50);
    eeprom_write(0x12, 1);
    eeprom_write(0x13, 0x10);
    eeprom_write(0x14, 2);
    eeprom_write(0x15, 0x20);
    eeprom_write(0x16, 4);
    eeprom_write(0x17, 0x50);
    eeprom_write(0x18, 0x10);
    eeprom_write(0x19, 0);
    eeprom_write(0x1a, 0x22);
    eeprom_write(0x1b, 0);
    eeprom_write(0x1c, 0x45);
    eeprom_write(0x1d, 0);

    eeprom_write(0x20, 0);
    eeprom_write(0x21, 0x10);
    eeprom_write(0x22, 0);
    eeprom_write(0x23, 0x22);
    eeprom_write(0x24, 0);
    eeprom_write(0x25, 0x47);
    eeprom_write(0x26, 1);
    eeprom_write(0x27, 0x00);
    eeprom_write(0x28, 2);
    eeprom_write(0x29, 0x20);
    eeprom_write(0x2a, 4);
    eeprom_write(0x2b, 0x70);
    eeprom_write(0x2c, 0x10);
    eeprom_write(0x2d, 0);


    eeprom_write(EEPROM_POWER_MEASURE_LEVEL, 0);
    eeprom_write(EEPROM_TANDEM_MATCH, 10);
    eeprom_write(EEPROM_DISPLAY_OFF_TIMER, 0);
    eeprom_write(EEPROM_ADDITIONAL_INDICATION, 1);
    eeprom_write(EEPROM_FEEDER_LOSS, 0x12);
    eeprom_write(EEPROM_DISABLE_RELAYS, 0x0);
    eeprom_write(EEPROM_LAST_SWR_L, 0x18);
    eeprom_write(EEPROM_LAST_SWR_H, 0);
    eeprom_write(EEPROM_LAST_SW, 0);
    eeprom_write(EEPROM_LAST_IND, 0);
    eeprom_write(EEPROM_LAST_CAP, 0);

#endif
}

//void testModes(void) {
//    //
//    /*  test mode?   enter step by step adjustments */
//    if (PORTB_AUTO_BUTTON == BUTTON_PRESSED & PORTB_BYPASS_BUTTON == BUTTON_PRESSED) { // g_b_Test_mode mode
//        g_b_Test_mode = 1;
//        g_b_Auto_mode = 0;
//    }
//
//    /*   if FAST TEST mode, then turn on all relays, and loop here forever */
//    if ((PORTB_AUTO_BUTTON == BUTTON_PRESSED) &&
//            (PORTB_BYPASS_BUTTON == BUTTON_PRESSED) &&
//            (PORTB_TUNE_BUTTON == BUTTON_PRESSED)) { // Fast g_b_Test_mode mode (loop)
//        if (e_c_ledtype == 4 | e_c_ledtype == 5)
//            uart_wr_str(0, 12, "FAST TEST", 9); // 128*64
//        else if (e_c_ledtype != 0)
//            uart_wr_str(0, 3, "FAST TEST", 9); // 1602 | 128*32
//        set_cap(255);
//        if (e_c_b_L_invert == 0)
//            set_ind(255);
//        else
//            set_ind(0);
//        set_sw(1);
//        CLRWDT();
//        while (1) {
//            Delay_ms(500);
//            CLRWDT();
//        }
//    }
//    /*   end of FAST TEST code */
//
//
//    /*  feeder loss mode */
//    if (Button(&PORTB, TUNE_BUTTON, 100, BUTTON_PRESSED)) { //  Fider loss input
//        if (e_c_ledtype == 4 | e_c_ledtype == 5) {
//            uart_wr_str(0, 6, "Fider Loss", 10); // 128*64
//            uart_wr_str(2, 6, "input", 5);
//            uart_wr_str(4, 6 + 3 * 12, "dB", 2);
//        } else if (e_c_ledtype != 0) {
//            uart_wr_str(0, 0, "Fider Loss input", 16); // 1602 | 128*32
//            uart_wr_str(1, 3, "dB", 2);
//        }
//        e_c_tenths_Fid_loss = Bcd2Dec(eeprom_read(EEPROM_FEEDER_LOSS));
//        show_loss();
//        //
//        while (1) {
//            if (Button(&PORTB, BYPASS_BUTTON, 50, BUTTON_PRESSED)) { // BYP button
//                if (e_c_tenths_Fid_loss < 99) {
//                    e_c_tenths_Fid_loss++;
//                    show_loss();
//                    eeprom_write(EEPROM_FEEDER_LOSS, Dec2Bcd(e_c_tenths_Fid_loss));
//                }
//                while (Button(&PORTB, BYPASS_BUTTON, 50, BUTTON_PRESSED))
//                    CLRWDT();
//            }
//            //
//            if (Button(&PORTB, AUTO_BUTTON, 50, BUTTON_PRESSED)) { // AUTO button
//                if (e_c_tenths_Fid_loss > 0) {
//                    e_c_tenths_Fid_loss--;
//                    show_loss();
//                    eeprom_write(EEPROM_FEEDER_LOSS, Dec2Bcd(e_c_tenths_Fid_loss));
//                }
//                while (Button(&PORTB, AUTO_BUTTON, 50, BUTTON_PRESSED))
//                    CLRWDT();
//            }
//            CLRWDT();
//        } // while
//    } //  Fider loss input
//    /*  end of feeder input loss code */
//}
