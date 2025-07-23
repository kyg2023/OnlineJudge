#include <iostream>
#include <signal.h>

#include "../comm/httplib.h"
#include "oj_controller.hpp"

using namespace httplib;
using namespace ns_controller;

Controller* ctrlr_ptr = nullptr;

void signal_handler(int sig) {
    if (ctrlr_ptr) {
        ctrlr_ptr->Recovery();
    }
}

int main() {
    signal(SIGQUIT, signal_handler);

    Server svr;
    Controller ctrlr;
    ctrlr_ptr = &ctrlr;

    svr.set_base_dir("./wwwroot");

    // 获取所有题目列表  返回一张包含所有题目的html网页
    svr.Get("/all_questions", [&ctrlr](const Request& req, Response& resp){
        std::string html_str;
        ctrlr.GetAllQuestionsHtml(html_str);
        resp.set_content(html_str, "text/html; charset=utf-8");
    });

    // 用户根据题目编号获取题目内容   
    // (\d+)：捕获组，匹配一个或多个数字字符（\d 是正则语法，不是 C++ 转义）
    svr.Get(R"(/question/(\d+))", [&ctrlr](const Request& req, Response& resp){
        std::string ques_id = req.matches[1];
        std::string html_str;
        ctrlr.GetQuestionHtml(ques_id, html_str);
        resp.set_content(html_str, "text/html; charset=utf-8");
    });

    // 用户提交代码，使用我的判题功能
    svr.Post(R"(/judge/(\d+))", [&ctrlr](const Request& req, Response& resp){
        std::string ques_id = req.matches[1];
        std::string json_str;
        ctrlr.Judge(ques_id, req.body, json_str);
        resp.set_content(json_str, "appilcate/json; charset=utf-8");
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}
