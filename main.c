//******************************************************************************
// ECE353 HW2
// Name: Arnav Mehta
//******************************************************************************
#include "msp.h"
#include "serial_debug.h"
#include "lcd.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// !! Once you have added the required files for HW2, un-comment the lines below.

//Fill in your name
#define RED_5   0x1F
#define GREEN_6 0x0F
#define BLUE_5  0x0F
#define BLACK 0x0000

#define FG_COLOR (RED_5 << 11)
#define BG_COLOR BLACK

#define X_COORD 63
#define Y_COOR  63
#define X_LEN   20
#define Y_LEN   20

char STUDENT_NAME[] = "Arnav Mehta";
char PROMPT[] = "-> : ";
char OK[] = "OK : ";
char ERR[] = "ERR: ";
char SETX[] = "set_x";
char SETY[] = "set_y";
char LINEX[] = "line_x";
char LINEY[] = "line_y";
char LINECLR[] = "line_clr";
char PIXEL[] = "pixel";
int x = 0;
int y = 0;
uint32_t color_line = 0;
uint32_t color_pixel = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    serial_debug_init();
    Crystalfontz128x128_Init();

    char* input;
    char* function;
    char* value;

    __enable_irq();

    while(1)
    {
        printf(PROMPT);
        while(!ALERT_STRING);

        //printf(Rx_String);
        printf("\r\n");

        input = strdup(Rx_String);
        function = strtok(Rx_String, " \n");

        int j;
        for(j=0; j<strlen(function); j++) {
            if(function[j]=='_') {
                continue;
            }
            function[j] = tolower(function[j]);
        }
        value = strtok(NULL, " \n");

        int valid = 0;

        if(strcmp(SETX, function)==0 && strcmp(value,"\0")!=0) {
            int val = atoi(value);
            if(val > -1 && val < 132) {
                x = val;
                valid = 1;
            }
        } else if(strcmp(SETY, function)==0 && strcmp(value,"\0")!=0) {
            int val = atoi(value);
            if(val > -1 && val < 132) {
                y = val;
                valid = 1;
            }
        } else if(strcmp(LINEX, function)==0 && strcmp(value,"\0")!=0) {
            int val = atoi(value);
            if(val > -1 && val < 132) {
                valid = 1;
                lcd_draw_line_horizontal(x, y, val, color_line);
            }
        } else if(strcmp(LINEY, function)==0 && strcmp(value,"\0")!=0) {
            int val = atoi(value);
            if(val > -1 && val < 132) {
                valid = 1;
                lcd_draw_line_vertical(x, y, val, color_line);
            }
        } else if(strcmp(LINECLR, function)==0) {
            uint32_t color_new = (uint32_t)strtol(value, NULL, 16);
            if (color_new <= 0xFFFF && color_new >= 0) {
                color_line = color_new;
                valid = 1;
            }
        } else if(strcmp(PIXEL, function)==0) {
            uint32_t color_new = (uint32_t)strtol(value, NULL, 16);
            if (color_new <= 0xFFFF && color_new >= 0) {
                color_pixel = color_new;
                valid = 1;
                lcd_draw_pixel(x, y, color_pixel);
            }
        }

        if(valid==0) {
            printf(ERR);
            printf(input);
            printf("\r\n");
        } else {
            printf(OK);
            printf(input);
            printf("\r\n");
        }
        ALERT_STRING = 0;
        Rx_Char_Count = 0;
        memset(Rx_String,0,80);
    }
}
