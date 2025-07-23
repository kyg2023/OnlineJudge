#pragma once

// 负责 正确调用 compile and run
// 适配用户请求

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "compiler.hpp"
#include "runner.hpp"

#include <jsoncpp/json/json.h>
#include <signal.h>
#include <unistd.h>

using namespace ns_compiler;
using namespace ns_runner;

namespace ns_compile_run
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompileRun
    {
        // 输入单纯的文件名
        static void RemoveTempFiles(const std::string& file_name) {
            std::string src = PathUtil::Src(file_name);
            std::string comerr = PathUtil::CompileErr(file_name);
            std::string exe = PathUtil::Exe(file_name);
            std::string in = PathUtil::Stdin(file_name);
            std::string out = PathUtil::Stdout(file_name);
            std::string err = PathUtil::Stderr(file_name);
            if (FileUtil::FileExists(src)) unlink(src.c_str());
            if (FileUtil::FileExists(comerr)) unlink(comerr.c_str());
            if (FileUtil::FileExists(exe)) unlink(exe.c_str());
            if (FileUtil::FileExists(in)) unlink(in.c_str());
            if (FileUtil::FileExists(out)) unlink(out.c_str());
            if (FileUtil::FileExists(err)) unlink(err.c_str());
        }

        // 将 状态码 转为 具体情况
        // status_code > 0 : 进程收到了信号导致异常崩溃
        // status_code < 0 : 非运行报错
        // status_code == 0 : 编译运行成功
        static std::string StatusCode2Desc(const int status_code, const std::string& file_name)
        {
            std::string desc;
            switch (status_code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "用户提交代码为空";
                break;
            case -2:
                desc = "发生未知错误";
                break;
            case -3:
                FileUtil::ReadFile(PathUtil::CompileErr(file_name), desc);
                break;
            case SIGABRT: // 6
                desc = "内存超过限制";
                break;
            case SIGFPE: // 8
                desc = "浮点数溢出";
                break;
            case SIGSEGV: // 11
                desc = "段错误，内存访问违规";
                break;
            case SIGXCPU: // 24
                desc = "CPU使用超时";
                break;
            default:
                desc = "未知: " + std::to_string(status_code);
                break;
            }
            return desc;
        }

    public:
        /****************
         * in_json_str ：
         *  code  : 用户提交的代码块
         *  input : code对应的输入
         *  cpu_limit: 时间限制
         *  mem_limit: 内存限制
         *
         * out_json_str ：
         *  status: 状态码
         *  reason：请求结果
         *  stdout: code运行完的结果（选填）
         *  stderr: code运行完的错误（选填）
         ******************************/
        static void Start(const std::string &in_json_str, std::string& out_json_str)
        {
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json_str, in_value);

            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            Json::Value out_value;

            int status_code = 0;
            int run_res = 0;
            std::string file_name = FileUtil::UniqFileName();
            if (code.size() == 0)
            {
                status_code = -1; // 用户提交代码为空
                goto END;
            }

            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status_code = -2; // 代码写入发生错误
                goto END;
            }

            if (!Compiler::Compile(file_name))
            {
                status_code = -3; // 编译报错
                goto END;
            }
            run_res = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_res < 0)
            {
                status_code = -2;
                goto END;
            }
            else if (run_res > 0)
            {
                status_code = run_res; // 运行报错，对应信号
                goto END;
            }
            else
            { // 运行成功
                status_code = 0;
                goto END;
            }
        END:
            out_value["status"] = status_code;
            out_value["reason"] = StatusCode2Desc(status_code, file_name);
            // 整个过程完全成功
            if (status_code == 0) 
            {
                std::string _stdout;
                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), _stdout);
                FileUtil::ReadFile(PathUtil::Stderr(file_name), _stderr);
                out_value["stdout"] = _stdout;
                out_value["stderr"] = _stderr;
            }

            Json::StyledWriter writer;
            out_json_str = writer.write(out_value);

            RemoveTempFiles(file_name);
        }
    };
}
