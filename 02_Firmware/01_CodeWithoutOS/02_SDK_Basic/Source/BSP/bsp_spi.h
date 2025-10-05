#ifndef _BSP_SPI_H
#define _BSP_SPI_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_ecspi.h" 
#include "fsl_gpio.h" 

void    bsp_spi_init(void);
uint8_t spi_read_write_byte(uint8_t txByte);
void    spi_cycle_exp(void);

#endif

