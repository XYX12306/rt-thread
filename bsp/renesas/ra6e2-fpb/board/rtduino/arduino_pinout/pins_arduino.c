/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2023-10-28     Wangyuqiang    first version
 */

#include <Arduino.h>
#include "pins_arduino.h"

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>

/*
 * {Arduino Pin, RT-Thread Pin [, Device Name, Channel]}
 * [] means optional
 * Digital pins must NOT give the device name and channel.
 * Analog pins MUST give the device name and channel(ADC, PWM or DAC).
 * Arduino Pin must keep in sequence.
 */
const pin_map_t pin_map_table[]=
{
    {D0, BSP_IO_PORT_04_PIN_10, "uart0"},                        /* Serial0-RX */
    {D1, BSP_IO_PORT_04_PIN_11, "uart0"},                        /* Serial0-TX */
    {D2, BSP_IO_PORT_01_PIN_05},
    {D3, BSP_IO_PORT_04_PIN_08},
    {D4, BSP_IO_PORT_05_PIN_00},
    {D5, BSP_IO_PORT_04_PIN_09},
    {D6, BSP_IO_PORT_01_PIN_13},
    {D7, BSP_IO_PORT_00_PIN_08},
    {D8, BSP_IO_PORT_00_PIN_06},
    {D9, BSP_IO_PORT_04_PIN_03},
    {D10, BSP_IO_PORT_03_PIN_01},
    {D11, BSP_IO_PORT_01_PIN_09},
    {D12, BSP_IO_PORT_01_PIN_10},
    {D13, BSP_IO_PORT_01_PIN_11},
    {D14, BSP_IO_PORT_01_PIN_01},
    {D15, BSP_IO_PORT_01_PIN_00},
    {A0, BSP_IO_PORT_00_PIN_00},
    {A1, BSP_IO_PORT_00_PIN_01},
    {A2, BSP_IO_PORT_00_PIN_02},
    {A3, BSP_IO_PORT_00_PIN_04},
    {A4, BSP_IO_PORT_00_PIN_03},
    {A5, BSP_IO_PORT_00_PIN_13}
};
