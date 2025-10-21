#ifndef _BSP_EMMC_H
#define _BSP_EMMC_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_usdhc.h" 

void bsp_emmc_init(void);
void emmc_write_data(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);
void emmc_read_data(uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);

#endif
