#ifndef __IDEA_DISPLAY_H
#define __IDEA_DISPLAY_H

#include <stdint.h>
#include "simple_uart.h"
#include "idea_board_config_pins.h"

#define SPI_FLASH_PAGE_NUM_0824                 0
#define SPI_FLASH_PAGE_NUM_BATTERY_BLUETH       1
#define SPI_FLASH_PAGE_STEP_DISTANCE_CALORIES   2
#define SPI_FLASH_PAGE_HATERATE                 3
//#define SPI_FLASH_PAGE_HATERATE                 4
#define SPI_FLASH_PAGE_SLEEP                    5
#define SPI_FLASH_PAGE_SEARCH                   6


void Display_show_time_mode(void);
void Display_show_step_mode(void);
void Display_show_calories_mode(void);
void Display_show_distance_mode(void);
void Display_show_charging_mode(uint8_t val);
void Display_show_low_power_mode(void);
void refresh_time(void);

#if USE_HEART_RATE
void Display_show_hr_mode(void);
void refresh_heart_rate(void);
#endif


#endif      // #ifndef __IDEA_DISPLAY_H