#include <vector>
#include <iostream>

int main() {
    std::vector<int> vec {1, 2, 3, 4, 5};

    // C++ 17 将 临时变量 放到 if / switch 内
    if (const auto itr = std::find(vec.begin(), vec.end(), 3);
        itr != vec.end()) {
        *itr = 40;
    }
 
    // 40
    std::cout << vec[2] << std::endl;
}