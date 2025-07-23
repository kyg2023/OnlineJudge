#pragma once

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>


#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_runner
{
    using namespace ns_util;
    using namespace ns_log;

    class Runner {
        public:
        static void SetProcLimit(int cpu_limit, int mem_limit) {
            struct rlimit cpu_rl;
            cpu_rl.rlim_cur = cpu_limit;
            cpu_rl.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &cpu_rl);

            struct rlimit mem_rl;
            mem_rl.rlim_cur = mem_limit * 1024;
            mem_rl.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_AS, &mem_rl);
        }


        /*******************************
         * 程序运行结果：
         * 1.代码运行成功，结果正确
         * 2.代码运行成功，结果错误
         * 3.代码运行失败
         * 
         * 此处只考虑运行是否成功，无需考虑结果正确与否
         * 
         * 参数：
         * cpu_limt  : 该程序运行可使用的最大运行时长（）
         * mem_limit : 该程序运行可使用的最大内存大小（KB）
         * 返回值：
         * > 0 运行异常，返回收到的信号
         * == 0 正常运行完毕
         * < 0 内部错误
         * **************************/
        static int Run(const std::string& file_name, const int cpu_limt, const int mem_limit) {
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            umask(0);
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);
            if(_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0) {
                LOG(ERROR) << "运行时，打开标准文件失败" << '\n';
                return -1; //打开对应标准文件失败
            }

            pid_t pid = fork();
            if(pid < 0) {
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                LOG(ERROR) << "运行时，创建子进程失败" << '\n';
                return -2; //创建运行子进程失败
            }
            else if(pid == 0) { // 子进程
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);

                SetProcLimit(cpu_limt, mem_limit);
                execl(_execute.c_str(), _execute.c_str(), nullptr);
                LOG(ERROR) << "运行时，启动运行失败" << '\n';
                _exit(1);
            }
            else { //父进程
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                waitpid(pid, &status, 0);
                
                LOG(INFO) << "status:" << status << '\n';
                LOG(INFO) << "运行完毕， info:" << WTERMSIG(status) << '\n';
                return WTERMSIG(status); // status & 0x7f  获取导致终止的信号编号，若正常则为0
            }
        }
    };
} // namespace ns_runner


