#include <tuple>
#include <iostream>

int main() {
    
    auto t = std::make_tuple(1, 2.6, "hello");

    auto size = std::tuple_size<decltype(t)>::value; // 3

    auto e0 = std::get<0>(t);
    auto e1 = std::get<1>(t);
    auto e2 = std::get<2>(t);

    std::cout << "size = " << size << ", [" << e0 << ", " << e1 << ", " << e2 << "]" << std::endl;  // size = 3, [1, 2.6, hello]

    auto [a0, a1, a2] = t; // C++ 17 解构赋值
    std::cout << a0 << " " << a1 << " " << a2 << std::endl; // 输出: 1 2.6 hello

    auto t2 = std::make_tuple(1, 3.2, "goodbye");

    auto is_same = t == t2; // false
    auto is_less = t < t2; // true
    std::cout << std::boolalpha << is_same << " " << is_less << std::endl; // false true
    return 0;
}