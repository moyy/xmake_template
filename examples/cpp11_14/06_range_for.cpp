// 基于范围的for循环
// 
// for (auto &elem: v) { }

#include <vector>
#include <iostream>

int main() {
    std::vector<int> vec;

    // before c++11
    for (auto iter = vec.begin(); iter != vec.end(); iter++) { 
        auto elem = *iter;
    }

    // c++11基于范围的for循环
    for (auto &elem: vec) {
    }

    return 0;
}