#include <iostream>

template <typename T>
T foo(T a) {
    // gcc 13.2 中，即使没有实例化模板，也会编译报错；
    t = a
    
    return t;
}

int main() {
    // std::cout << foo(0) << std::endl;
    return 0;
}