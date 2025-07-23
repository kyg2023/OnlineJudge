
// 测试资源限制
// #include <iostream>
// #include <sys/time.h>
// #include <sys/resource.h>
// #include <unistd.h>
// #include <signal.h>

// void handler(int signo) {
//     std::cout << "signo : " << signo << std::endl;
//     exit(1);
// }

// int main() {
//     for(int i = 1; i <= 31; i++) {
//         signal(i, handler);
//     }

//     // 测试限制运行时长  
//     struct rlimit rl;
//     rl.rlim_cur = 1;
//     rl.rlim_max = RLIM_INFINITY;
//     setrlimit(RLIMIT_CPU, &rl);
//     while(1);
//     // 超出限制被 信号6 SIGABRT 终止

//     // struct rlimit rl;
//     // rl.rlim_cur = 1024*1024*20; // 20M
//     // rl.rlim_max = RLIM_INFINITY;
//     // setrlimit(RLIMIT_AS, &rl);
//     // int count = 0;
//     // while(1)
//     // {
//     //     int* p = new int[1024*1024];
//     //     count++;
//     //     std::cout << count << "M" << std::endl;
//     //     sleep(1);
//     // }
//     // 超出限制被 信号24 SIGXCPU 终止

//     return 0;
// }


// 测试json序列化/反序列化
// #include <iostream>
// #include <jsoncpp/json/json.h>

// int main() {
//     // 序列化
//     Json::Value root;
//     root["code"] = "code";
//     root["user"] = "kyg";
//     root["age"] = "24";
//     Json::FastWriter writer;
//     // Json::StyledWriter writer;
//     std::string str = writer.write(root);
//     std::cout << str << '\n';

//     return 0;
// }


// 测试 boost::split 切分字符串
// #include <iostream>
// #include <vector>
// #include <boost/algorithm/string.hpp>

// using namespace boost;

// int main() {
//     std::string str = "1:判断回文数:::简单:1:30000";
//     std::vector<std::string> tokens;
//     std::string sep = ":";
//     boost::split(tokens, str, is_any_of(sep), boost::algorithm::token_compress_on);
//     for(auto& str : tokens) {
//         std::cout << str << std::endl;
//     }

//     return 0;
// }


#include <iostream>
#include <string>
#include <ctemplate/template.h>

int main() {
    std::string in_html_file_name = "./test.html";
    std::string out_html_str;

    std::string value = "测试使用ctemplate";
    // 创建数据字典
    ctemplate::TemplateDictionary dict("test");
    dict.SetValue("key", value);

    // 旧版写法
    // ctemplate::Template* tpl = ctemplate::Template::GetTemplate(in_html_file_name, ctemplate::DO_NOT_STRIP); // 获取被渲染网页对象
    // tpl->Expand(&out_html_str, &dict); // 添加字典数据到网页

    // 新版写法 
    ctemplate::ExpandTemplate(in_html_file_name, ctemplate::DO_NOT_STRIP, &dict, &out_html_str);

    std::cout << out_html_str << std::endl;

    return 0;
}
