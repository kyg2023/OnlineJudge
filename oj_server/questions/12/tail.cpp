#ifndef COMPILE

#include "header.cpp"

#endif

void Test1() {
    vector<int> arr = {1, 2, 3, 7, 5};
    int ret = Solution().Max(arr);
    if(ret == 7) {
        std::cout << "通过测试用例1" << std::endl;
    }
    else {
        std::cout << "未通过测试用例1：{1, 2, 3, 7, 5}" << std::endl;
    }
}

void Test2() {
    vector<int> arr = {1, -1, 3, -7, 5};
    int ret = Solution().Max(arr);
    if(ret == 5) {
        std::cout << "通过测试用例2" << std::endl;
    }
    else {
        std::cout << "未通过测试用例2：{1, -1, 3, -7, 5}" << std::endl;
    }
}

int main() {
    Test1();
    Test2();

    return 0;
}