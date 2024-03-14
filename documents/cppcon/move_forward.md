#! https://zhuanlan.zhihu.com/p/686980663
# std::move 和 std::forward 实现细节

# 01. `std::remove_reference_t<T>`

+ 如果 T是 A& 或 A&&，则返回 A
+ 否则返回 T

注：到 [Godbolt](https://godbolt.org/z/9rd4fsWh3) 运行代码

``` cpp
#include <type_traits>

int main() {
    static_assert(std::is_same_v<int, std::remove_reference_t<int>>);
    
    static_assert(std::is_same_v<int, std::remove_reference_t<int&>>);
    
    static_assert(std::is_same_v<int, std::remove_reference_t<int&&>>);
    
    return 0;
}
```

# 02. `std::is_lvalue_reference_v<T>`

是否左值引用: 

+ 如果 T是 A&，则返回 true
+ 否则，返回 false （包括 A && 或 A）

注：到 [Godbolt](https://godbolt.org/z/eaejYE3Pj) 运行代码

``` cpp
#include <type_traits>

int main() {
    static_assert(!std::is_lvalue_reference_v<int>);
    
    static_assert(std::is_lvalue_reference_v<int&>);

    static_assert(!std::is_lvalue_reference_v<int&&>);
    
    return 0;
}
```

# 03. std::move

std::move(t) 将变量 t 变成 右值引用 T&&，不管 t是 T，T& 还是 T&&

注：到 [Godbolt](https://godbolt.org/z/eo7G7GTs9) 运行代码

```cpp
#include <utility>

int main() {

    static_assert(std::is_same_v<int, decltype(4)>);

    int a = 4;
    static_assert(std::is_same_v<int&&, decltype(std::move(a))>);

    int& b = a;
    static_assert(std::is_same_v<int&&, decltype(std::move(b))>);

    int&& c = std::move(a);
    static_assert(std::is_same_v<int&&, decltype(std::move(c))>);
    
    return 0;
}
```

# 04. std::mvoe 实现 my_move

## 04.1. 万能引用（Universal References）

C++ 11 引入的语言特性；

当函数模板的参数类型是 T&& 时，该类型 可以接受 左值引用 或 右值引用；

万能引用可以根据传入参数的类型自动地变成左值引用或右值引用，这是通过类型推导加引用折叠规则实现的。

## 04.2. 引用折叠（Reference Collapsing）

C++ 11 引入的语言特性；

+ T& &, T& &&, T&& & --> T&
+ T&& && --> T&&

## 04.3. my_move 实现

下面代码的要点是：

+ my_move的参数声明 T&& t, 是万有引用，可以接受 左值引用 或 右值引用；

注：到 [Godbolt](https://godbolt.org/z/1PxbYxfY5) 运行代码

``` cpp
#include <type_traits>

template <class T>
constexpr std::remove_reference<T>&& my_move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}
```

## 04.4. 传递左值引用

下面我们加上一些类型断言，以便于更清楚的知道整个推导过程：

注：到 [Godbolt](https://godbolt.org/z/hGKjeh8r8) 运行代码

``` cpp
#include <type_traits>

// 当调用 my_move(a)时，a是 int&，左值引用；
template <class T>
constexpr std::remove_reference_t<T>&& my_move(T&& t) noexcept {
    
    // T = int&
    static_assert(std::is_same_v<T, int&>);

    // 引用折叠，t的类型也是 int&
    static_assert(std::is_same_v<decltype(t), int&>);
    
    // std::remove_reference_t<T> 就是 int
    static_assert(std::is_same_v<std::remove_reference_t<T>, int>);

    // 最后返回 int&&
    return static_cast<std::remove_reference_t<T>&&>(t);
}

void foo(int && a) { }

int main() {
    int a = 4;
    
    // 注：这里的 move(a) 的 a 就是 左值引用 int&
    // 证明返回值类型就是 int&&
    static_assert(std::is_same_v<int&&, decltype(my_move(a))>);

    // 调用函数成功
    foo(my_move(a));
}
```

## 04.5. 传递右值引用

注：这里有个思维误区，即使 是 int &&a = 42; 当你调用 my_move(a) 时，T = int& 也是左值引用；因为 表达式 a 可以取地址！

要构造右值引用的参数，我这里用了 std::move(a)，以便于我代码里面的 static_assert 不至于判断多个类型。

下面我们加上一些类型断言，以便于更清楚的知道整个推导过程：

注：到 [Godbolt](https://godbolt.org/z/M48zW65j5) 运行代码

``` cpp
#include <utility>
#include <type_traits>

template <class T>
constexpr std::remove_reference_t<T>&& my_move(T&& t) noexcept {
    
    // 引用折叠 后，T && && 就是 T&&，T = int
    static_assert(std::is_same_v<T, int>);

    // 引用折叠，t的类型是 int&&
    static_assert(std::is_same_v<decltype(t), int&&>);
    
    // T = int, 所以 std::remove_reference_t<T> 就是 int
    static_assert(std::is_same_v<std::remove_reference_t<T>, int>);

    // 最后返回 int&&
    return static_cast<std::remove_reference_t<T>&&>(t);
}

void foo(int &&a) { }

int main() {
    int a = 4;
    
    // 证明返回值类型就是 int&&
    static_assert(std::is_same_v<int&&, decltype(my_move(std::move(a)))>);

    // 调用函数成功
    foo(my_move(std::move(a)));
}
```

# 05. std::forward

## 05.1. 作用

如果 没有 std::forward，无法转发右值，因为右值变量的使用也被看成左值.

注：到 [Godbolt](https://godbolt.org/z/vzax7j4P6) 运行代码

``` cpp
#include <utility>
#include <type_traits>
#include <iostream>

void foo(int &a) {
    std::cout << "foo(int &)" << std::endl;
}

void foo(int &&a) { 
    std::cout << "foo(int &&)" << std::endl;
}

template <typename T>
void f(T && a) {

    static_assert(std::is_same_v<int, T>);
    static_assert(std::is_same_v<int&&, decltype(a)>);
    
    // 虽然 a 是 int &&
    // 但是 foo(a) 的参数 a这个表达式的a可以取地址，所以依然是左值；
    foo(a);
}

int main() {
    int a = 4;
    
    // 打印 foo(int &)
    f(std::move(a));
}
```

下面看看加了 std::forward 的版本

注：到 [Godbolt](https://godbolt.org/z/doYaKE3MG) 运行代码

``` cpp
#include <utility>
#include <type_traits>
#include <iostream>

void foo(int &a) {
    std::cout << "foo(int &)" << std::endl;
}

void foo(int &&a) { 
    std::cout << "foo(int &&)" << std::endl;
}

template <typename T>
void f(T && a) {

    static_assert(std::is_same_v<int, T>);
    static_assert(std::is_same_v<int&&, decltype(a)>);
    
    // 虽然 a 是 int &&
    // std::forward<int>(a) 就是 int&&
    foo(std::forward<int>(a));
}

int main() {
    int a = 4;
    
    // 打印 foo(int &&)
    f(std::move(a));
}
```

## 05.2. std::forward 实现 my_forward

``` cpp
#include <type_traits>

// 左值表达式
template <class T>
constexpr T&& my_forward(std::remove_reference_t<T>& t) noexcept {

    static_assert(std::is_same_v<T, int&>);
    
    return static_cast<T&&>(t);
}

// 右值表达式
template <class T>
constexpr T&& my_forward(std::remove_reference_t<T>&& t) noexcept {
    static_assert(!std::is_lvalue_reference_v<T>, "bad forward call");
    
    return static_cast<T&&>(t);
}
```

## 05.4. 传递左值引用

注：留意下面代码的 断言

注：到 [Godbolt](https://godbolt.org/z/PPTKMv5nx) 运行代码

``` cpp
#include <iostream>
#include <type_traits>

template <class T>
constexpr T&& my_forward(std::remove_reference_t<T>& t) noexcept {
    
    // 左值引用时候，T = int&
    static_assert(std::is_same_v<T, int&>);
    
    // 引用折叠，返回 int & && --> int &
    return static_cast<T&&>(t);
}

void foo(int &a) {
}

template <typename T>
void f(T && t) {
    // 左值引用时候，T = int& 
    static_assert(std::is_same_v<T, int&>);
    
    // 左值引用时候，t = int&
    static_assert(std::is_same_v<decltype(t), int&>);
    
    foo(my_forward<T>(t));
}

int main() {
    int a = 4;
 
    f(a);   
 
    return 0;
}
```

## 05.5. 传递右值引用

注：留意下面代码的 断言

注：到 [Godbolt](https://godbolt.org/z/vf3TrcYPW) 运行代码

``` cpp
#include <iostream>
#include <type_traits>

template <class T>
constexpr T&& my_forward(std::remove_reference_t<T>& t) noexcept {
    
    // 左值引用时候，T = int
    static_assert(std::is_same_v<T, int>);
    
    // 引用折叠，返回 int &&
    return static_cast<T&&>(t);
}

void foo(int &&a) {
}

template <typename T>
void f(T && t) {
    // 右值引用时候，T = int 
    static_assert(std::is_same_v<T, int>);
    
    // 右值引用时候，t = int&&
    static_assert(std::is_same_v<decltype(t), int&&>);
    
    // 注意：T = int
    foo(my_forward<T>(t));
}

int main() {
    int a = 4;
 
    f(std::move(a));   
 
    return 0;
}
```

## 05.6. 传递右值表达式

注：留意下面代码的 断言

注：到 [Godbolt](https://godbolt.org/z/Td377eq8M) 运行代码

``` cpp
#include <iostream>
#include <type_traits>

template <class T>
constexpr T&& my_forward(std::remove_reference_t<T>&& t) noexcept {
    
    static_assert(std::is_same_v<int, T>);
    
    static_assert(std::is_same_v<decltype(t), int&&>);
    
    static_assert(!std::is_lvalue_reference_v<T>, "bad forward call");

    // T&& --> int&&    
    return static_cast<T&&>(t);
}

void foo(int &&a) {
}

template <typename T>
void f(T && t) {
    // 左值引用，T = int& 
    static_assert(std::is_same_v<T, int&>);
    
    // 右值引用时候，t = int&
    static_assert(std::is_same_v<decltype(t), int&>);
    
    // std::remove_reference_t<T> = int
    static_assert(std::is_same_v<int, std::remove_reference_t<T>>);

    foo(my_forward<std::remove_reference_t<T>>(std::move(t)));
}

int main() {
    int a = 4;
 
    f(a);   
 
    return 0;
}
```

# 06. std::remove_reference_t 实现

注：到 [Godbolt](https://godbolt.org/z/1PxbYxfY5) 运行代码

``` cpp
#include <type_traits>

template <typename T>
struct my_remove_reference {
    using type = T;
};

template <typename T>
struct my_remove_reference<T&> {
    using type = T;
};

template <typename T>
struct my_remove_reference<T&&> {
    using type = T;
};

template <typename T>
using my_remove_reference_t = typename my_remove_reference<T>::type;


int main() {
    static_assert(std::is_same_v<int, my_remove_reference_t<int>>);
    
    static_assert(std::is_same_v<int, my_remove_reference_t<int&>>);
    
    static_assert(std::is_same_v<int, my_remove_reference_t<int&&>>);
    
    return 0;
}
```

# 07. std::is_lvalue_reference_v 实现

注：到 [Godbolt](https://godbolt.org/z/ocG17PhYT) 运行代码

``` cpp
template <class>
constexpr bool is_lvalue_reference_v = false;

template <class _Ty>
constexpr bool is_lvalue_reference_v<_Ty&> = true;

int main() {
    static_assert(!is_lvalue_reference_v<int>);
    
    static_assert(is_lvalue_reference_v<int&>);

    static_assert(!is_lvalue_reference_v<int&&>);
    
    return 0;
}
```