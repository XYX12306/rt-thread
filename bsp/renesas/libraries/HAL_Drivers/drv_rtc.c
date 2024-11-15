/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-14     Mr.Tiger     first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include <sys/time.h>
#include "hal_data.h"
#include <drv_config.h>

#ifdef BSP_USING_ONCHIP_RTC

#define DBG_TAG              "drv.rtc"
#ifdef DRV_DEBUG
    #define DBG_LVL               DBG_LOG
#else
    #define DBG_LVL               DBG_INFO
#endif /* DRV_DEBUG */
#include <rtdbg.h>

static rt_err_t ra_rtc_init(void)
{
    rt_err_t result = RT_EOK;

    if (R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg) != RT_EOK)
    {
        LOG_E("rtc init failed.");
        result = -RT_ERROR;
    }

    return result;
}

static time_t get_rtc_timestamp(void)
{
    struct tm tm_new = {0};
    rtc_time_t g_current_time = {0};

    R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_current_time);

    tm_new.tm_year  = g_current_time.tm_year;
    tm_new.tm_mon   = g_current_time.tm_mon;
    tm_new.tm_mday  = g_current_time.tm_mday;

    tm_new.tm_hour  = g_current_time.tm_hour;
    tm_new.tm_min   = g_current_time.tm_min;
    tm_new.tm_sec   = g_current_time.tm_sec;

    tm_new.tm_wday  = g_current_time.tm_wday;
    tm_new.tm_yday  = g_current_time.tm_yday;
    tm_new.tm_isdst = g_current_time.tm_isdst;

    return timegm(&tm_new);
}

static rt_err_t ra_get_secs(time_t *sec)
{
    *(rt_uint32_t *)sec = get_rtc_timestamp();
    LOG_D("RTC: get rtc_time %x\n", *(rt_uint32_t *)sec);

    return RT_EOK;
}

static rt_err_t set_rtc_time_stamp(time_t time_stamp)
{
    struct tm now;
    rtc_time_t g_current_time = {0};
    gmtime_r(&time_stamp, &now);
    if (now.tm_year < 100)
    {
        return -RT_ERROR;
    }

    g_current_time.tm_sec    = now.tm_sec ;
    g_current_time.tm_min    = now.tm_min ;
    g_current_time.tm_hour   = now.tm_hour;
    g_current_time.tm_mday   = now.tm_mday;
    g_current_time.tm_mon    = now.tm_mon;
    g_current_time.tm_year   = now.tm_year;
    g_current_time.tm_wday   = now.tm_wday;
    g_current_time.tm_yday   = now.tm_yday;

    if (R_RTC_CalendarTimeSet(&g_rtc_ctrl, &g_current_time) != FSP_SUCCESS)
    {
        LOG_E("set rtc time failed.");
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t ra_set_secs(time_t *sec)
{

    rt_err_t result = RT_EOK;

    if (set_rtc_time_stamp(*(rt_uint32_t *)sec))
    {
        result = -RT_ERROR;
    }
    LOG_D("RTC: set rtc_time %x\n", *(rt_uint32_t *)sec);

    return result;
}

#ifdef RT_USING_ALARM
static rt_err_t ra_get_alarm(struct rt_rtc_wkalarm *alarm)
{
    rt_err_t result = RT_EOK;
    struct rt_rtc_wkalarm *wkalarm = alarm;
    rtc_alarm_time_t alarm_time_get =
    {
        .sec_match        =  RT_FALSE,
        .min_match        =  RT_FALSE,
        .hour_match       =  RT_FALSE,
        .mday_match       =  RT_FALSE,
        .mon_match        =  RT_FALSE,
        .year_match       =  RT_FALSE,
        .dayofweek_match  =  RT_FALSE,
    };

    if (RT_EOK == R_RTC_CalendarAlarmGet(&g_rtc_ctrl, &alarm_time_get))
    {
        wkalarm->tm_hour = alarm_time_get.time.tm_hour;
        wkalarm->tm_min  = alarm_time_get.time.tm_min;
        wkalarm->tm_sec  = alarm_time_get.time.tm_sec;
    }
    else
    {
        LOG_E("Calendar alarm Get failed.");
    }

    return result;
}

static rt_err_t ra_set_alarm(struct rt_rtc_wkalarm *alarm)
{
    rt_err_t result = RT_EOK;
    struct rt_rtc_wkalarm *wkalarm = alarm;
    rtc_alarm_time_t alarm_time_set =
    {
        .sec_match        =  RT_TRUE,
        .min_match        =  RT_TRUE,
        .hour_match       =  RT_TRUE,
        .mday_match       =  RT_FALSE,
        .mon_match        =  RT_FALSE,
        .year_match       =  RT_FALSE,
        .dayofweek_match  =  RT_FALSE,
    };

    alarm_time_set.time.tm_hour = wkalarm->tm_hour;
    alarm_time_set.time.tm_min  = wkalarm->tm_min;
    alarm_time_set.time.tm_sec  = wkalarm->tm_sec;
    if (1 == wkalarm->enable)
    {
        if (RT_EOK != R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set))
        {
            LOG_E("Calendar alarm Set failed.");
            result = -RT_ERROR;
        }
    }
    else
    {
        alarm_time_set.sec_match        =  RT_FALSE;
        alarm_time_set.min_match        =  RT_FALSE;
        alarm_time_set.hour_match       =  RT_FALSE;
        if (RT_EOK != R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set))
        {
            LOG_E("Calendar alarm Stop failed.");
            result = -RT_ERROR;
        }
    }
    return result;
}
#endif /* RT_USING_ALARM */

