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

    const std::string questions_list_path = "./questions/questions.list";
    const std::string questions_root = "./questions/";

    class Model {
        std::unordered_map<std::string, Question> _questions;
    private:
        // 将所有题目信息加载进内存
        bool LoadQuestions(const std::string& path) {
            std::ifstream fin(path);
            if(!fin.is_open()) {
                LOG(FATAL) << "加载题库失败，请检查题库是否存在！！！" << '\n';
                return false;
            }

            std::string line;
            while(getline(fin, line)) {
                std::vector<std::string> tokens;
                StringUtil::SpiltString(line, tokens, " ");
                if(tokens.size() != 5) {
                    LOG(WARNING) << "某个题目文件格式有问题！！！" << '\n';
                    continue;
                }
                Question q;
                q.id = tokens[0];
                q.title = tokens[1];
                q.difficulty = tokens[2];
                q.cpu_limit = atoi(tokens[3].c_str());
                q.mem_limit = atoi(tokens[4].c_str());
                std::string ques_root = questions_root + q.id;
                if(FileUtil::ReadFile(ques_root + "/" + "desc.txt", q.desc) && 
                   FileUtil::ReadFile(ques_root + "/" + "header.cpp", q.header) &&
                   FileUtil::ReadFile(ques_root + "/" + "tail.cpp", q.tail)) {
                    _questions[q.id] = q;
                }
                else {
                    LOG(WARNING) << "题目[" + q.id + "]文件有缺失！！！" << '\n';
                }
            }
            LOG(INFO) << "加载题库完成！！！" << '\n';
            fin.close();
            return true;
        }
    public:
        Model() {
            assert(LoadQuestions(questions_list_path));
        }

        bool GetAllQuestions(std::vector<Question>& out) {
            if(_questions.size() == 0) {
                LOG(ERROR) << "用户获取题库失败，题库为空！！！" << '\n';
                return false;
            }
            for(auto& iter : _questions) {
                out.push_back(iter.second);
            }
            return true;
        }

        bool GetOneQuestion(const std::string number, Question& out) {
            const auto& iter = _questions.find(number);
            if(iter == _questions.end()) {
                LOG(ERROR) << "用户获取题目[" + number + "]失败！！！" << '\n';
                return false;
            }
            out = iter->second;
            return true;
        }
    };
} // namespace ns_model

