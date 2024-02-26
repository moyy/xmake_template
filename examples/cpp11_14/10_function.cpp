// + std::function 可调用对象 的 封装器
// 
// 可调用对象:
// 
//     + 函数指针
//     + 仿函数: 具有 operator() 成员函数 的 类对象
//     + lambda表达式
//     + 类 数据成员 / 成员函数
//     + std::bind 
// 
// std::function<void(int)> f; // f 参数 int，返回值 void
// 

#include <iostream>
#include <functional>

void print_num(int i) { std::cout << i << '\n'; }

// 仿函数
struct PrintNum {
    void operator()(int i) const { std::cout << i << '\n'; }
};

struct Foo {
    Foo(int num) : num_(num) {}
    
    void print_add(int i) const { std::cout << num_ + i << '\n'; }
    
    int num_;
};

int main() {
    // 函数
    std::function<void(int)> f_display = print_num;
    f_display(-9);

    // lambda
    std::function<void()> f_display_42 = []() { print_num(42); };
    f_display_42();

    // 函数对象 的调用
    std::function<void(int)> f_display_obj = PrintNum();
    f_display_obj(18);

    // std::bind 调用的结果
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();

    // 成员函数
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    const Foo foo(314159);
    f_add_display(foo, 1);
    f_add_display(314159, 1);

    // 成员函数: 绑定
    using std::placeholders::_1;
    std::function<void(int)> f_add_display3 = std::bind(&Foo::print_add, &foo, _1);
    f_add_display3(3);

    // 数据成员 
    // Foo const& 和 const Foo& 一样的意思
    std::function<int(Foo const&)> f_num = &Foo::num_;
    std::cout << "num_: " << f_num(foo) << '\n';
}