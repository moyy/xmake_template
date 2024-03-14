#! https://zhuanlan.zhihu.com/p/687064021
# 笔记：C++ 17 的 值类别 

来源：[C++17 的值类别](https://weakyon.com/2022/10/31/value-category-of-cpp17.html)

# 01. 如何区分 lvalue, rvalue, xvalue

注：到 [Godbolt](https://godbolt.org/z/WxehYGdEo) 运行代码

``` cpp
#include <iostream>
#include <type_traits>

// C++ 14 判断 表达式的 值类别, 看 decltype(expr)
//  + 返回 T&: 表达式 是 lvalue
//  + 返回 T&&: 表达式 是 xvalue
//  + 返回 T: 表达式 是 prvalue

#define PRINT_VALUE_CAT(e) \
    if constexpr (std::is_lvalue_reference<decltype((e))>::value) { \
        std::cout << "lvalue\n"; \
    } else if constexpr (std::is_rvalue_reference<decltype((e))>::value) { \
        std::cout << "xvalue\n"; \
    } else { \
        std::cout << "prvalue\n"; \
    }

struct S { int i; };

int main ()
{
    int&& r = 42;

    PRINT_VALUE_CAT(r); // lvalue，可以取地址

    PRINT_VALUE_CAT(4); // prvalue
    PRINT_VALUE_CAT(std::move(r)); // xvalue
    
    PRINT_VALUE_CAT(S{0}); // prvalue
    PRINT_VALUE_CAT(S{0}.i); // xvalue：注意，这个是 xvalue

    return 0;
}
```

# 02. C++ 17: 修改 prvalue 概念

+ RVO: 返回值优化；
+ NRVO: name-RVO 具名的返回值优化；

目的：让 RVO 可以 不依赖 移动构造函数

`C++ 17` 之后，就彻底分成：glvalue / prvalue 两大阵营

只有 xvalue 可以被移动; prvalue 不能被移动，不再是临时对象

+ prvalue (省略拷贝) 用于初始化的情况下，会直接在其最终目的地的存储空间中构建
+ 其他情况下，prvalue 隐式转换为 xvalue 进行移动

prvalue 可被理解为 能够产生临时对象的初始化器，但其本身并未成为临时对象。


注：到 [Godbolt](https://godbolt.org/z/r8oTxfaqG) 运行代码

``` cpp
struct NonMovable {
    int a_;

    NonMovable(int a): a_(a) {}

    // 禁止 如下函数
    NonMovable(const NonMovable&) = delete;
    NonMovable& operator=(const NonMovable&) = delete;

    NonMovable(NonMovable&&) noexcept = delete;
    NonMovable& operator=(NonMovable&&) noexcept = delete;
};

void foo(NonMovable v) { }

// 返回值优化
NonMovable rvo(int a) {
    return NonMovable {a};
}

// nrvo: 通不过编译，需要移动构造
// NonMovable nrvo(int a) {
//    auto v = NonMovable {a};
//    return v;
// }

int main() {
    // C++ 17: 只有 xvalue 可以被移动; prvalue 不能被移动，不再是临时对象
    // prvalue (省略拷贝) 用于初始化的情况下，会直接在其最终目的地的存储空间中构建
    // 其他情况下，prvalue 隐式转换为 xvalue 进行移动

    NonMovable v {4};

    // prvalue 用于初始化，等价于 NonMovable v2 {3};
    NonMovable v2 = NonMovable {3};

    // prvalue 用于初始化，等价于 foo(NonMovable v = {8});
    foo(NonMovable {8});

    // prvalue 用于初始化，等价于 auto r = NonMovable {9};
    auto r = rvo(9);
    
    // --------------------- 其他情况 1: NRVO，需要 移动构造
    // auto y = nrvo();
}
```