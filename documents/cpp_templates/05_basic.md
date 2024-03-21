#! https://zhuanlan.zhihu.com/p/688179287
# 《C++ template 2nd》05. 基础技巧

# 01. typename

typename 用在模板内，表示其后的标识符是类型，而不是一个值。

``` cpp
template <typename T>
class MyClass {
public:
    void foo() {
        // 表示 T::SubType 是类型，而非一个值
        typename T::SubType* ptr;
    }
};
```

# 02. 零值初始化

用 统一初始化，可以赋初值为0；

``` cpp
template <typename T>
class MyClass {
public:
private:
    T x {}; // 值初始化，即使是内置类型也可以赋0值
};
```

或者 构造函数

``` cpp
template <typename T>
class MyClass {
public:
    MyClass() : x{} {} // 值初始化，即使是内置类型也可以赋0值
private:
    T x;
};
```

函数的 默认参数 必须写 =

``` cpp
template <typename T>
void foo(T p = T{}) {
}
```

# 03. this->

对于基类模板，如果成员 x 被继承，则在子类中要 使用 this->x 访问，而不是 x，否则会编译错误

点击 [这里](https://godbolt.org/z/E1xjEfThh) 查看编译错误

``` cpp
template <typename T>
class Base {
public:
    void bar();
};

template <typename T>
class Derived : Base<T> {
public:
    void foo() {
        bar(); // 调用 外部作用域的 bar()，找不到就报错
        // this->bar();
    }
};
```

# 04. 数组 & 字符串字面量

+ 模板声明为引用时，参数不会衰变："hello" 为 char const[6]，期望是不同长度数组或字符串字面量时，要小心那个坑 
+ 按值传递参数时，类型才会衰变："hello" 为 char *

点击 [这里](https://godbolt.org/z/znxbTGGGc) 运行

``` cpp
template<typename T, int N, int M>
bool less (T(&a)[N], T(&b)[M])
{
    auto min = N < M ? N : M;
    for (int i = 0; i < min; ++i) {
        if (a[i] < b[i]) return true;
        if (b[i] < a[i]) return false;
    }
    return N < M;
}

int main() {
    int x[] = {1, 2, 3};
    int y[] = {1, 2, 3, 4, 5};

    // T = int, N = 3, M = 5
    less(x, y);
}
```

对数组所有 重载/偏特化 情况 如下：

+ `struct MyClass<T[SZ]>` 大小为 SZ 的 数组
+ `struct MyClass<T(&)[SZ]>` 大小为 SZ 的 数组 的 引用
+ `struct MyClass<T[]>` 不知长度的数组
+ `struct MyClass<T(&)[]>` 不知长度的数组引用
+ `struct MyClass<T*>` 指针

对于这种：extern int i[]; 当通过引用传递时，就变成了一个 int(&)[]

# 05. 成员模板

+ 类的成员函数 可以是 模板函数
+ 例子：模板赋值运算符，模板化构造函数
    - 实现模板赋值，可以将 `Stack<int>` 赋值给 `Stack<float>`
    - 检查类型兼容很困难，只能看编译器报错信息
    - 不同类型的模板类能看到彼此的私有数据，只能用友元 `template<typename> friend class Stack`

# 06. 变量模板

C++ 14 之后，变量也可以参数化

``` cpp
// 使用时，pi<float> 等
template <typename T>
constexpr T pi {3.1415926535897932385}
```

变量模板可以使用默认参数：

``` cpp
// 使用 默认 参数：pi<>
template <typename T = long double>
constexpr T pi = T {3.1415926535897932385};
```

非类型模板参数：

``` cpp
#include <array>

// N个元素的数组，进行了 值初始化
template <int N>
std::array<int, N> arr {}; 

// 通过 模板值 来确定 dval的类型
// 使用时：dval<'c'>
template <auto N>
constexpr decltype(N) dval = N; 
```