#include<any>
#include<iostream>

int main() { // c++17可编译
    std::any a = 1;
    // int 1
    std::cout << a.type().name() << " " << std::any_cast<int>(a) << std::endl;
    
    a = 2.2f;
    // float 2.2
    std::cout << a.type().name() << " " << std::any_cast<float>(a) << std::endl;
    
    if (a.has_value()) {
        // float
        std::cout << a.type().name() << std::endl;
    }

    a.reset();
    if (!a.has_value()) {
        // after reset, none value
        std::cout << "after reset, none value" << std::endl;;
    }

    a = std::string("a");
    // class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > a
    std::cout << a.type().name() << " " << std::any_cast<std::string>(a) << std::endl;

    return 0;
}