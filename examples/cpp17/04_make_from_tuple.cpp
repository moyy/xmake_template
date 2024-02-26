#include <tuple>
#include <iostream>

struct Foo {
    Foo(int p1, float p2, int p3) {
        std::cout << p1 << ", " << p2 << ", " << p3 << std::endl;
    }
};

int main() {
   auto tuple = std::make_tuple(42, 3.14f, 0);
   
   // 将tuple 展开 作为 构造函数 参数
   auto foo = std::make_from_tuple<Foo>(std::move(tuple));
}