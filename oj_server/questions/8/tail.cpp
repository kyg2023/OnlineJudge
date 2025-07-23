#ifndef COMPILE

#include "header.cpp"

#endif

void Test1() {
    int ret = Solution().Add(1, 2);
    if(ret == 3) {
        std::cout << "通过测试用例1" << std::endl;
    }
    else {
        std::cout << "未通过测试用例1：1, 2" << std::endl;
    }
}

void Test2() {
    int ret = Solution().Add(30, 20);
    if(ret == 50) {
        std::cout << "通过测试用例2" << std::endl;
    }
    else {
        std::cout << "未通过测试用例2：30, 20" << std::endl;
    }
}

int main() {
    Test1();
    Test2();

    return 0;
}