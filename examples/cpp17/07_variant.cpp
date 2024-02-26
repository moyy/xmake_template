#include <variant> // C++ 17
#include <iostream>
#include <string>

// std::variant 类似 union 功能，比union更高级
// union 不能有 string 这种类型，但std::variant 可以
// 还可以支持更多复杂类型，如map等
// 

struct A {
    A(int i) { }  
};

void test() {
    // 注：一般情况下variant的第一个类型一般要有对应的构造函数，否则编译失败：
    
    // std::variant<A, int> var; // 编译失败
    
    // 用 std::monostate 打个桩，模拟一个空状态
    std::variant<std::monostate, A, int> var;
}

int main() { 
    test();
    
    std::variant<int, std::string> var("hello");
    std::cout << var.index() << std::endl; // 1
    
    var = 123;
    std::cout << var.index() << std::endl; // 0

    try {
        var = "world";
        std::string str = std::get<std::string>(var); // 通过类型获取值
        var = 3;
        
        int i = std::get<0>(var); // 通过index获取对应值
        std::cout << str << std::endl; // world
        std::cout << i << std::endl;   // 3
    } catch(...) {
        // xxx;
    }
 
    return 0;
}