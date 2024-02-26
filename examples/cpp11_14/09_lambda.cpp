// 
// lambda 表达式
// 
// + auto func = [capture] (params) opt -> ret { func_body; };
// + 修改 捕获变量, opt 换成 mutable
// + [] 不捕获 任何变量
// + [&] 引用 捕获，捕获 外部作用域 所有变量，在函数体内 当作引用 使用
// + [=] 值 捕获，捕获 外部作用域 所有变量，在函数内 有个副本 使用
// + [=, &a] 按引用捕获a变量, 其他 值捕获
// + [a] 只 值捕获 a变量
// + [this] 捕获 当前类 的 this指针
//

#include <iostream>

int main() {

    auto func1 = [](int a) -> int { return a + 1; };
    // 简写
    auto func2 = [](auto a) { return a + 2; };
    std::cout << func1(1) << ", " << func2(2) << std::endl;

    // ========== 捕获
    int a = 0;
    auto f1 = [=](){ return a; }; // 值捕获a
    std::cout << f1() << std::endl; // 0

    // auto f2 = [=]() { return a++; }; // 编译不过
    auto f3 = [=]() mutable { return a++; };
    f3();
    std::cout << "a = " << a << std::endl; // a = 0
    
    auto f4 = [&]() mutable { return a++; };
    f4();
    std::cout << "a = " << a << std::endl; // a = 1
    
    return 0;
}