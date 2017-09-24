#ifndef _IDEA_SPI_FLASH_H
#define _IDEA_SPI_FLASH_H
#include <stdint.h>


extern const uint8_t Num_0824[256];
extern const uint8_t Num0805_battery_blutooth[256];
extern const uint8_t step_distence_calorie[256];
extern const uint8_t heart_rate[256];
extern const uint8_t Haterate[256];
extern const uint8_t lowpower[256];
extern const uint8_t charging[256];
extern const uint8_t chargfull[256];


void spi_flash_init(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_Erase_Sector(uint32_t nDest);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite,uint8_t offset);
void SPI_FLASH_PageRead(uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t offset);

#endif
