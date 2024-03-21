#! https://zhuanlan.zhihu.com/p/688179504
# 《C++ template 2nd》04.可变参数模板

# 1. 例子

打印任意模板参数：

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

# 2. 重载模板函数

+ C++ 17: 若两个函数模板的区别，仅在于末尾参数包的不同，则首选没有末尾参数包的函数模板

点击 [这里](https://godbolt.org/z/qvE43M19K) 运行

``` cpp
#include <string>
#include <iostream>

template<typename T>
void print (T arg)
{
    std::cout << arg << ' ';
}

template<typename T, typename... Types>
void print (T firstArg, Types... args)
{
    print(firstArg);
    
    // 当 args 只有一个元素时，调用 print(T arg)
    print(args...); 
}

int main() {
    std::string s("world");
    print(7.5, "hello", s);
    std::cout << std::endl;    
    return 0;
}
```

# 3. sizeof... 可变参数的数量

点击 [这里](https://godbolt.org/z/rx1Pc3hEY) 运行

``` cpp
#include <string>
#include <iostream>

// 必须要有，因为下面的 sizeof... 编译时实例化
// 如果没有这个，那么 args个数为0时候print() 无法通过编译；
void print() {}

template<typename T, typename... Types>
void print (T firstArg, Types... args) {
    std::cout << firstArg << ' ';
    if (sizeof...(args) > 0) {
        print(args...);
    }
}

int main() {
    std::string s("world");
    print(7.5, "hello", s);
    std::cout << std::endl;
    
    return 0;
}
```

# 4. C++ 17：折叠表达式

+ 几乎所有的二元运算符，都可以 用 折叠表达式

|表示|展开|
|-|-|
|( ... + s )|((s1 + s2) + s3)...|
|( ... + s )|((s1 + s2) + s3)...|
|(0 + ... + s)|((0 + s1) + s2)...|
|(s + ... + 0)|( s1 + ( ... ( sn + 0 ) ... ))|

点击 [这里](https://godbolt.org/z/6fqf7sMr9) 运行

``` cpp
#include <cassert>

template<typename... T>
auto foldSum (T... s) {
    // return (... + s);
    // return (... + s);
    return (0 + ... + s);
    // return (s + ... + 0);
}

int main() {
    auto s = foldSum(1, 2, 3, 4, 5);
    assert(s == 15);
    return 0;
}
```