#pragma once


#include <iostream>
#include <string>

#include "util.hpp"

namespace ns_log
{
    using namespace ns_util;

    enum {
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    inline std::ostream& Log(const std::string& level, const std::string& file_name, const int line) {
        // 1.添加日志等级
        std::string message = "[";
        message += level;
        message += "]";

        // 2.添加报错文件名
        message += "[";
        message += file_name;
        message += "]";

        // 3.添加报错行
        message += "[";
        message += std::to_string(line);
        message += "]";

        // 4.添加日志时间戳
        message += "[";
        message += TimeUtil::GetTimeStamp();
        message += "]";

        std::cout << message;

        return std::cout; //开放式日志
    }

    // LOG(level) << other_message << "\n";
    #define LOG(level) Log(#level, __FILE__, __LINE__)

}
