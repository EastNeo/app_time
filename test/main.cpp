/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：main.cpp
*   创 建 者：Bai Xiaodong
*   创建日期：2021年03月02日
*   描    述：
*
================================================================*/


#include <iostream>
#include <cstdint>
#include <chrono>
#include <thread>
#include <cstring>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "../src/appointment_struct.h"

using namespace std;

int MQ1_ID;

int main(int argc, char** argv)
{
    key_t key;

    //key = ftok("/work/tmp/app_bak/bin/", 1);
    key = ftok("/mnt/", 1);
    if(-1 == key)
    {
        cout << "key failed" << endl;
        return -1;
    }

    cout << "key: " << hex << key << endl;

    MQ1_ID = msgget(key, IPC_CREAT|0644);
    if(-1 == MQ1_ID)
    {
        cout << "msgget failed" << endl;
        return -1;
    }
    else
    {
        cout << "MQ1_ID: " << MQ1_ID << endl;
    }

    uint16_t time = 0;
    cout << "time1: ";
    cin >> time;

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
    _ap_t.time_exce = time;

    MsgData msgdata;
    memset(&msgdata, 0, sizeof(msgdata));
    msgdata.ap_count++;
    memcpy(&msgdata.ap_l[0], &_ap_t, sizeof(_ap_t));
    
    ap_t ap1;
    memcpy(&ap1, &_ap_t, sizeof(ap1));
    ap1.week = 0x04;
    cout << "time2: ";
    cin >> time;
    ap1.time_exce = time;
    msgdata.ap_count++;
    memcpy(&msgdata.ap_l[1], &ap1, sizeof(ap1));

    int count = 1;
    while(count){
        cout << "msg send" << endl;
        msgsnd(MQ1_ID, &msgdata, sizeof(msgdata), IPC_NOWAIT);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        count--;
    }


    return 0;
}
