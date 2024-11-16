/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-10     Sherman       first version
 */

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>
#include <drv_config.h>
#include "ssd1306.h"
#include "sensor_dallas_dht11.h"

#define LED1_PIN    BSP_IO_PORT_02_PIN_07 /* Onboard LED1 pins */
#define LED2_PIN    BSP_IO_PORT_02_PIN_06 /* Onboard LED2 pins */

rt_device_t dev_dht11 = RT_NULL;

void user_rtc_periodic_callback(rt_rtc_periodic_t rtc_periodic, time_t timestamp)
{
    time_t now;
    char buffer[21];
    now = time(RT_NULL);
    const char *time_str = ctime(&now);
    strncpy(buffer, time_str, 19);
    buffer[19] = '\0';
    buffer[20] = '\0';
    rt_kprintf("user rtc_periodic callback function.\n");
    rt_kprintf("%s\n", ctime(&now));
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString(buffer, Font_6x8, White);

    struct rt_sensor_data sensor_data;
    rt_size_t res;

    res = rt_device_read(dev_dht11, 0, &sensor_data, 1);
    if (res != 1)
    {
        rt_kprintf("read data failed! result is %d\n", res);
        rt_device_close(dev_dht11);
    }
    else
    {
        if (sensor_data.data.temp >= 0)
        {
            uint8_t temp = (sensor_data.data.temp & 0xffff) >> 0;      // get temp
            uint8_t humi = (sensor_data.data.temp & 0xffff0000) >> 16; // get humi
//            rt_kprintf("temp:%d, humi:%d\n" ,temp, humi);
            rt_memset(buffer, RT_NULL, sizeof(buffer) / sizeof(char));
            rt_sprintf(buffer, "Temp: %.2d Humi: %.2d%%", temp, humi);
            ssd1306_SetCursor(2, 16);
            ssd1306_WriteString(buffer, Font_6x8, White);
        }
    }
    ssd1306_UpdateScreen();
}

void hal_entry(void)
{
    struct rt_rtc_periodic_setup setup;
    struct rt_rtc_periodic *rtc_periodic = RT_NULL;

    rt_kprintf("\nHello RT-Thread!\n");

    ssd1306_Init();
    setup.flag = RTC_PERIODIC_IRQ_SELECT_1_SECOND;
    rtc_periodic = rt_rtc_periodic_create(user_rtc_periodic_callback, &setup);
    if(RT_NULL != rtc_periodic)
    {
		;
    }
//    rt_thread_mdelay(10000);
//    rt_rtc_periodic_delete(rtc_periodic);

    rt_uint8_t get_data_freq = 1; /* 1Hz */

    dev_dht11 = rt_device_find("temp_dht11");
    if (dev_dht11 == RT_NULL)
    {
        return;
    }
    if (rt_device_open(dev_dht11, RT_DEVICE_FLAG_RDWR) != RT_EOK)
    {
        rt_kprintf("open device failed!\n");
        return;
    }
    rt_device_control(dev_dht11, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));

    while (1)
    {
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_pin_write(LED2_PIN, PIN_LOW);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_pin_write(LED2_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
    }
}
