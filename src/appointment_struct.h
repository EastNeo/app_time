/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：appointment_struct.h
*   创 建 者：Bai Xiaodong
*   创建日期：2021年02月25日
*   描    述：
*
================================================================*/


#pragma once

#include <cstdint>

using namespace std;

//预约定时
typedef struct AppintmentTime{
    //手机系统时区
    int8_t time_zone;
    //定时条数
    uint8_t numbers;
    //定时是否有效
    bool effect;
    //执行星期，00为执行一次，低位为周一
    uint8_t week;
    //执行时间, 低位为分
    uint16_t time_exce;
    //房间个数
    uint8_t room_numbers;
    //房间标识
    uint8_t room_id[15];
    //工作模式，00为扫拖、01为仅扫、02为仅拖
    uint8_t clean_mode;
    //风机挡位，最大五档，00，01，02，03，04
    uint8_t fan_level;
    //水箱挡位，同上
    uint8_t water_level;
    //扫地清扫次数
    uint8_t sweep_count;
}ap_t;

//勿扰定时
typedef struct DisturbTime{
    //手机系统时区
    uint8_t time_zone;
    //表示当天或者第二天，00当天，01第二天，目前未使用，直接置00
    uint8_t day;
    //勿扰定时开始时间
    uint16_t start_time;
    //勿扰定时结束时间
    uint16_t end_time;
}dtb_t;

