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
#include "sensor_inven_mpu6xxx.h"

#define LED1_PIN    BSP_IO_PORT_02_PIN_07 /* Onboard LED1 pins */
#define LED2_PIN    BSP_IO_PORT_02_PIN_06 /* Onboard LED2 pins */
#define MPU6XXX_DEVICE_NAME  "i2c1"

rt_device_t dev_dht11 = RT_NULL;
struct mpu6xxx_device *dev_mpu6050 = RT_NULL;
rt_mutex_t oled_mutex;

void user_rtc_periodic_callback(rt_rtc_periodic_t rtc_periodic, time_t timestamp)
{
    time_t now;
    char buffer[21];
    now = time(RT_NULL);
    const char *time_str = ctime(&now);

    rt_memset(buffer, RT_NULL, sizeof(buffer) / sizeof(char));
    strncpy(buffer, time_str, 19);
//    rt_kprintf("user rtc_periodic callback function.\n");
//    rt_kprintf("%s\n", ctime(&now));
//    ssd1306_Fill(Black);
    ssd1306_Fill_Page(Black, 0);
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
            ssd1306_Fill_Page(Black, 1);
            ssd1306_Fill_Page(Black, 2);
            ssd1306_SetCursor(2, 16);
            ssd1306_WriteString(buffer, Font_6x8, White);
        }
    }
    rt_mutex_take(oled_mutex, RT_WAITING_FOREVER);
//    ssd1306_UpdateScreen();
//    ssd1306_UpdateScreenPage(0);
//    ssd1306_UpdateScreenPage(1);
//    ssd1306_UpdateScreenPage(2);
    ssd1306_UpdateScreenPageFromTo(0, 2);
    rt_mutex_release(oled_mutex);
}

static void mpu6050_show_thread_entry(void *param)
{
    struct mpu6xxx_3axes accel, gyro;
    char buffer[50];

    while (1)
    {
        rt_mutex_take(oled_mutex, RT_WAITING_FOREVER);
        mpu6xxx_get_accel(dev_mpu6050, &accel);
        mpu6xxx_get_gyro(dev_mpu6050, &gyro);
        rt_mutex_release(oled_mutex);

//        rt_kprintf("accel.x = %3d, accel.y = %3d, accel.z = %3d ", accel.x, accel.y, accel.z);
//        rt_kprintf("gyro.x = %3d gyro.y = %3d, gyro.z = %3d\n", gyro.x, gyro.y, gyro.z);
        rt_memset(buffer, RT_NULL, sizeof(buffer) / sizeof(char));
        rt_sprintf(buffer, "     x    y     z");
        ssd1306_Fill_Page(Black, 3);
        ssd1306_Fill_Page(Black, 4);
        ssd1306_SetCursor(2, 32);
        ssd1306_WriteString(buffer, Font_6x8, White);
        rt_memset(buffer, RT_NULL, sizeof(buffer) / sizeof(char));
        rt_sprintf(buffer, "A: %+3d  %+3d  %+3d", accel.x, accel.y, accel.z);
        ssd1306_Fill_Page(Black, 5);
        ssd1306_SetCursor(2, 40);
        ssd1306_WriteString(buffer, Font_6x8, White);
        rt_memset(buffer, RT_NULL, sizeof(buffer) / sizeof(char));
        rt_sprintf(buffer, "G: %+3d  %+3d  %+3d", gyro.x, gyro.y, gyro.z);
        ssd1306_Fill_Page(Black, 6);
        ssd1306_SetCursor(2, 48);
        ssd1306_WriteString(buffer, Font_6x8, White);
        rt_mutex_take(oled_mutex, RT_WAITING_FOREVER);
//        ssd1306_UpdateScreen();
//        ssd1306_UpdateScreenPage(3);
//        ssd1306_UpdateScreenPage(4);
//        ssd1306_UpdateScreenPage(5);
//        ssd1306_UpdateScreenPage(6);
        ssd1306_UpdateScreenPageFromTo(3, 6);
        rt_mutex_release(oled_mutex);
        rt_thread_mdelay(200);
    }
}

void hal_entry(void)
{
    struct rt_rtc_periodic_setup setup;
    struct rt_rtc_periodic *rtc_periodic = RT_NULL;

    rt_kprintf("\nHello RT-Thread!\n");

    ssd1306_Init();
    oled_mutex = rt_mutex_create("oled_mutex", RT_IPC_FLAG_PRIO);
    if (oled_mutex == RT_NULL)
    {
        rt_kprintf("Failed to create mutex\n");
    }
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
        ;
    }
    if (rt_device_open(dev_dht11, RT_DEVICE_FLAG_RDWR) != RT_EOK)
    {
        rt_kprintf("open device failed!\n");
    }
    rt_device_control(dev_dht11, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));

    dev_mpu6050 = mpu6xxx_init(MPU6XXX_DEVICE_NAME, RT_NULL);
    if (dev_mpu6050 == RT_NULL)
    {
        rt_kprintf("mpu6xxx init failed\n");
    }
    rt_kprintf("mpu6xxx init succeed\n");

    rt_thread_t tid;

    tid = rt_thread_create("mpu6050_show",
                           mpu6050_show_thread_entry, RT_NULL,
                           1024,
                           9,
                           5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

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
