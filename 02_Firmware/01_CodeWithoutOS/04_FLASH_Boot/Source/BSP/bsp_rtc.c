#include "bsp_rtc.h"
#include "fsl_debug_console.h"
#include "bsp_uart.h"

snvs_hp_rtc_datetime_t rtcDate;
snvs_hp_rtc_config_t snvsRtcConfig;

void bsp_rtc_init(void)
{
	/* Init SNVS */
    /*
     * snvsConfig->rtccalenable = false;
     * snvsConfig->rtccalvalue = 0U;
     * snvsConfig->srtccalenable = false;
     * snvsConfig->srtccalvalue = 0U;
     * snvsConfig->PIFreq = 0U;
     */
    SNVS_HP_RTC_GetDefaultConfig(&snvsRtcConfig);
    SNVS_HP_RTC_Init(SNVS, &snvsRtcConfig);

    /* Set a start date time and start RT */
    rtcDate.year = 2014U;
    rtcDate.month = 12U;
    rtcDate.day = 25U;
    rtcDate.hour = 19U;
    rtcDate.minute = 0;
    rtcDate.second = 0;

    /* Set RTC time to default time and date and start the RTC */
    SNVS_HP_RTC_SetDatetime(SNVS, &rtcDate);
    SNVS_HP_RTC_StartTimer(SNVS);

}

void get_rtc_time(void)
{
	/* Get date time */
	SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);

	/* print default time */
	uart_printf("Current datetime: %d-%d-%d %d:%d:%d\r\n", rtcDate.year, rtcDate.month, rtcDate.day,
		   rtcDate.hour, rtcDate.minute, rtcDate.second);

}