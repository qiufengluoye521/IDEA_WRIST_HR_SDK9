#include "idea_spi_flash.h"
#include "idea_spi_master.h"
#include "nrf_gpio.h"
#include "spi_master_config.h"
#include "nrf_assert.h"
#include "string.h"
#include "simple_uart.h"


#define SPI_FLASH_CS_LOW()              nrf_gpio_pin_clear(SPI_PSELSS1)
#define SPI_FLASH_CS_HIGH()             nrf_gpio_pin_set(SPI_PSELSS1)

#define SPI_FLASH_PageSize              4096
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable                0x06 
#define W25X_WriteDisable               0x04 
#define W25X_ReadStatusReg              0x05 
#define W25X_WriteStatusReg             0x01 
#define W25X_ReadData                   0x03 
#define W25X_FastReadData               0x0B 
#define W25X_FastReadDual               0x3B 
#define W25X_PageProgram                0x02 
#define W25X_BlockErase                 0xD8 
#define W25X_SectorErase                0x20 
#define W25X_ChipErase                  0xC7 
#define W25X_PowerDown                  0xB9 
#define W25X_ReleasePowerDown           0xAB 
#define W25X_DeviceID                   0xAB 
#define W25X_ManufactDeviceID           0x90 
#define W25X_JedecDeviceID              0x9F 

uint8_t spim_dma_txbuf[8]           = {0};
uint8_t spim_dma_rxbuf[260]         = {0};



void spi_flash_init(void)
{
    uint32_t *spi_base_address = spi_master_init(SPI1,SPI_MODE0,false);
    if (spi_base_address == 0) {
        return;
    };
    spi_master_enable(SPI1);
    //spi_master_disable(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
    spi_master_enable(SPI1);
    uint32_t err;
    SPI_FLASH_CS_HIGH();
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "Write Enable" instruction */
    //SPI_FLASH_SendByte(W25X_WriteEnable);
    spim_dma_txbuf[0] = W25X_WriteEnable;
    ASSERT(spi_master_tx_rx((uint32_t *)NRF_SPI1,1,spim_dma_txbuf,spim_dma_rxbuf));
    spi_master_disable(SPI1);
}

void SPI_FLASH_Erase_Sector(uint32_t nDest)
{
    spi_master_enable(SPI1);
    nDest *= SPI_FLASH_PageSize;

    spim_dma_txbuf[0] = W25X_SectorErase;
    spim_dma_txbuf[1] = (uint8_t)((nDest & 0xFFFFFF) >> 16);
    spim_dma_txbuf[2] = (uint8_t)((nDest & 0xFFFF) >> 8);
    spim_dma_txbuf[3] = (uint8_t)nDest & 0xFF;
    SPI_FLASH_CS_HIGH();
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    ASSERT(spi_master_tx_rx((uint32_t *)NRF_SPI1,4,spim_dma_txbuf,spim_dma_rxbuf));
    spi_master_disable(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite,uint8_t offset)
{
    spi_master_enable(SPI1);
    uint32_t err;
    uint32_t add;
    add = WriteAddr * SPI_FLASH_PerWritePageSize + offset;
     /* Chip Select high */
    SPI_FLASH_CS_HIGH();
//    spim_dma_buffer[0] = W25X_WriteEnable;                  // Enable the write access to the FLASH 
    spim_dma_txbuf[0] = W25X_PageProgram;                  // Page Program Command
    spim_dma_txbuf[1] = (add & 0xFF0000) >> 16;      // Add bit23~bit16
    spim_dma_txbuf[2] = (add & 0x00FF00) >> 8;       // Add bit15~bit8
    spim_dma_txbuf[3] = (add & 0x0000FF) >> 0;       // Add bit7~bit0
    
    for(uint16_t i=0;i<NumByteToWrite;i++)
    {
        spim_dma_txbuf[4+i] = pBuffer[i];
    }
    
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    ASSERT(spi_master_tx_rx((uint32_t *)NRF_SPI1,NumByteToWrite + 4,spim_dma_txbuf,spim_dma_rxbuf));
    spi_master_disable(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageRead
* Description    : Read a page to the FLASH with a single Read
*                  cycle(Page Read sequence). The number of byte can't exceed
*                  a FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    Read from the FLASH.
*                  - ReadAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to read from the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageRead(uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t offset)
{
    spi_master_enable(SPI1);
    uint32_t err;
    uint32_t add;
    add = ReadAddr * SPI_FLASH_PerWritePageSize + offset;
     /* Chip Select high */
    SPI_FLASH_CS_HIGH();
    spim_dma_txbuf[0] = W25X_ReadData;               // Read Data Bytes Command
    spim_dma_txbuf[1] = (add & 0xFF0000) >> 16;      // Add bit23~bit16
    spim_dma_txbuf[2] = (add & 0x00FF00) >> 8;       // Add bit15~bit8
    spim_dma_txbuf[3] = (add & 0x0000FF) >> 0;       // Add bit7~bit0
    
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    ASSERT(spi_master_tx_rx((uint32_t *)NRF_SPI1,NumByteToRead + 4,spim_dma_txbuf,spim_dma_rxbuf));
    
    spi_master_disable(SPI1);
}

