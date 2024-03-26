#! https://zhuanlan.zhihu.com/p/689081187
# 《C++ template 2nd》07. 传值还是传引用

这章最好连着[ Effective Modern C++ Item 41](https://cntransgroup.github.io/EffectiveModernCppChinese/8.Tweaks/item41.html) 一起看。

性能：在必须拷贝的场合，值传递 比 引用传递，多了 一个 移动构造 的开销；

# 01. 值传递

+ 通常参数是值传递的函数，在调用处，如果可以，尽可能调用移动语义，减少拷贝成本
+ `衰变` (`Decay`)：实参传递，模板推导的时候，会将 数组/函数 变 指针，删除 cv限定符（const / volatile）

点击 [这里](https://godbolt.org/z/qd9Yns6To) 运行

``` cpp
#include <string>
#include <type_traits>

template <typename T>
void foo (T arg) { 
    static_assert(std::is_same_v<T, std::string>);
}

template <typename T>
void foo2 (T arg) { 
     static_assert(std::is_same_v<T, char const*>);
}

template <typename T>
void foo3 (T arg) { 
     static_assert(std::is_same_v<T, int*>);
}

int main() {
    std::string const c = "hi";
    foo(c); // T = std::string，衰变去掉 const

    auto &p = "hi"; // char const &p[3] = "hi";
    static_assert(std::is_same_v<decltype(p), char const(&)[3]>);
    foo2(p); // T = char const*, 不是字符数组，注意 const 是修饰 char，不是指针，所以const保留

    foo2("hi"); // T = char const*, 不是字符数组，注意 const 是修饰 char，不是指针，所以const保留

    int arr[4];
    foo3(arr); // T = int*, 衰变，数组变指针
}
```

# 02. 常量引用

``` cpp
template <typename T>
void printR(T const& arg) {

}
```

+ 可以传 左值 / 右值
+ 实参传递时，不会 衰变 ( 函数 / 数组 / const / volatile)
+ 底层实现: 通过地址传递；
    - 编译器不会极致优化，因为可以通过 const_cast 去掉 const; 原变量还可以在别的地方修改；
    - rust的编译器可以做极致优化，因为所有权机制；
    - 内联代码让编译器看到全局，可以做极致优化；

# 03. 非常量引用

``` cpp
template <typename T>
void outR(T & arg) {

}

+ 引用：不会衰变
+ 不能传 右值，比如 std::move() 或 临时对象
+ 可以传 const 右值，会 统一变成 const 左值

虽然 可以 传 const，但是 模板代码修改的地方就一定会错误。如果想一开始就明确禁止 printR 传 const，方法如下：

静态断言：

``` cpp
template <typename T>
void outR (T& arg) {
    static_assert(!std::is_const_v<T>, "out parameter of foo<T>(T&) is const");

    // ...
}
```

C++ 20 Concepts:

``` cpp
template <typename T>
requires !std::is_const_v<T>
void outR (T& arg) {
    // ...
}
```

C++ 14: enable_if_t

``` cpp
template <typename T, typename = std::enable_if_t<!std::is_const_v<T>>
void outR (T& arg) {
    // ...
}
```

# 04. 转发引用

``` cpp
template <typename T>
void passR (T&& arg) {
    // ...
}
```

+ 只有 T 是 模板参数时候，T&&才是转发引用；
  - passR(typename T::value_type&& arg) 这个是右值引用；
+ 引用：不会衰变
+ 可以接 左值 / 右值
+ 值传递 变 左值引用

问题：函数内部定义 局部变量 时候，要小心

点击 [这里](https://godbolt.org/z/c7r1cnKzb) 查看编译错误

``` cpp
template<typename T>
void passR(T&& arg) { // arg is a forwarding reference
    T x; // 局部变量
}

int main() {
    passR(42); // T = int, 函数的 T x 就是 int x;
    
    int i;
    passR(i); // 编译错误，T = int&, int &x; 没有初始化；
}
```

解决办法：用 std::remove_reference_t 配合 转发引用一起定义局部变量

点击 [这里](https://godbolt.org/z/aGTq9Y3e3) 运行

``` cpp
#include <type_traits>

template <typename T> 
void passR(T&&) {
    std::remove_reference_t<T> x {};
}

int main() {
    passR(42); // T = int; int x;
    
    int i;
    passR(i); // T = int&; int x;
}
```

# 05. 实参：std::ref() / std::cref()

就算参数是值传递，在调用函数时一样可以通过 ref / cref 传递 引用 / 常量引用；

点击 [这里](https://godbolt.org/z/aGTq9Y3e3) 运行

``` cpp
#include <string>
#include <type_traits>

template<typename T>
void printT (T arg) {
    static_assert(std::is_same_v<T, std::string>);
}

template<typename T>
void printT2 (T arg) {
    static_assert(std::is_same_v<T, std::reference_wrapper<std::string const>>);
}

int main() {
    std::string s = "hello";
    printT(s); // T = std::string

    printT2(std::cref(s)); // T =  std::reference_wrapper<string const> 类似 const std::string&
}
```

# 06. 数组 / 字符串字面量

如果想要显式衰变，可以用 `std::decay<T>()`

有时候，想让函数只对数组有效

方法1：修改模板参数

``` cpp
template <typename T, std::size_t L1, std::size_t L2>
void foo(T (&arg1)[L1], T (&arg2)[L2])
{
    T* pa = arg1; // decay arg1
    T* pb = arg2; // decay arg2
    if (compareArrays(pa, L1, pb, L2)) {
        // ...
    }
}
```

方法2：用 enable_if_t / concepts

``` cpp
#include <type_traits>

template < typename T, typename = std::enable_if_t<std::is_array_v<T>> >
void foo (T&& arg1, T&& arg2) {
    // ...
}
```


# 07. 返回值

啥时候返回引用：

+ 返回 容器元素 (例如，通过 operator[] 或 front())
+ 授予 类成员 读 / 写 访问权限
+ 链式调用 的 对象，例子：输入输出运算符，赋值运算符）

问题：如果握着引用的时候，将源对象delete掉了，就会出现野引用；

``` cpp
auto s = std::make_shared<std::string>("whatever");
auto& c = (*s)[0];
s.reset();
std::cout << c;
```

下面说下模板返回的问题：

点击 [这里](https://godbolt.org/z/Pe9zc3sPs) 运行

``` cpp
#include <type_traits>

template<typename T>
T retV(T&& p) 
{
    return p; // T 有可能是引用 
}

int main() {
    int x;
    static_assert(std::is_same_v<int&, decltype(retV<int&>(x))>);
}
```

如果一定要返回值，那么：

可以用 std::decay / std::remove_reference_t

点击 [这里](https://godbolt.org/z/MEdTsMGY7) 运行

``` cpp#include <type_traits>

template<typename T>
typename std::remove_reference_t<T> retV(T&& p) 
{
    return p;
}

int main() {
    int x;
    static_assert(std::is_same_v<int, decltype(retV<int&>(x))>);
}
```

可以直接 返回 auto，因为auto总是衰变

点击 [这里](https://godbolt.org/z/68rYhjW7G) 运行

``` cpp
#include <type_traits>

template<typename T>
auto retV(T&& p) 
{
    return p;
}

int main() {
    int x;
    static_assert(std::is_same_v<int, decltype(retV<int&>(x))>);
}
```

# 08. 结论

+ 值传递
    - 简单，衰变，不是最佳性能
    - 调用方 必要时 可用 std::cref() / std::ref() 传引用
+ 引用传递：转发引用
    - 最好性能
    - 左值 / 右值
    - 不会衰变：注意 字符串字面量的问题；

最后我们看看一个例子，c++ 11 的 make_pair 简化版：

注意：转发引用 && 完美转发

``` cpp
template <typename T1, typename T2>
constexpr pair<typename std::decay_t<T1>, typename std::decay_t<T2>>
make_pair (T1&& a, T2&& b) {
    return pair<
        typename std::decay_t<T1>,
        typename std::decay_t<T2>
    >(
        std::forward<T1>(a), 
        std::forward<T2>(b)
    );
}
```