// 传递 string时 会触发 拷贝，很影响运行效率
// 有了string_view就可以避免拷贝操作，平时传递过程中传递string_view即可。

#include <string>
#include <string_view>

#include <iostream>

void func(std::string_view stv) { 
    std::cout << stv << std::endl; 
}

int main(void) {
    std::string str = "Hello World";
    std::cout << str << std::endl;

    std::string_view stv(str.c_str(), str.size());
    std::cout << stv << std::endl;
    func(stv);
    
    return 0;
}