rt_err_t ra_get_periodic(uint32_t *rate)
{
    rt_err_t result = RT_EOK;

    if (rate == NULL)
    {
        result = -RT_ERROR;
        return result;
    }

    *rate = R_RTC->RCR1_b.PES;

    return result;
}

rt_err_t ra_set_periodic(rtc_periodic_irq_select_t *rate)
{
    rt_err_t result = RT_EOK;

    if (rate == NULL)
    {
        result = -RT_ERROR;
        return result;
    }

    if (R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, *rate) != RT_EOK)
    {
        LOG_E("set rtc periodic failed.");
        result = -RT_ERROR;
    }

    return result;
}

#define EVENT_FLAG_RTC_PERIODIC (1 << 3)
static struct rt_rtc_periodic_container _container;

static void rtc_periodic_update(rt_uint32_t event)
{
    struct rt_rtc_periodic *rtc_periodic;
    time_t timestamp = (time_t)0;
    struct tm now;
    rt_list_t *next;

    rt_mutex_take(&_container.mutex, RT_WAITING_FOREVER);
    if (!rt_list_isempty(&_container.head))
    {
        /* get time of now */
        get_timestamp(&timestamp);
        gmtime_r(&timestamp, &now);

        for (next = _container.head.next; next != &_container.head; next = next->next)
        {
            rtc_periodic = rt_list_entry(next, struct rt_rtc_periodic, list);
            /* check the overtime alarm */
            timestamp = (time_t)0;
            get_timestamp(&timestamp);
            rtc_periodic->callback(rtc_periodic, timestamp);
        }
    }
    rt_mutex_release(&_container.mutex);
}

/** \brief send a rtc periodic event
 *
 * \param dev pointer to RTC device(currently unused,you can ignore it)
 * \param event RTC periodic event
 * \return none
 */
static void rt_rtc_periodic_update(rt_device_t dev, rt_uint32_t event)
{
    rt_event_send(&_container.event, event);
}

/** \brief rtc periodic service thread entry
 *
 */
