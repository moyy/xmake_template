// 自定义 字面量

#include <iostream>

struct Time {
    unsigned long long valueInMS; // 毫秒
};

// 自定义字面量
// 必须是 char类型 或 unsigned long long
constexpr Time operator"" s ( unsigned long long s ) {
    
    auto valueInMS = s * 1000;

    return Time { valueInMS };
}

constexpr Time operator"" ms ( unsigned long long ms ) {
    
    auto valueInMS = ms;

    return Time { valueInMS };
}

int main() {
    Time t = 3s;
    std::cout << "3s is " << t.valueInMS << "ms" << std::endl;

    t = 6ms;
    std::cout << "6ms is " << t.valueInMS << "ms" << std::endl;
}
