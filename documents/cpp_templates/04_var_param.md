# 《C++ template 2nd》04.可变参数模板

# 1. 例子

打印任意模板参数

点击 [这里](https://godbolt.org/z/zrYbWhnM7) 运行

``` cpp
#include <iostream>

namespace detail {
    void print() {}

    template<typename T, typename... Types>
    void print(T firstArg, Types... args)
    {
        std::cout << firstArg << " ";
        print(args...);
    }
}

template<typename... Types>
void print(Types... args) {
    std::cout << "print: ";

    detail::print(args...);
    
    std::cout << std::endl;
}

int main() {
    std::string s("world");
 
    print(7.5, "hello", s);
    return 0;
}
```

