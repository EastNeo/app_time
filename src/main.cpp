/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：main.cpp
*   创 建 者：Bai Xiaodong
*   创建日期：2021年02月26日
*   描    述：
*
================================================================*/


#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "appointment_struct.h"
#include "appointment_monitor.h"
#include "db/sqlite3_handle.h"

using namespace std;

bool g_exit_thread = false;

//const char* dbName = "/mnt/UDISK/test.db";
const char* dbName = "/mnt/d/tmp/appointment/bin/test.db";
const char* tableName = "APPOINTMENT";

int main(int argc, char** argv)
{
   /* 
    ap_t _ap_t;
    _ap_t.clean_mode = 0x01;
    _ap_t.effect = true;
    _ap_t.fan_level = 0x01;
    _ap_t.water_level = 0x00;
    _ap_t.week = 0x00;
    _ap_t.numbers = 2;
    _ap_t.room_id[0] = 1;
    _ap_t.room_id[1] = 2;
    _ap_t.sweep_count = 0x01;
    _ap_t.time_zone = 0x03;
    _ap_t.time_exce = 0x0B04;
*/

    cout << "main start" << endl;
    auto p_clock_thread = Clock_Handle::getInstance();
    thread bv_clock_thread(&Clock_Handle::clock_monitor_thread, p_clock_thread);
    bv_clock_thread.detach();
    
    thread bv_read_clock(&Clock_Handle::read_from_server_thread, p_clock_thread);
    bv_read_clock.detach();
    

    while(!g_exit_thread){
        cout << "main" << endl;

        std::this_thread::sleep_for(chrono::milliseconds(5000));
    }

}
/*{
    sqlite3Handle sql;

    bool rc = sql.sqlite3_open_db(dbName);

    if(!rc)
    {
        cout << "open database failed" << endl;
        return -1;
    }
    
    sql.sqlite3_create_table(tableName);
    
    ap_t _ap_t;
    _ap_t.clean_mode = 0x01;
    _ap_t.effect = true;
    _ap_t.fan_level = 0x01;
    _ap_t.water_level = 0x00;
    _ap_t.week = 0x00;
    _ap_t.numbers = 2;
    _ap_t.room_id[0] = 1;
    _ap_t.room_id[1] = 2;
    _ap_t.sweep_count = 0x01;
    _ap_t.time_zone = 0x03;
    _ap_t.time_exce = 0x0C32;

    ap_t tmp;
    memcpy(&tmp, &_ap_t, sizeof(_ap_t));
    tmp.time_exce = 0x0B1E;
    sql.sqlite3_insert_data<ap_t>(tableName, _ap_t, 1);
    sql.sqlite3_insert_data<ap_t>(tableName, tmp, 2);

    list<ap_t> ap_l;
    sql.sqlite3_select_data<ap_t>(tableName, ap_l);

    for(auto it : ap_l)
    {
        cout << "time_exce: " << hex << it.time_exce << endl;
    }

    sql.sqlite3_clear_data(tableName, 1);
    return 0;
}
*/
