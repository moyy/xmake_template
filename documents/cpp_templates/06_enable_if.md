#! https://zhuanlan.zhihu.com/p/688435379
# 《C++ template 2nd》06. 移动语义 与 enable_if

# 01. 完美转发

通过模板转发参数时，想:

+ 非const对象 保持 可修改转发
+ const对象 保持 只读转发
+ 可移动对象（右值引用）保持 可移动转发

简单来说：转发时，保持 左值/右值/常量 属性

``` cpp
// T&& 在C++ 17 中 被称为 转发引用，描述了用来干什么
// 注意：T&& 的 T 必须是模板参数的名称。仅依赖模板参数是不够的。
// 例子：typename T::iterator&& 只是一个右值引用，而不是转发引用
template <typename T>
void f (T&& val) {
    g(std::forward<T>(val)); // perfect forward val to g()
}
```

# 02. `std::enable_if<Condition, Type>`

在C++ 20 后，被 Concepts 代替

标准库 提供 std::enable_if，以便在不满足条件时 忽略 函数模板。

+ `std::enable_if_t<Condition>` Condition 为 true，返回 void;
+ `std::enable_if_t<Condition, Type>` Condition true，返回 Type;
+ 如果 Condition 为false，则忽略该函数的定义

``` cpp
// 如 sizeof(T) > 4 为 true, 生成函数 void foo() {};
// 否则，没有 foo函数 的 定义
template<typename T>
typename std::enable_if_t<(sizeof(T) > 4)> foo() { }
```

觉得不清晰，可以：

``` cpp
template <typename T>
using EnableIfSizeGreater4 = std::enable_if_t<(sizeof(T) > 4)>;

template <typename T, typename = EnableIfSizeGreater4<T>>
void foo() { }
```

# 03. enable_if 应用

我们先看看下面的 Person 类

点击 [这里](https://godbolt.org/z/Mo816s77n) 运行

``` cpp
#include <utility>
#include <string>
#include <iostream>

class Person {
private:
    std::string name;
public:
    // 模板构造
    template<typename STR>
    explicit Person(STR&& n) : name(std::forward<STR>(n)) {
        std::cout << "Person: " << name << "\n";
    }
    
    // 拷贝构造
    Person (Person const& p) : name(p.name) {
        std::cout << "Person const&: " << name << "\n";
    }

    // 移动构造
    Person (Person&& p) : name(std::move(p.name)) {
        std::cout << "Person &&: " << name << "\n";
    }
};

int main() {
    std::string s = "sname";
    
    // Person:sname
    Person p1(s); 
    
    // Person: tmp
    Person p2("tmp"); 
    
    // 编译错误：
    // 对 非常来说，Person(STR&& n) 比const Person & 更优先
    Person p3(p1); // ERROR

    // Person &&: sname
    Person p4(std::move(p1));
    
    // Person: ctmp
    Person const p2c("ctmp"); 
    // Person cpnst&: ctmp
    Person p3c(p2c);
}
```

为了让 上面代码 非const Person 对象 可以用 拷贝构造函数，将构造函数改为；

``` cpp
template<typename T>
using EnableIfString = std::enable_if_t<
    std::is_convertible_v<T,std::string>
>;
```

也可以使用 std::is_constructible，允许显式转换用于初始化，但参数顺序和 std::is_convertible 相反。

``` cpp
template<typename T>
using EnableIfString = std::enable_if_t<
    std::is_constructible_v<std::string, T>
>;
```

模板构造函数 如下：

``` cpp
// 如果 STR 能推导 string，则定义该函数
template <typename STR, typename = EnableIfString<STR>>
explicit Person(STR&& n): name(std::forward<STR>(n)) { }
```

完整代码见：[这里](https://godbolt.org/z/sjMr531xP) 运行

# 04. 禁用 默认拷贝构造

有模板构造函数，编译器还是会生成默认的拷贝构造；而且优先调用const拷贝构造

具体例子：下面代码，不会打印任何东西；

完整代码见：[这里](https://godbolt.org/z/vYaMq1jMz) 运行

``` cpp
#include <iostream>

class C {
public:
    C() = default;
    
    // 这么定义，编译器还是会生成 默认的拷贝构造函数
    template <typename T>
    C (T const&) {
        std::cout << "tmpl copy constructor\n";
    }
};

int main() {
    C a;
    
    // 调用 默认的拷贝构造函数
    C b { a };
}
```

修改，加上 = delete 禁用 const volatile & 的 拷贝构造即可/

完整代码见：[这里](https://godbolt.org/z/nd7E3b3Md) 运行

``` cpp
#include <iostream>

class C {
public:
    C() = default;

    C (C const volatile &) = delete;

    template <typename T>
    C (T const&) {
        std::cout << "tmpl copy constructor\n";
    }
};

int main() {
    C a;
    
    // 调用 默认的拷贝构造函数
    C b { a };
}
```

# 05. C++ 20: Concepts

下面使用 Concepts 重写 03. Person 的 Demo

首先是头文件 

``` cpp
#include <concepts>
```

然后是定义 StringConvertible 概念

``` cpp
// StringConvertible: 检查类型是否可以转换为 std::string
template<typename STR>
concept StringConvertible = requires(STR s) {
    std::string{std::forward<STR>(s)};
};
```

最后是 模板构造函数使用 约束：

``` cpp
// 模板参数 STR 必须满足 StringConvertible 概念
template<StringConvertible STR>
explicit Person(STR&& n) : name(std::forward<STR>(n)) {
    std::cout << "Person: " << name << "\n";
}
```

完整代码 点击 [这里](https://godbolt.org/z/33dThdYbW) 运行

``` cpp
#include <concepts>
#include <string>
#include <iostream>
#include <utility>

// 检查类型是否可以转换为 std::string
template<typename STR>
concept StringConvertible = requires(STR s) {
    std::string{std::forward<STR>(s)};
};

class Person {
private:
    std::string name;
public:
    // 使用概念约束模板构造函数
    template<StringConvertible STR>
    explicit Person(STR&& n) : name(std::forward<STR>(n)) {
        std::cout << "Person: " << name << "\n";
    }
    
    // 拷贝构造
    Person(const Person& p) : name(p.name) {
        std::cout << "Person const&: " << name << "\n";
    }

    // 移动构造
    Person(Person&& p) noexcept : name(std::move(p.name)) {
        std::cout << "Person &&: " << name << "\n";
    }
};

int main() {
    std::string s = "sname";
    
    // Person: sname
    Person p1(s);
    
    // Person: tmp
    Person p2("tmp");
    
    // 拷贝构造
    Person p3(p1); // 现在这里不会出错了

    // Person &&: sname
    Person p4(std::move(p1));
    
    // Person: ctmp
    const Person p2c("ctmp");
    // Person const&: ctmp
    Person p3c(p2c);
}

```