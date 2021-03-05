/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：appointment_handle.cpp
*   创 建 者：Bai Xiaodong
*   创建日期：2021年02月25日
*   描    述：
*
================================================================*/


#include <algorithm>
#include <sys/prctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "appointment_monitor.h"


int MQ1_ID;
Clock_Handle* Clock_Handle::instance = NULL;
std::once_flag Clock_Handle::oc_;


//从数据库读取时间列表
bool Clock_Handle::read_data_from_db(TbEnum tb_enum)
{
    std::cout << "read_data_from_db" << std::endl;
    if(tb_enum >= +TbEnum::KTbMax || tb_enum <= +TbEnum::KTbMin)
    {
        std::cout << "no table" << std::endl;
        return false;
    }

    switch(tb_enum){
        case KTbApp:
            if(sql->isTableExist(ap_table_name, db)){
                std::lock_guard<std::mutex> lck(ap_mut);
                apTimeList.clear();
                sql->sqlite3_select_data<ap_t>(ap_table_name, apTimeList, db);
            }
            break;
        case KTbDtb:
            if(sql->isTableExist(dtb_table_name, db)){
                std::lock_guard<std::mutex> lck(dtb_mut);
                dtbTimeList.clear();
                sql->sqlite3_select_data<dtb_t>(dtb_table_name, dtbTimeList, db);
            }
            break;
        case KTbBoth:
            if(sql->isTableExist(ap_table_name, db)){
                std::lock_guard<std::mutex> lck(ap_mut);
                apTimeList.clear();
                sql->sqlite3_select_data<ap_t>(ap_table_name, apTimeList, db);
            }
            if(sql->isTableExist(dtb_table_name, db)){
                std::lock_guard<std::mutex> lck(dtb_mut);
                dtbTimeList.clear();
                sql->sqlite3_select_data<dtb_t>(dtb_table_name, dtbTimeList, db);
            }
            break;
        default:
            std::cout << "read nothing" << std::endl;
            return false;
    }


    for(auto it : apTimeList)
    {
        std::cout << "ap time: " << it.time_exce << std::endl;
    }
    for(auto it : dtbTimeList)
    {
        std::cout << "dtb time: " << it.start_time << " " << it.end_time << std::endl;
    }
    return true;
}

