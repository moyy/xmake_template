#! https://zhuanlan.zhihu.com/p/684537801
# 《C++ template 2nd》01. 函数模板

# 1. 定义模板

+ 下面片段，T 需要 实现 < 符号
+ C++ 17 之后，T 不需要 满足 拷贝 / 移动
+ `typename` 可以 换成 `class`，但建议只用 `typename`
+ ::max(...) 在 全局作用域找 max函数
+ 具体类型 替换 模板参数 的 过程 称为 `模板实例化`，生成 `模板实例`

点击 [这里](https://godbolt.org/z/aranf6Yvz) 运行

``` c++
#include <iostream>

// 返回 两个参数 中的 大者
template <typename T>
T max(T a, T b) {
    // Equivalence 要求 满足 <，相等 为 !(a < b) && !(b < a)
    // Equality    要求 满足 ==，相等 定义为 a == b。
    
    // C++ 标准规定 Returns the first argument when the arguments are equivalent.
    // 见：https://www.zhihu.com/question/266917342
    return b < a ? a : b;
    // return a < b ? b : a; //
}

int main()
{
    int i = 42;
    
    // 实例化 int max(int a, int b);
    // max(7,i): 42
    std::cout << "max(7,i): " << ::max(7,i) << std::endl; 

    double f1 = 3.4;
    double f2 = -6.7;

    // 实例化 double max(double a, double b);
    // max(f1,f2): 3.4
    std::cout << "max(f1,f2): " << ::max(f1,f2) << std::endl; 

    std::string s1 = "mathematics";
    std::string s2 = "math";
   
    // 实例化 std::string max(std::string a, std::string b);
    // max(s1,s2): mathematics
    std::cout << "max(s1,s2): " << ::max(s1,s2) << std::endl; 

    return 0;
}
```

# 2. 模板的两阶段编译 

+ 若在定义时不进行实例化，则会 忽略模板参数 去 检查模板代码的正确性
  - 语法错误，如: 缺少分号。
  - 使用 未知名 的 模板参数 (类型名、函数名......)。
  - 检查 不依赖于模板参数 的 静态断言
+ 实例化时，再次检查模板代码，确保生成的代码有效，如 T对应的具体类型是否满足 < 运算符

注意：有些编译器，在定义处不会进行语法检查，全部弄到实例化再检查！

点击 [这里](https://godbolt.org/z/6h1Psvv9P) 查看编译错误

```c++
#include <iostream>

template <typename T>
T foo(T a) {
    // gcc 13.2 中，即使没有实例化模板，也会编译报错；
    t = a
    
    return t;
}

int main() {
    // std::cout << foo(0) << std::endl;
    return 0;
}
```

# 3. 参数推导规则

`cv-限定符`，指的是：const / volatile 限定符

模板 形参 按值传递: T a，实例化时：

+ 忽略 实参的引用；例子：实参是 int&，T 推导为 int
+ 忽略 实参的 `cv-限定符`；例子：实参 是 const int，T 推导为 int
+ 数组 / 函数，变 指针，例子：实参 是 int[10]，T 推导为 int*

点击 [这里](https://godbolt.org/z/rx9z1xePv) 查看编译错误

``` cpp
#include <string>

template<typename T>
T max (T a, T b) {
    return b < a ? a : b;
}

int main() {
    int i = 17;
    int const c = 42;
    
    max(i, i); // OK: T推导为int
    max(i, c); // OK: T推导为int
    max(c, c); // OK: T推导为int
    
    int& ir = i;
    max(ir, ir); // OK: T推导为int
    max(i, ir); // OK: T推导为int
    
    int arr[4];
    max(&i, arr); // OK: T推导为int*

    max(4, 7.2); // ERROR: T推导为int或double
    
    std::string s;
    max("hello", s); // ERROR: T推导为char const*或std::string

    return 0;
}
```

有三种方法，解决上面例子的错误：

+ 强制转换: max(static_cast<double>(4), 7.2); 
+ 显式指定 T 类型: max<double>(4, 7.2); 
+ 定义模板时，参数定义为不同类型，见 第5节；

模板 形参 按引用传递: T &a，实例化时：类型必须完全匹配：包括 `cv-限定符`；

点击 [这里](https://godbolt.org/z/b8fEjhY1T) 查看编译错误

``` cpp
template<typename T>
T& max (T& a, T& b) {
    return b < a ? a : b;
}

int main() {
    int i = 17;
    int const c = 42;
    
    max(i, i); // OK: T推导为 int
    max(c, c); // OK: T推导为 const int
    
    max(i, c); // Error
    
    return 0;
}
```

模板 形参 按指针传递: T *a，实例化时：类型必须完全匹配：包括 `cv-限定符`；

点击 [这里](https://godbolt.org/z/s6d8f64W6) 查看编译错误

``` cpp
template<typename T>
T* max (T* a, T* b) {
    return *b < *a ? a : b;
}

int main() {
    int i = 17;
    int const c = 42;
    
    max(&i, &i); // OK: T推导为 int
    max(&c, &c); // OK: T推导为 const int
    
    max(&i, &c); // Error
    
    return 0;
}
```

# 4. 默认参数

当 模板函数指定默认参数时，类型 也要 指定 默认参数的类型

点击 [这里](https://godbolt.org/z/qnnEhh9vK) 运行

``` cpp
#include <string>

template<typename T = std::string>
void f(T = "") {}

int main() {
    f(1); // OK: 推导出T为int，因此其可以调用f<int>(1)
    f();  // OK: 默认类型和参数
    return 0;
}
```

# 5. 多模板参数

下面 max函数 的 两个参数 分别定义成 T1，T2

点击 [这里](https://godbolt.org/z/9vWer38xz) 运行

``` cpp
// 注意：这么写最大的问题是：T1 比 T2 窄化时候，精度会丢失
template<typename T1, typename T2>
T1 max (T1 a, T2 b)
{
    return b < a ? a : b;
}

int main () {
    auto m = ::max(4, 7.2); // OK, 不过返回类型与第一个参数类型一样
    return 0;
}
```

三个解决方法：

+ 5.1. 坏：为返回类型引入第三个模板参数
+ 5.2. 好：让编译器找出返回类型
+ 5.3. 好：将返回类型声明为两个参数类型的“公共类型”

## 5.1. 坏：为返回类型引入第三个模板参数

这里的问题是，模板推导 和 函数重载 都不处理返回类型，需要显式指定；

点击 [这里](https://godbolt.org/z/c6h16x6hz) 运行

``` cpp
template<typename RT, typename T1, typename T2>
RT max (T1 a, T2 b)
{
    return b < a ? a : b;
}

int main () {
    auto m = ::max<double>(4, 7.2); // OK, 需要指定返回类型 RT = double
    return 0;
}
```

## 5.2. 好：用 `auto` 让编译器找出返回类型

注：下面的代码存在一个思维误区，以为 max的返回值是由运行时 a，b 大者 的 返回值 决定的，其实不是；

因为：编译时，调用 max 的 a，b类型就确定了，所以 max的返回值类型也确定了；

返回值 由 T1，T2 的 精度大的决定，比如 T1=int，T2=double，则返回类型为 double；

点击 [这里](https://godbolt.org/z/vd91GdKMW) 运行

``` cpp
// C++ 14后
#include <type_traits>

template<typename T1, typename T2>
auto max (T1 a, T2 b)
{
    return b < a ? a : b;
}

int main () {
    auto r1 = ::max(4, 7.2); // OK, double r1
    static_assert(std::is_same_v<decltype(r1), double>);

    double a = 4.1;
    int b = 7;
    auto r2 = ::max(a, b); // OK, double r2
    static_assert(std::is_same_v<decltype(r2), double>);

    double &ra = a;
    int &rb = b;
    
    static_assert(std::is_same_v<decltype(ra), double&>);
    static_assert(std::is_same_v<decltype(rb), int&>);
    
    auto r3 = ::max(ra, rb); // OK, double r3
    static_assert(std::is_same_v<decltype(r3), double>);

    return 0;
}
```

题外话，C++ 11，得这么写：

+ 下面代码的 decltype(b<a?a:b) 可以改写成 decltype(true?a:b) 原因同上一段代码的注释也一样的；

点击 [这里](https://godbolt.org/z/zbGTsGb7b) 运行

``` cpp
#include <type_traits>

// C++ 11, auto -> decltype
template<typename T1, typename T2>
auto max (T1 a, T2 b) -> typename std::decay<decltype(b<a?a:b)>::type
{
    return b < a ? a : b;
}

int main () {
    auto r1 = ::max(4, 7.2); // OK, double r1
    static_assert(std::is_same<decltype(r1), double>::value);

    double a = 4.1;
    int b = 7;
    auto r2 = ::max(a, b); // OK, double r2
    static_assert(std::is_same<decltype(r2), double>::value);

    double &ra = a;
    int &rb = b;
    
    static_assert(std::is_same<decltype(ra), double&>::value);
    static_assert(std::is_same<decltype(rb), int&>::value);
    
    auto r3 = ::max(ra, rb); // OK, double r3
    static_assert(std::is_same<decltype(r3), double>::value);

    return 0;
}
```

## 5.3. 好：将返回类型声明为两个参数类型的“公共类型”

用 `std::common_type_t<T1, T2>` 显式指定，就要 两个类型的公共类型；

点击 [这里](https://godbolt.org/z/vYdzv4cr9) 运行

``` cpp
#include <type_traits>

template<typename T1, typename T2>
std::common_type_t<T1, T2> max (T1 a, T2 b)
{
    return b < a ? a : b;
}

int main () {
    double a = 4.8;
    int b = 7;
    
    auto r = ::max(a, b); // OK, double r
    static_assert(std::is_same_v<decltype(r), double>);

    return 0;
}
```

# 6. 重载

点击 [这里](https://godbolt.org/z/MPnfGvKfb) 运行

``` cpp
#include <iostream>

template <typename T>
T max (T a, T b) {
    std::cout << "T max(T, T);" << std::endl;
    return a < b ? b: a;
}

int max (int a, int b) {
    std::cout << "int max(int, int);" << std::endl;
    return a < b ? b: a;
}

int main() {
    ::max(7, 42);          // int max(int, int);
    ::max(7.0, 42.0);      // T max(T, T);
    ::max('a', 'b');       // T max(T, T);  
    ::max<>(7, 42);        // T max(T, T); 只有模板才能解析 <>
    ::max<double>(7, 42);  // T max(T, T); 只有模板才能解析 <>
    ::max('a', 42.7);      // int max(int, int); 只有非模板函数允许这种类型转换
    
    return 0; 
}
```

在调用函数之前，要声明所有的重载版本，否则有时候会出错

点击 [这里](https://godbolt.org/z/3oP76Yjca) 运行

``` cpp
#include <iostream>

// 打开这行注释，就可以 调用 int max(int, int) 了
// int max(int a, int b);

template <typename T>
T max (T a, T b) {
    std::cout << "T max(T, T);" << std::endl;
    return a < b ? b: a;
}

template <typename T>
T max (T a, T b, T c) {
    return max(max(a, b), c); // max(a, b) 只能是 模板的，下面的 int 版本在这里不可见
}

int max (int a, int b) {
    std::cout << "int max(int, int);" << std::endl;
    return a < b ? b: a;
}

int main() {
    ::max(7, 42, 60);
    
    return 0; 
}
```
