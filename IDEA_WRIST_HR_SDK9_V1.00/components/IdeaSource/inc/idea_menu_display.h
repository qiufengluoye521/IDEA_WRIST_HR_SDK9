#ifndef __IDEA_MENU_DISPLAY_H
#define __IDEA_MENU_DISPLAY_H

#include <stdint.h>

#define UI_MENU_DISPLAY_BONDING         1
#define UI_MENU_DISPLAY_BONDED          2
#define UI_MENU_DISPLAY_CALLING         3
#define UI_MENU_DISPLAY_ALARM           4
#define UI_MENU_DISPLAY_LOSE            5
#define UI_MENU_DISPLAY_SWITCH          6
#define UI_MENU_DISPLAY_TARGET          7
#define UI_MENU_DISPLAY_SLEEP           8
#define UI_MENU_DISPLAY_STEP            9


#define SPI_FLASH_PAGE_BONDING          0
#define SPI_FLASH_PAGE_BONDED           1
#define SPI_FLASH_PAGE_CALLING          2
#define SPI_FLASH_PAGE_ALARM            3
#define SPI_FLASH_PAGE_LOSE             4
#define SPI_FLASH_PAGE_SWITCH           5
#define SPI_FLASH_PAGE_TARGET           6
#define SPI_FLASH_PAGE_SLEEP            7
#define SPI_FLASH_PAGE_STEP             8

void menu_display(uint8_t display_mode);

#endif          //__IDEA_MENU_DISPLAY_H