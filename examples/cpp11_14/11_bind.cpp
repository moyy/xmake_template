// std::bind 将 可调用对象 和 参数, 其结果 用 std::function 保存
// std::cref 常量引用
// std::ref  引用
//    当你想在 不支持引用传递 的 上下文中（如std::thread的构造函数）按 引用传递 对象 时。
//    std::bind 使用 时，确保 绑定的参数 是按引用传递，而不是被拷贝。

#include <functional>
#include <iostream>
#include <memory>

void f(int n1, int n2, int n3, const int& n4, int n5) {
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << std::endl;
}

int g(int n1) { return 3 * n1; }

struct Foo {
    void print_sum(int n1, int n2) { std::cout << n1 + n2 << std::endl; }
    int data = 10;
};

int main() {
    using namespace std::placeholders;  // 针对 _1, _2, _3...

    // 演示参数重排序和按引用传递
    int n = 7;
    // （ _1 与 _2 来自 std::placeholders ，并表示将来会传递给 f1 的参数）
    auto f1 = std::bind(f, _2, 42, _1, std::cref(n), n);
    
    n = 10;
    // f(20, 42, 100, 10, 7)
    f1(100, 20, 1001);  
    
    // 嵌套 bind 子表达式共享占位符
    auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
    // f(12, g(12), 12, 4, 5)
    f2(10, 11, 12); 

    // 成员函数
    Foo foo;
    auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
    // foo.print_num(95, 5);
    f3(5);

    // 数据成员
    auto f4 = std::bind(&Foo::data, _1);
    // _.data()
    std::cout << f4(foo) << std::endl;
}