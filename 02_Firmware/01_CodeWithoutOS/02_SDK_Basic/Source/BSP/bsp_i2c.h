#ifndef _BSP_I2C_H
#define _BSP_I2C_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_i2c.h" 

void bsp_i2c_init(void);

void i2c_write_bytes(void);

void i2c_read_bytes(void);

#endif
