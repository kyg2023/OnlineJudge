#pragma once

// model: 主要用来与数据交互，对外提供访问数据的接口
// 根据题目列表将所有题目信息加载进内存

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <fstream>
#include <stdlib.h>
#include <mysql/mysql.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_model {
    using namespace ns_log;
    using namespace ns_util;

    struct Question {
        std::string id;         // 题目编号
        std::string title;      // 题目标题
        std::string difficulty; // 难度
        std::string desc;       // 题目描述
        std::string header;     // 题目预设代码
        std::string tail;       // 题目测试代码
        int cpu_limit;          // 时间要求
        int mem_limit;          // 空间要求
    };

    const std::string host = "127.0.0.1";
    const std::string user = "oj";
    const std::string password = "kygdmm2023";
    const std::string database = "oj";
    const int port = 3306;

    const std::string table_name = "questions";

    class Model {
        // 连接数据库，执行sql语句，返回结果
        bool QueryMySQL(const std::string& sql, std::vector<Question>& out) {
            // 创建mysql句柄
            MYSQL* mysql = mysql_init(nullptr);
            if (!mysql) {
                LOG(FATAL) << "mysql初始化失败" << std::endl;
                return false;
            }
            // FOR DEBUG
            // std::cout << mysql_get_client_info() << std::endl;

            // 连接数据库
            if(mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, nullptr, 0) == nullptr) {
                LOG(FATAL) << "连接数据库失败" << std::endl;
                LOG(DEBUG) << "错误代码: " << mysql_errno(mysql) << std::endl;
                mysql_close(mysql);
                return false;
            }

            // 设置链接编码
            mysql_set_character_set(mysql, "utf8");

            LOG(INFO) << "连接数据库成功" << std::endl;
            // 执行sql语句
            if(mysql_query(mysql, sql.c_str()) != 0) {
                LOG(ERROR) << "执行SQL语句失败" << std::endl;
                return false;
            }
            // 提取结果集
            MYSQL_RES* res = mysql_store_result(mysql);
            if(res == nullptr) {
                LOG(ERROR) << "获取结果集失败" << std::endl;
                return false;
            }
            // 读取结果集
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))) {
                Question q;
                q.id = row[0];
                q.title = row[1];
                q.difficulty = row[2];
                q.desc = row[3];                
                q.header = row[4];               
                q.tail = row[5];                
                q.cpu_limit = atoi(row[6]);       
                q.mem_limit = atoi(row[7]);       
                out.push_back(q);
            }
            // 释放资源
            mysql_free_result(res); 
            mysql_close(mysql);
            return true;
        }

        bool QueryMySQL(const std::string& sql, Question& out) {
            MYSQL* mysql = mysql_init(nullptr);
            if(mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, nullptr, 0) == nullptr) {
                LOG(FATAL) << "连接数据库失败" << std::endl;
                mysql_close(mysql);
                return false;
            }
            LOG(INFO) << "连接数据库成功" << std::endl;
            if(mysql_query(mysql, sql.c_str()) != 0) {
                LOG(ERROR) << "执行SQL语句失败" << std::endl;
                return false;
            }
            MYSQL_RES* res = mysql_store_result(mysql);
            if(res == nullptr) {
                LOG(ERROR) << "获取结果集失败" << std::endl;
                return false;
            }
            MYSQL_ROW row;
            if((row = mysql_fetch_row(res))) {
                out.id = row[0];
                out.title = row[1];
                out.difficulty = row[2];
                out.desc = row[3];                
                out.header = row[4];               
                out.tail = row[5];                
                out.cpu_limit = atoi(row[6]);       
                out.mem_limit = atoi(row[7]);       
            }
            mysql_free_result(res); 
            mysql_close(mysql);
            return true;
        }

    public:
        bool GetAllQuestions(std::vector<Question>& out) {
            // 从数据库中读取所有题目信息，并将其存入out中
            std::string sql = "SELECT * FROM " + table_name;
            return QueryMySQL(sql, out);
        }

        bool GetOneQuestion(const std::string number, Question& out) {
            std::string sql = "SELECT * FROM " + table_name + " WHERE id = " + number;
            return QueryMySQL(sql, out);
        }
    };
} // namespace ns_model

