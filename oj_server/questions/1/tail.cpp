#ifndef COMPILE

#include "header.cpp"

#endif

void Test1() {
    bool ret = Solution().isPalindrome(121);
    if(ret) {
        std::cout << "通过测试用例1" << std::endl;
    }
    else {
        std::cout << "未通过测试用例1：121" << std::endl;
    }
}

void Test2() {
    bool ret = Solution().isPalindrome(20);
    if(!ret) {
        std::cout << "通过测试用例2" << std::endl;
    }
    else {
        std::cout << "未通过测试用例2：20" << std::endl;
    }
}

int main() {
    Test1();
    Test2();

    return 0;
}