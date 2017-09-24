#ifndef _IDEA_SPI_FLASH_H
#define _IDEA_SPI_FLASH_H
#include <stdint.h>

void spi_flash_init(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_Erase_Sector(uint32_t nDest);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite,uint8_t offset);
void SPI_FLASH_PageRead(uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t offset);

#endif
