#include "compile_run.hpp"
#include "../comm/httplib.h"

using namespace ns_compile_run;
using namespace httplib;

void Usage(const std::string& proc) {
    std::cerr << "Usage:" << "\n\t" << proc << "  [prot]" << std::endl;
}

int main(int argc, char* argv[]) {
//     Json::Value in_value;
//     in_value["code"] = R"(#include <iostream>

// int main() {
//     std::cout << "heheheeeeeee" << std::endl;
//     int a = 1;
//     return 0;
// })";
//     in_value["input"] = "";
//     in_value["cpu_limit"] = 1;
//     in_value["mem_limit"] = 10240 * 10; //单位 KB

//     Json::FastWriter writer;
//     std::string in_json_str = writer.write(in_value);
//     std::string out_json_str;    
//     CompileRun::Start(in_json_str, out_json_str);
//     std::cout << out_json_str << std::endl;

    if(argc != 2) {
        Usage(argv[0]);
        return 1;
    }

    Server svr;
    svr.Get("/", [](const Request& req, Response& resp){
        resp.set_content("hellow httplib, 你好 httplib", "text/plain; charset=utf-8");
    });

    svr.Post("/compile_run", [](const Request& req, Response& resp){
        std::string in_json_str = req.body;
        std::string out_json_str;
        if(!in_json_str.empty()) {
            CompileRun::Start(in_json_str, out_json_str);
            resp.set_content(out_json_str, "application/json; charset=utf-8");
        }
    });

    int port = atoi(argv[1]);
    if (!svr.listen("0.0.0.0", port)) {
        std::cerr << "端口 " << port << " 已被占用，服务器启动失败！" << std::endl;
        return 1;
    }

    return 0;
}
