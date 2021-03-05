/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：sqlite3_handle.cpp
*   创 建 者：Bai Xiaodong
*   创建日期：2021年02月25日
*   描    述：
*
================================================================*/


#include "sqlite3_handle.h"

sqlite3Handle* sqlite3Handle::instance = NULL;
std::once_flag sqlite3Handle::oc_;

sqlite3Handle::sqlite3Handle()
{
}

sqlite3Handle::~sqlite3Handle()
{
    std::cout << "close database" << std::endl;
}

sqlite3* sqlite3Handle::sqlite3_open_db(const std::string dbName)
{
    sqlite3* db = NULL;
    int result = sqlite3_open(dbName.c_str(), &db);
    if(SQLITE_OK != result)
    {
        sqlite3_close(db);
        std::cout << "open sqlite failed" << std::endl;
        return NULL;
    }

    return db;
}

bool sqlite3Handle::isTableExist(const std::string tableName, sqlite3* db)
{
    char* zErrMsg = NULL;
    char sql[1000];
    int rc;

    sqlite3_stmt* stmt = NULL;

    std::sprintf(sql, "SELECT * FROM sqlite_master WHERE type = 'table' AND name = '%s';", tableName.c_str());
    rc = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
    if(SQLITE_OK != rc)
    {
        if(stmt)
        {
            sqlite3_finalize(stmt);
            std::cout << "table select failed" << std::endl;
            return false;
        }
    }

    rc = sqlite3_step(stmt);
    if(rc == SQLITE_DONE)
    {
        std::cout << "table doesn't exist" << std::endl;
        return false;
    }
    else if(rc == SQLITE_ROW)
    {
        std::cout << "table had exist" << std::endl;
    }
    
    return true;
}

bool sqlite3Handle::sqlite3_create_table(const std::string tableName, sqlite3* db)
{
    bool res = isTableExist(tableName, db);
    if(res == true)
    {
        std::cout << "table had exist, don't need to create" << std::endl;
        return true;
    }

    char* zErrMsg = NULL;
    char sql[1000];
    int rc;

    std::sprintf(sql, "CREATE TABLE %s(ID INT PRIMARY KEY NOT NULL, DATA BLOB);", tableName.c_str());
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if(SQLITE_OK != rc)
    {
        std::cout << "create table failed" << std::endl;
        std::cout << "Error Msg: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}

bool sqlite3Handle::sqlite3_delete_table(const std::string tableName, sqlite3* db)
{
    char* zErrMsg = NULL;
    char sql[1000];
    int rc;

    std::sprintf(sql, "DROP TABLE %s;", tableName.c_str());
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if(SQLITE_OK != rc)
    {
        std::cout << "delete table falied" << std::endl;
        std::cout << "Error Msg: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}

bool sqlite3Handle::sqlite3_clear_data(const std::string tableName, sqlite3* db)
{
    char* zErrMsg = NULL;
    char sql[1000];
    int rc;

    std::sprintf(sql, "DELETE FROM %s;", tableName.c_str());
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if(SQLITE_OK != rc)
    {
        std::cout << "clear data failed" << std::endl;
        std::cout << "Error Msg: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}

bool sqlite3Handle::sqlite3_clear_data(const std::string tableName, int recordId, sqlite3* db)
{
    char* zErrMsg = NULL;
    char sql[1000];
    int rc;

    std::sprintf(sql, "DELETE FROM %s WHERE ID = %d;", tableName.c_str(), recordId);
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if(SQLITE_OK != rc)
    {
        std::cout << "clear data failed" << std::endl;
        std::cout << "Error Msg: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}

bool sqlite3Handle::sqlite3_close_db(sqlite3* db)
{
    sqlite3_close(db);
    std::cout << "manual close db" << std::endl;
    return true;
}
