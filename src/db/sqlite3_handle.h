/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：sqlite3_handle.h
*   创 建 者：Bai Xiaodong
*   创建日期：2021年02月25日
*   描    述：
*
================================================================*/


#pragma once
#include "sqlite3.h"
#include <sys/sem.h>
#include <iostream>
#include <list>
#include <string>
#include <mutex>
#include <cstring>


class sqlite3Handle
{
private:
    sqlite3Handle();
    ~sqlite3Handle();
public:
    static sqlite3Handle* getInstance(){
        std::call_once(oc_, [&](){
            instance = new sqlite3Handle();
        });

        return instance;
    }
    sqlite3* sqlite3_open_db(const std::string dbName);
    bool isTableExist(const std::string tableName, sqlite3* db);
    bool sqlite3_create_table(const std::string tableName, sqlite3* db);
    template<class T>
    bool sqlite3_insert_data(const std::string tableName, const T ap, const int recordId, sqlite3* db)
    {
        char* zErrMsg = NULL;
        char sql[1000];
        int rc;

        sqlite3_stmt* stmt;
        std::sprintf(sql, "INSERT INTO %s values('%d',?);", tableName.c_str(), recordId);
        rc = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
        if(SQLITE_OK == rc)
        {
            sqlite3_bind_blob(stmt, 1, &ap, sizeof(ap), NULL);
            sqlite3_step(stmt);
        }
        else
        {
            if(stmt)
            {
                sqlite3_finalize(stmt);
            }
            std::cout << "insert table data failed" << std::endl;
            return false;
        }

        sqlite3_finalize(stmt);

        return true;
    }
    bool sqlite3_delete_table(const std::string tableName, sqlite3* db);
    bool sqlite3_clear_data(const std::string tableName, sqlite3* db);
    bool sqlite3_clear_data(const std::string tableName, int recordId, sqlite3* db);
    template<class T>
    bool sqlite3_select_data(const std::string tableName, std::list<T> &_ap_t, sqlite3* db)
    {
        char* zErrMsg = NULL;
        char sql[1000];
        int rc;

        sqlite3_stmt* stmt;
        std::sprintf(sql, "SELECT * FROM %s;", tableName.c_str());

        rc = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
        if(SQLITE_OK != rc)
        {
            std::cout << "select data failed" << std::endl;
            if(stmt)
            {
                sqlite3_finalize(stmt);
            }
            return false;
        }

        int result = sqlite3_step(stmt);
        int id = 0, len = 0;
        while(SQLITE_ROW == result)
        {
            T appointment;
            const void* readApData = sqlite3_column_blob(stmt, 1);
            len = sqlite3_column_bytes(stmt, 1);

            memcpy(&appointment, readApData, len);
            _ap_t.push_back(appointment);
            result = sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
        
        return true;
    }
    template<class T>
    bool sqlite3_select_data(const std::string tableName, int recordId, T &_ap_t, sqlite3* db)
    {
        char* zErrMsg = NULL;
        char sql[1000];
        int rc;

        sqlite3_stmt* stmt;
        std::sprintf(sql, "SELECT * FROM %s WHERE ID = %d;", tableName.c_str(), recordId);

        rc = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
        if(SQLITE_OK != rc)
        {
            std::cout << "select data failed" << std::endl;
            if(stmt)
            {
                sqlite3_finalize(stmt);
            }
            return false;
        }

        int result = sqlite3_step(stmt);
        int id = 0, len = 0;

        const void* readApData = sqlite3_column_blob(stmt, 1);
        len = sqlite3_column_bytes(stmt, 1);

        memcpy(&_ap_t, readApData, len);

        return true;
    }
    bool sqlite3_close_db(sqlite3* db);

private:
    static sqlite3Handle* instance;
    static std::once_flag oc_;
};


