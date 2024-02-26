// std::forward 完美转发
// 
// 用于 函数模板
// 将 左值 传 左值
// 将 右值 传 右值
//
// 原理:
//    万能引用: 函数模板中, && 既可以接受 &, 也可以接受 &&
//    引用折叠:
//          - & & --> &
//          - & && --> &
//          - && & --> &
//          - && && --> &&

#include <utility>
#include <iostream>

void foo(int &t) {
    std::cout << "lvalue" << std::endl;
}

void foo(int &&t) {
    std::cout << "rvalue" << std::endl;
}

template<typename T>
void test_forward(T &&t) {
    
    std::cout << "------ foo(t): " << std::endl;
    foo(t);

    std::cout << "------ foo(std::forward<T>(t)): " << std::endl;
    foo(std::forward<T>(t));
    
    std::cout << "------ foo(std::move(t)): " << std::endl;
    foo(std::move(t));
}

int main() {
    test_forward(1); // lvalue rvalue rvalue
    
    int a = 1;
    test_forward(a); // lvalue lvalue rvalue
    test_forward(std::forward<int>(a)); // lvalue rvalue rvalue
    test_forward(std::forward<int&>(a)); // lvalue lvalue rvalue
    test_forward(std::forward<int&&>(a)); // lvalue rvalue rvalue
    
    return 0;
}