//监控线程
void Clock_Handle::clock_monitor_thread()
{
    std::cout << "monitor thread start" << std::endl;
    //设置线程名称，获取线程id
    prctl(PR_SET_NAME, "bv_appointment_monitor");
    pthread_t tid = gettid();

    sql = sqlite3Handle::getInstance();

    db = sql->sqlite3_open_db(db_path);

    //将数据库中的数据读取到内存中
    read_data_from_db(TbEnum::KTbBoth);

    while(!g_exit_thread){
        //预约时间队列改变，写入到数据库
        if(ap_flag)
        {
            std::lock_guard<std::mutex> lck(ap_mut);
            write_data_to_db(TbEnum::KTbApp);
            ap_flag = false;
        }
        //勿扰时间队列改变，写入到数据库
        if(dtb_flag)
        {
            std::lock_guard<std::mutex> lck(dtb_mut);
            write_data_to_db(TbEnum::KTbDtb);
            dtb_flag = false;
        }

        //判断勿扰模式是否开启，预约时间列表是否为空
        if(!dtb_clock_action() && !apTimeList.empty())
        {
            ap_clock_action();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    }
    sql->sqlite3_close_db(db);
}

//初始化消息队列
bool Clock_Handle::initMsgQ()
{
    key_t key;

    //key = ftok("/work/tmp/app_bak/bin/", 1);
    key = ftok("/mnt/", 1);
    if(-1 == key)
    {
        std::cout << "ftok failed" << std::endl;
        return false;
    }

    std::cout << "key: " << hex << key << std::endl;

    MQ1_ID = msgget(key, IPC_CREAT|0644);

    if(-1 == MQ1_ID)
    {
        std::cout << "init MQ1_ID failed!" << std::endl;
        std::cout << "error msg: " << strerror(errno);
        return false;
    }
    else
    {
        std::cout << "The MQ1_ID = " << MQ1_ID << std::endl;
    }

    return true;
}

//将手机所在时区的时间转换成扫地机所在时区的时间
void Clock_Handle::change_time_local()
{
    int diff_zone = 0;
    int hour = 0, min = 0;
    //获取扫地机所在时区
    int local_zone = get_time_zone();

    auto it = apTimeFromServer.begin();

    for(; it!=apTimeFromServer.end(); ++it){
        //计算时区差
        diff_zone = local_zone - it->time_zone;
        //转换时间
        hour = ((it->time_exce >> 8) & 0xFF) + diff_zone;
        min = it->time_exce & 0xFF;
        if(hour > 23)
        {
            hour -= 24;
            //week转换
        }
        else if(hour < 0)
        {
            hour += 24;
            //week转换
        }
        it->time_exce = (hour << 8) | min;
    }
}

static bool ap_compare(const ap_t ap1, const ap_t ap2)
{
    return ap1.time_exce < ap2.time_exce;
}


//将从服务器接收的预约时间队列按时间升序排列
void Clock_Handle::sort_apTime()
{
    apTimeFromServer.sort(ap_compare);
}


void Clock_Handle::write_ap_to_db()
{
    sort_apTime();
    sql->sqlite3_clear_data(ap_table_name, db);
    sql->sqlite3_create_table(ap_table_name, db);
    int i = 0;
    for(auto it : apTimeFromServer){
        std::cout << "ap time: " << it.time_exce << std::endl;;
        sql->sqlite3_insert_data<ap_t>(ap_table_name, it, i++, db);
    }
}

void Clock_Handle::write_dtb_to_db()
{
    sql->sqlite3_clear_data(dtb_table_name, db);
    sql->sqlite3_create_table(dtb_table_name, db);
    int i = 0;
    for(auto it : dtbTimeFromServer){
        std::cout << "dtb time: " << it.start_time << " " << std::endl;;
        sql->sqlite3_insert_data<dtb_t>(dtb_table_name, it, i++, db);
    }
}

//将从服务器接收的时间队列存到数据库和内存中
bool Clock_Handle::write_data_to_db(TbEnum tb_enum)
{
    if(tb_enum >= +TbEnum::KTbMax || tb_enum <= +TbEnum::KTbMin)
    {
        std::cout << "no table" << std::endl;
        return false;
    }
    
    switch(tb_enum){
        case KTbApp:
            write_ap_to_db();
            break;
        case KTbDtb:
            write_dtb_to_db();
            break;
        case KTbBoth:
            write_ap_to_db();
            write_dtb_to_db();
            break;
        default:
            std::cout << "write nothing" << std::endl;
            return false;
    }
    return true;
}

//从服务器接受消息线程
void Clock_Handle::read_from_server_thread()
{    
    std::cout << "read thread start" << std::endl;
    //设置线程名称，获取线程id
    prctl(PR_SET_NAME, "bv_read_clock");
    pthread_t tid = gettid();

    //初始化消息队列
    while(!initMsgQ()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }

    MsgData msgdata;

    while(!g_exit_thread){
        memset(&msgdata, 0, sizeof(msgdata));

        std::cout << "waiting for message from MQ1_ID..." << std::endl;
        auto len = msgrcv(MQ1_ID, &msgdata, sizeof(msgdata), 0x0L, 0);
        
        if(msgdata.ap_flag){
            apTimeFromServer.clear();
            for(int i = 0; i < msgdata.ap_count; ++i)
            {
                std::cout << "i: " << i << std::endl;
                apTimeFromServer.push_back(msgdata.ap_l[i]);
            }
            std::lock_guard<std::mutex> ap_lck(ap_mut);
            apTimeList.clear();
            apTimeList = apTimeFromServer;
            ap_flag = true;
            for(auto it : apTimeList)
            {
                std::cout << "time: " << hex << it.time_exce << std::endl;
            }
        }
        if(msgdata.dtb_flag){
            for(int i = 0; i < msgdata.dtb_count; ++i)
            {
                dtbTimeFromServer.push_back(msgdata.dtb_l[i]);
            }
            std::lock_guard<std::mutex> dtb_lck(dtb_mut);
            dtbTimeList.clear();
            dtbTimeList = dtbTimeFromServer;
            dtb_flag = true;
            for(auto it : dtbTimeList)
            {
                std::cout << "time: " << hex << it.start_time << it.end_time << std::endl;
            }
        }

        std::cout << "read msg end" << std::endl;
    }
}

//判断是否在勿扰时间段内
bool Clock_Handle::dtb_clock_action()
{
    std::cout << "dtb action start" << std::endl;

    bool dtb_action = false;
    time_t t1;
    struct tm *tm_local;

    time(&t1);
    tm_local = localtime(&t1);
    uint16_t cur_time = (tm_local->tm_hour << 8) | tm_local->tm_min;

    auto it = dtbTimeList.begin();
    for(; it != dtbTimeList.end(); ++it)
    {
        //勿扰时间段在同一天内
        if(it->start_time < it->end_time)
        {
            if((cur_time >= it->start_time) && (cur_time <= it->end_time))
                dtb_action = true;
            else
                dtb_action = false;
        }
        //勿扰结束时间在第二天
        else
        {
            if((cur_time >= it->start_time) && (cur_time <= (it->end_time + (24 << 8))))
                dtb_action = true;
            else
                dtb_action = false;
        }
    }

    return dtb_action;
}

//判断是否到了预约时间
void Clock_Handle::ap_clock_action()
{
    std::cout << "ap action start" << std::endl;
    time_t t1;
    struct tm *tm_local;
    
    time(&t1);

    tm_local = localtime(&t1);
    uint16_t cur_time = (tm_local->tm_hour << 8) | tm_local->tm_min;

    auto it = apTimeList.begin();
    for(; it != apTimeList.end(); ++it)
    {
        //判断本次预约是否有效
        if(it->effect){
            //判断是否到预约时间
            if(cur_time == it->time_exce)
            {
                //判断执行星期
                //0x00 仅执行一次
                if(it->week == 0x00)
                {
                    it->effect = false;
                    std::cout << "exec once" << std::endl;
                }
                //判断是否是星期日
                else if(((it->week >> (7-1)) & 0x1) && tm_local->tm_wday == 0)
                {
                    std::cout << "sunday exec" << std::endl;
                }
                //判断除星期日外的其他天数
                else if(tm_local->tm_wday != 0)
                {
                    if((it->week >> (tm_local->tm_wday-1)) & 0x1)
                    {
                        std::cout << "week " << tm_local->tm_wday << " exec" << std::endl;
                    }
                }
            }
        }
    }

}