static void rt_rtc_periodic_thread_init(void *param)
{
    rt_uint32_t recv;

    while (1)
    {
        if (rt_event_recv(&_container.event, EVENT_FLAG_RTC_PERIODIC,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_FOREVER, &recv) == RT_EOK)
        {
            rtc_periodic_update(recv);
        }
    }
}

int rt_rtc_periodic_system_init(void)
{
    rt_thread_t tid;

    rt_list_init(&_container.head);
    rt_event_init(&_container.event, "rtc_periodic", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&_container.mutex, "rtc_periodic", RT_IPC_FLAG_PRIO);

    tid = rt_thread_create("rtc_periodic",
                           rt_rtc_periodic_thread_init, RT_NULL,
                           2048,
                           2,
                           5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

INIT_PREV_EXPORT(rt_rtc_periodic_system_init);

/** \brief create a rtc periodic
 *
 * \param flag Periodic Interrupt select e.g:
 * \param setup pointer to setup infomation
 */
rt_rtc_periodic_t rt_rtc_periodic_create(rt_rtc_periodic_callback_t callback, struct rt_rtc_periodic_setup *setup)
{
    struct rt_rtc_periodic *rtc_periodic;

    if (setup == RT_NULL)
        return (RT_NULL);

    rtc_periodic = rt_malloc(sizeof(struct rt_alarm));
    if (rtc_periodic == RT_NULL)
        return (RT_NULL);

    rt_list_init(&rtc_periodic->list);

    rtc_periodic->flag = setup->flag;
    rtc_periodic->callback = callback;
		ra_set_periodic(&(rtc_periodic->flag));
    rt_mutex_take(&_container.mutex, RT_WAITING_FOREVER);
    rt_list_insert_after(&_container.head, &rtc_periodic->list);
    rt_mutex_release(&_container.mutex);

    return (rtc_periodic);
}

/** \brief delete a rtc periodic
 *
 * \param rtc periodic pointer to rtc periodic
 * \return RT_EOK
 */
rt_err_t rt_rtc_periodic_delete(rt_rtc_periodic_t rtc_periodic)
{
    rt_err_t ret = RT_EOK;

    if (rtc_periodic == RT_NULL)
        return -RT_ERROR;
    rt_mutex_take(&_container.mutex, RT_WAITING_FOREVER);
    /* stop the alarm */
    R_RTC->RCR1_b.PIE = 0;
    rt_list_remove(&rtc_periodic->list);
    rt_free(rtc_periodic);

    rt_mutex_release(&_container.mutex);

    return (ret);
}

void rtc_callback(rtc_callback_args_t *p_args)
{
#ifdef RT_USING_ALARM
    static rt_device_t ra_device;
    if (RTC_EVENT_ALARM_IRQ == p_args->event)
    {
        rt_alarm_update(ra_device, 1);
    }
#endif
    static rt_device_t ra_device_rtc_periodic;
    if (RTC_EVENT_PERIODIC_IRQ == p_args->event)
    {
        rt_rtc_periodic_update(ra_device_rtc_periodic, EVENT_FLAG_RTC_PERIODIC);
    }
}

static const struct rt_rtc_ops ra_rtc_ops =
{
    .init      = ra_rtc_init,
    .get_secs  = ra_get_secs,
    .set_secs  = ra_set_secs,
#ifdef RT_USING_ALARM
    .set_alarm = ra_set_alarm,
    .get_alarm = ra_get_alarm,
#endif
    .get_periodic = ra_get_periodic,
    .set_periodic = ra_set_periodic,
};

static rt_rtc_dev_t ra_rtc_dev;

static int rt_hw_rtc_init(void)
{
    rt_err_t result;

    ra_rtc_dev.ops = &ra_rtc_ops;

    result = rt_hw_rtc_register(&ra_rtc_dev, "rtc", RT_DEVICE_FLAG_RDWR, RT_NULL);
    if (result != RT_EOK)
    {
        LOG_E("rtc register err code: %d", result);
        return result;
    }
    LOG_D("rtc init success");

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_init);
#endif
