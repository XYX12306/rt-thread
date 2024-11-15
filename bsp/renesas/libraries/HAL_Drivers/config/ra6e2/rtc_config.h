/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-15     Mr.XYX     first version
 */

#ifndef __RTC_CONFIG_H__
#define __RTC_CONFIG_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "hal_data.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(BSP_USING_ONCHIP_RTC)

typedef struct rt_rtc_periodic *rt_rtc_periodic_t;
typedef void (*rt_rtc_periodic_callback_t)(rt_rtc_periodic_t rtc_periodic, time_t timestamp);

struct rt_rtc_periodic_container
{
    rt_list_t head;
    struct rt_mutex mutex;
    struct rt_event event;
};

struct rt_rtc_periodic
{
    rt_list_t list;
    rt_uint32_t flag;
    rt_rtc_periodic_callback_t callback;
};

struct rt_rtc_periodic_setup
{
    rt_uint32_t flag;                /* rtc_periodic flag */
};

rt_rtc_periodic_t rt_rtc_periodic_create(rt_rtc_periodic_callback_t callback, struct rt_rtc_periodic_setup *setup);
rt_err_t rt_rtc_periodic_delete(rt_rtc_periodic_t rtc_periodic);

#endif

#ifdef __cplusplus
}
#endif

#endif
