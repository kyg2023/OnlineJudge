#pragma once

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler 
{
    using namespace ns_util;
    using namespace ns_log;

    class Compiler 
    {
    public:
        // 输入参数：需要编译的文件名 ./temp/xxx.cpp
        // 返回值：编译成功为ture，失败false
        static bool Compile(const std::string& file_name) {
            pid_t pid = fork();
            if(pid < 0) {
                LOG(ERROR) << "编译时，创建子进程失败" << "\n";
                return false;
            }
            else if(pid == 0) { // 子进程：调用编译器完成编译
                // 1.创建错误文件
                umask(0);
                int compile_err_fd = open(PathUtil::CompileErr(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                if(compile_err_fd < 0) {
                    LOG(WARNING) << "编译时，创建错误输出文件失败" << "\n";
                    exit(1);
                }
                // 2.重定向标准错误到错误文件
                dup2(compile_err_fd, 2);
 
                // g++ -o target src -std=c++11
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(), PathUtil::Src(file_name).c_str(), "-std=c++11" , "-D", "COMPILE",nullptr);
                LOG(ERROR) << "编译时，启动 g++ 编译器失败" << "\n";
                exit(2);
            }
            else{ // 父进程
                waitpid(pid, nullptr, 0);
                // 通过可执行程序是否存在 判断是否编译成功
                if(FileUtil::FileExists(PathUtil::Exe(file_name))) {
                    LOG(INFO) << PathUtil::Src(file_name).c_str() << "编译成功" << "\n";
                    return true;
                }
                LOG(ERROR) << "编译失败，未生成可执行程序" << "\n";
                return false;
            }
        }
    };
}