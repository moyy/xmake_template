// 折叠表达式
// 能 在模板中，方便 传递 变长参数

#include <string>
#include <iostream>

template <typename ... Ts>
auto sum(Ts ... ts) {
    return (ts + ...);
}

int main() {
    int s {sum(1, 2, 3, 4, 5)};
    std:: cout << s << std::endl; // 15

    std::string a{"hello "};
    std::string b{"world"};
    std::cout << sum(a, b) << std::endl; // hello world

    return 0;
}
