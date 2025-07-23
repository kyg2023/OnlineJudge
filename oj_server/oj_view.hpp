#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctemplate/template.h>

// #include "oj_model_file.hpp"
#include "oj_model_mysql.hpp"

namespace ns_view {
    using namespace ns_model;

    const std::string template_path = "./template_html/";
    class View {
    public:
        // 渲染所有题目列表网页
        void ExpandTemplate(const std::vector<Question>& qs, std::string& out_html_str) {
            // 编号 标题 难度
            std::string src_html_path = template_path + "all_questions.html";
            ctemplate::TemplateDictionary dict("all_questions");
            for(const auto& q : qs) {
                ctemplate::TemplateDictionary* sub = dict.AddSectionDictionary("questions_list");
                sub->SetValue("id", q.id);
                sub->SetValue("title", q.title);
                sub->SetValue("difficulty", q.difficulty);
            }
            ctemplate::ExpandTemplate(src_html_path, ctemplate::DO_NOT_STRIP, &dict, &out_html_str);
        }

        // 渲染单个题目详情网页
        void ExpandTemplate(const Question& q, std::string& out_html_str) {
            std::string src = template_path + "one_question.html";
            ctemplate::TemplateDictionary dict("one_question");
            dict.SetValue("id", q.id);
            dict.SetValue("title", q.title);
            dict.SetValue("difficulty", q.difficulty);
            dict.SetValue("desc", q.desc);
            dict.SetIntValue("cpu_limit", q.cpu_limit);
            dict.SetIntValue("mem_limit", q.mem_limit);
            dict.SetValue("pre_code", q.header);
            ctemplate::ExpandTemplate(src, ctemplate::DO_NOT_STRIP, &dict, &out_html_str);
        }
    };

}
