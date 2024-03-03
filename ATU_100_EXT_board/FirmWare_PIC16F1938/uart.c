/*  oled_control.c */
#include "uart.h"

#define MYONE 1
#define MYZERO 0

#define CHAR_TAB 9
#define CHAR_LF 10
#define CHAR_CR 13


#define BUFFER_LEN (32+5)
static char buffer[BUFFER_LEN] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, CHAR_TAB,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0x20, 0x20, CHAR_CR, CHAR_LF
};

bool refresh = false;

void uartProcessOutput(void) {
    static uint8_t strPosition = 0;
    static uint8_t chrPosition = 0;

    static uint8_t buf = 0x00;

    if (chrPosition == 0) {
        if (refresh) {
            if (strPosition < BUFFER_LEN) {
                UART_OUT_PIN = MYZERO; // Start bit
                buf = buffer[strPosition];
            } else {
                buf = 0xFF;
            }
            chrPosition++;
        }
    } else if (chrPosition <= 8) {
        if (buf & 0x01) {
            UART_OUT_PIN = MYONE;
        } else {
            UART_OUT_PIN = MYZERO;
        }
        buf >>= 1;
        chrPosition++;
    } else if (chrPosition <= 10) {
        UART_OUT_PIN = MYONE; // Stop bit
        chrPosition++;
    } else {
        chrPosition = 0;
        strPosition++;
        if (strPosition == BUFFER_LEN) {
            refresh = false;
        } else if (strPosition > 137) {
            strPosition = 0;
        }
    }
}

void uart_wr_str(char lin, char col, char str[], char len) {
    char pos = lin * 16 + col;
    
    if (pos >= 16) pos++;
    
    for (uint8_t i = 0; i < len; i++) {
        if (pos == 16) {
            buffer[pos++] = CHAR_TAB;
        }
        buffer[pos++] = str[i];
    }
    refresh = true;
}

////////////////////////////////////////

char inbuf = 0;

enum state_e {
    START_BIT,
    DATA_BIT,
    STOP_BIT,
};

void uartProcessInput(void) {
    static uint8_t state;
    static uint8_t buf;
    static uint8_t bitCounter = 0;

    bool inbit = UART_IN_PIN;

    switch (state) {
        case START_BIT:
            if (inbit == MYZERO) {
                bitCounter = 0;
                state = DATA_BIT;
            }
            break;
        case DATA_BIT:
            buf >>= 1;
            buf |= inbit << 7;

            if (++bitCounter == 8) {
                state = STOP_BIT;
            }
            break;
        case STOP_BIT:
            if (inbit == MYONE) {
                inbuf = buf;
                buffer[BUFFER_LEN - 3] = buf;
                refresh = true;
            }
            state = START_BIT;
            break;
    }
}

char uartGetChar(void) {
    char ret = inbuf;
    inbuf = 0;
    return ret;
}
