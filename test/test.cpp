/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：test.cpp
*   创 建 者：Bai Xiaodong
*   创建日期：2021年03月03日
*   描    述：
*
================================================================*/


#include <iostream>

using namespace std;

int main()
{
    uint16_t time = 0x040F;

    int hour = 0, min = 0;
    int zone = 0;
    while(true){
        cout << "zone: ";
        cin >> zone;

        hour = ((time >> 8) & 0xFF) + zone;
        min = time & 0xFF;

        if(hour > 23){
            hour -= 24;
        }else if(hour < 0){
            hour += 24;
        }
        
        time = (hour << 8) | min;
        cout << "time: " << hex << time << endl;
    }

    return 0;
}

