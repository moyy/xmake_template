
#include <iostream>

// 函数模板
// 注意：这里 蕴含着 对T的约束 T 可拷贝，可比较
// 注意：C++ 17 可拷贝 也可以 忽略 了
// 尽量不要用 class T
template <typename T>
T max(T a, T b) {
    return a < b ? b : a;
}

template<typename T = std::string>
T foo(T t = "abc") {
    return t;
}

template<typename RT, typename T1, typename T2>
RT max_rt(T1 a, T2 b) {
    return a < b ? b : a;
}

#include <type_traits>

// 类型推导：
//
// 1. 当 形参 是 & 时候，要 完全一样
// 2. 当 形参 是 值传递 时：忽略 CV，数组和函数 变 指针 处理
// 3. C++ 不会从上下文自动推导 返回值类型
// 
int main() {
    // :: 代表全局作用域，以便于 和 std::max 混淆
    auto a = ::max(7, 42);
    auto b = ::max(14, 3);
    
    // 42, 14
    std::cout << a << ", " << b << std::endl;

    // abc
    std::cout << foo() << std::endl;
    // 1
    std::cout << foo(1) << std::endl;

    // 42.0
    std::cout << max_rt<double>(7.0, 42) << std::endl;

    return 0;
}