#ifndef _BSP_EMMC_H
#define _BSP_EMMC_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_usdhc.h" 

status_t eMMC_Init(void);

status_t eMMC_ReadBlocks(USDHC_Type *base,
                        uint32_t startBlock,
                        uint8_t *buffer,
                        uint32_t blockCount,
                        uint32_t blockSize);
status_t eMMC_WriteBlocks(USDHC_Type *base,
                         uint32_t startBlock,
                         const uint8_t *buffer,
                         uint32_t blockCount,
                         uint32_t blockSize);
status_t eMMC_Blocking_Test(void);

#endif
