#include "bsp_epit.h"

#define EPIT_IRQ_ID    EPIT1_IRQn
#define EXAMPLE_EPIT   EPIT1
/* Get source clock for EPIT driver (EPIT prescaler = 0) */
#define EXAMPLE_EPIT_CLK_FREQ (CLOCK_GetFreq(kCLOCK_IpgClk) / \
                              (CLOCK_GetDiv(kCLOCK_PerclkDiv) + 1U))

volatile bool epitIsrFlag = false;

void EXAMPLE_EPIT_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    EPIT_ClearStatusFlags(EXAMPLE_EPIT, kEPIT_OutputCompareFlag);
    epitIsrFlag = true;
}

void bsp_epit_init(void)
{
/* Structure of initialize EPIT */
    epit_config_t epitConfig;
	/* Tricky here: As IRQ handler in example doesn't use parameters, just ignore them */
    SystemInstallIrqHandler(EPIT1_IRQn, (system_irq_handler_t)(uint32_t)EXAMPLE_EPIT_IRQHandler, NULL);

    EPIT_GetDefaultConfig(&epitConfig);

    /* Init EPIT module */
    EPIT_Init(EXAMPLE_EPIT, &epitConfig);

    /* Set timer period */
    EPIT_SetTimerPeriod(EXAMPLE_EPIT, USEC_TO_COUNT(2000000U, EXAMPLE_EPIT_CLK_FREQ) - 1);
    EPIT_SetOutputCompareValue(EXAMPLE_EPIT, 0);

    /* Enable output compare interrupts */
    EPIT_EnableInterrupts(EXAMPLE_EPIT, kEPIT_OutputCompareInterruptEnable);

    /* Enable at the Interrupt */
    EnableIRQ(EPIT_IRQ_ID);

    /* Start Timer */
    EPIT_StartTimer(EXAMPLE_EPIT);
}