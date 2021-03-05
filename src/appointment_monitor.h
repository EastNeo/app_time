/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：appointment_handle.h
*   创 建 者：Bai Xiaodong
*   创建日期：2021年02月25日
*   描    述：
*
================================================================*/


#pragma once

#include "appointment_struct.h"
#include "db/sqlite3_handle.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

#include <list>
#include <mutex>
#include <string>
#include <thread>

#define gettid() syscall(SYS_getpid)

extern bool g_exit_thread;
extern int MQ1_ID;

enum TbEnum{
    KTbMin = -1,
    KTbApp,
    KTbDtb,
    KTbBoth,
    KTbMax
};


class Clock_Handle
{
public:
    static Clock_Handle* getInstance(){
        std::call_once(oc_, [&](){
            instance = new Clock_Handle();
        });

        return instance;
    }

    bool read_data_from_db(TbEnum tb_enum);
    void clock_monitor_thread();
    bool initMsgQ();
    void change_time_local();
    void sort_apTime();
    void write_ap_to_db();
    void write_dtb_to_db();
    bool write_data_to_db(TbEnum tb_enum);
    void read_from_server_thread();

    void ap_clock_action();
    int get_time_zone() const
    {
        int timezone = 0;
        time_t t1, t2;
        struct tm *tm_local, *tm_utc;

        time(&t1);
        tm_utc = gmtime(&t1);
        t2 = mktime(tm_utc);

        timezone = (t1 - t2) / 3600;

        return timezone;        
    }
private:
    static Clock_Handle* instance;
    static std::once_flag oc_;
    sqlite3Handle* sql;
    sqlite3* db;
    const std::string db_path = "/mnt/UDISK/test.db";
    //const char* db_path = "/mnt/d/tmp/appointment/bin/test.db";
    //const std::string db_path = "/work/tmp/app_time/bin/test.db";
    const std::string ap_table_name = "APPOINTMENT";
    const std::string dtb_table_name = "DISTURB";

    std::mutex ap_mut;
    std::list<ap_t> apTimeList;
    std::list<ap_t> apTimeFromServer;
    bool ap_flag = false;
    std::mutex dtb_mut;
    std::list<dtb_t> dtbTimeList;
    std::list<dtb_t> dtbTimeFromServer;
    bool dtb_flag = false;
};
