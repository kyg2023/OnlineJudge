#pragma once

#include <iostream>
#include <string>
#include <atomic>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#include <boost/algorithm/string.hpp>

namespace ns_util {
    const std::string temp_path = "./temp/";

    class TimeUtil {
    public:
        // 秒级别时间戳
        static std::string GetTimeStamp() {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return std::to_string(tv.tv_sec);
        }

        // 毫秒级别时间戳
        static std::string GetTimeStampMs() {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return std::to_string(tv.tv_sec * 1000 + tv.tv_usec / 1000);
        }
    };


    class PathUtil {
    public:
        static std::string AddSuffix(const std::string& filename, const std::string& suffix) {
            return filename+suffix;
        }


        // 构建 源文件路径+后缀 
        static std::string Src(const std::string& filename) {
            return temp_path + AddSuffix(filename, ".cpp");
        }
        // 构建 对应的可执行程序路径+后缀
        static std::string Exe(const std::string& filename) {
            return temp_path + AddSuffix(filename, ".exe");
        }
        // 构建 对应的运行错误文件路径+后缀
        static std::string CompileErr(const std::string& filename) {
            return temp_path + AddSuffix(filename, ".compile_err");
        }

        // 构建 对应的标准输入文件路径+后缀
        static std::string Stdin(const std::string& filename) {
            return temp_path + AddSuffix(filename, ".stdin");
        }
        // 构建 对应的标准输出文件路径+后缀
        static std::string Stdout(const std::string& filename) {
            return temp_path + AddSuffix(filename, ".stdout");
        }
        // 构建 对应的标准错误文件路径+后缀
        static std::string Stderr(const std::string& filename) {
            return temp_path + AddSuffix(filename, ".stderr");
        }
    };

    
    class FileUtil {
    public:
        static bool FileExists(const std::string& file_path) {
            struct stat st;
            if(stat(file_path.c_str(), &st) == 0) {
                return true;
            }
            return false;
        }

        // 单纯形成一个唯一的文件名称，无路径无后缀
        // 通过 毫秒级时间戳+原子性递增唯一值 来保证
        static std::string UniqFileName() {
            static std::atomic_uint id(0);
            std::string uniq_file_name = TimeUtil::GetTimeStampMs()+"_"+std::to_string(id++);
            return uniq_file_name;
        }

        static bool WriteFile(const std::string& target_file, const std::string& output) {
            std::ofstream fout(target_file);
            if(!fout.is_open()) 
                return false;
            fout.write(output.c_str(), output.size());
            fout.close();
            return true;
        }

        // target_file : 需要读取文件的路径
        // input       : 输出型参数
        static bool ReadFile(const std::string& target_file, std::string& input, const bool keep = true) {
            std::ifstream fin(target_file);
            if(!fin.is_open()) 
                return false;
            input.clear();
            std::string line;
            while(std::getline(fin, line)) {  // getline 不保留行分隔符
                input += line + (keep ? "\n" : "");
            }

            fin.close();
            return true;
        }
    };

    class StringUtil {
    public:
        // str : "1 判断回文数 简单 1 30000"
        // out : 输出型参数
        // sep : 分割符
        static void SpiltString(const std::string& str, std::vector<std::string>& out, const std::string& sep = " ") {
            boost::split(out, str, boost::is_any_of(sep), boost::token_compress_on);
        }
    };
    
}