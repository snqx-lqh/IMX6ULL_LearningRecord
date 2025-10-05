#ifndef _BSP_I2C_H
#define _BSP_I2C_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_i2c.h" 

void bsp_i2c1_master_init(void);

void i2c_master_write_bytes(I2C_Type *I2C,uint8_t addr,uint8_t reg,
                                        uint8_t *data,uint32_t len);

void i2c_master_read_bytes(I2C_Type *I2C,uint8_t addr,uint8_t reg,
                                        uint8_t *data,uint32_t len);

#endif
