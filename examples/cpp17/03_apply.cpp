// std::apply 将 tuple 展开

#include <tuple>
#include <iostream>

int add(int p1, int p2) { return p1 + p2; }

int main() {
    // std::cout << add(std::pair(1, 2)) << std::endl; // error
    std::cout << std::apply(add, std::pair(1, 2)) << std::endl;   // 3
    
    auto add = [](auto p1, auto p2, auto p3) { return p1 + p2 + p3; };
    std::cout << std::apply(add, std::tuple(2.0f, 3.0f, 4.0f)) << std::endl; // 9

    return 0;
}