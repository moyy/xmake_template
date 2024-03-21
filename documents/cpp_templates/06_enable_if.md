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

为了让 上面代码 非const Person 对象 可以用构造函数，将构造函数改为；

``` cpp
// 如果 STR 能推导 string，则定义该函数
template<typename STR, typename = std::enable_if_t<std::is_convertible_v<STR, std::string>>>
Person(STR&& n) { ... }
```

