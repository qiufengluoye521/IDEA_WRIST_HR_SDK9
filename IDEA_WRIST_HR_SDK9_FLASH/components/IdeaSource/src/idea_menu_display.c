#include "idea_menu_display.h"
#include "idea_oled.h"
//#include "idea_spi_flash.h"
#include "idea_sw_flash.h"

extern uint8_t spim_dma_rxbuf[260];

void menu_display(uint8_t display_mode)
{
    switch(display_mode)
    {
        case UI_MENU_DISPLAY_BONDING:
            OLED_Clear();
            ////SPI_FLASH_PageRead(SPI_FLASH_PAGE_BONDING,128,0);
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_BONDING,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_BONDED:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_BONDED,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_CALLING:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_CALLING,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_ALARM:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_ALARM,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_LOSE:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_LOSE,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_SWITCH:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_SWITCH,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_TARGET:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_TARGET,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_SLEEP:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_SLEEP,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        case UI_MENU_DISPLAY_STEP:
            OLED_Clear();
            SpiFlash_Read_Data(0,SPI_FLASH_PAGE_STEP,0,256);
            OLED_DrawBMP(0,0,64,4,spim_dma_rxbuf);
            break;
        default :
            break;
    }
    
}



