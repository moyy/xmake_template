#! https://zhuanlan.zhihu.com/p/689272226
# 《C++ template 2nd》08. 编译时编程

+ static_assert 编译时断言
+ 模板偏特化
+ constexpr: 常量表达式
+ SFINAE，选择模板
+ C++ 17 引入了“编译时 if”

# 01. 模板元编程

模板元编程：就是使用 偏特化，typename 类型化，constexpr 常量表达式，递归 进行 图灵计算的过程。

下面仅以判断素数为例子，看看模板元编程的样子：

判断 p 是否 为素数，除了0-3之外，就要递归 判断 p 是否被 [2, p/2] 的 整数整除，都不能整除就是素数。

判断 p 是否被 [2, d] 的整数整除，就是 递归的过程 p能否被d整除，或者 p 是否被 [2, d-1] 整除。

点击 [这里](https://godbolt.org/z/YnhY35PGr) 运行

``` cpp
// 递归判断，p 能否被 [2, d] 的 整数 整除
template <unsigned p, unsigned d> 
struct IsDivisible {
    static constexpr bool value = (p % d == 0) || IsDivisible<p, d - 1>::value;
};

// 递归终点：p 能否被 2 整除
template <unsigned p> 
struct IsDivisible<p, 2> {
    static constexpr bool value = (p % 2 == 0);
};

template<unsigned p> 
struct IsPrime {
    // 开始递归，看 p 能否被 [2, p/2] 的 整数 整除
    static constexpr bool value = !IsDivisible<p, p / 2>::value;
};

// 初始值：这几个值算法覆盖不到
template<>
struct IsPrime<0> { static constexpr bool value = false; };

template<>
struct IsPrime<1> { static constexpr bool value = false; };

template<>
struct IsPrime<2> { static constexpr bool value = true; };

template<>
struct IsPrime<3> { static constexpr bool value = true; };

int main() {
    static_assert(IsPrime<2>::value);
    static_assert(IsPrime<3>::value);
    static_assert(!IsPrime<4>::value);
    static_assert(IsPrime<5>::value);
  
    static_assert(IsPrime<19>::value);
    static_assert(!IsPrime<20>::value);
}
```

# 02. 常量表达式 constexpr

+ 编译时需要值时 (数组长度 / 非类型模板参数 / static_assert)，编译器会计算，计算不了就报错；
+ 否则：编译器自行决定 编译时还是运行时计算；

到了 C++ 14，constexpr 支持 if / for，就可以仅用它表达值计算了，不需要偏特化和模板了。

点击 [这里](https://godbolt.org/z/K3Ex9bbzz) 运行

``` cpp
constexpr bool isPrime (unsigned p)
{
    for (unsigned int d = 2; d <= p / 2; ++d) {
        if (p % d == 0) {
            return false;
        }
    }
    return p > 1;
}

int main() {
    static_assert(isPrime(2));
    static_assert(isPrime(3));
    static_assert(!isPrime(4));
    static_assert(isPrime(5));
  
    static_assert(isPrime(19));
    static_assert(!isPrime(20));
}
```

# 03. 偏特化选择不同的策略

编译时使用偏特化在不同实现之间进行选择

点击 [这里](https://godbolt.org/z/En4zjeefG) 运行

``` cpp
// 主模板
template<int SZ, bool = isPrime(SZ)>
struct Helper;

// 如果 SZ 不是素数
template<int SZ>
struct Helper<SZ, false>
{
    int a = 1;
};

// 如果 SZ 是素数
template<int SZ>
struct Helper<SZ, true>
{
    int a = 2;
};

template<typename T, long unsigned SZ>
int foo (std::array<T, SZ> const& coll)
{
    Helper<SZ> h;
    return h.a;
}

int main() {
    std::array<int, 10> coll;
    assert(1 == foo(coll));
}
```

# 04.  `SFINAE`: 替换失败不是错误

当模板参数依次替换实参的过程中，出现了语法错误，编译器并不认为是。

如果所有候选的模板参数都失败了，编译器会报错：找不到匹配的函数；

点击 [这里](https://godbolt.org/z/sbKz3aWs4) 查看编译错误

``` cpp
// 数组会用这个重载
template <typename T, unsigned N>
int len (T(&)[N])
{
    return N;
}

// T 有个 size_type 的 关联类型就会 用这个重载
template<typename T>
typename T::size_type len (T const& t)
{
    return t.size();
}

int main() {
    int a[10];
    // 用数组版本
    // 当用 T::size_type 版本的时，函数原型会有编译错误
    // 但这个并不认为是错误，而是直接忽略这个函数；
    len(a); 
    
    // 用数组版本
    // 当用 T::size_type 版本的时，函数原型会有编译错误
    // 但这个并不认为是错误，而是直接忽略这个函数；
    len("tmp"); 

    std::vector<int> v;
    // 用 T::size_type 的版本
    len(v);  

    int* p;
    // ERROR: no matching len() function found
    // 注意：指针 也没有 size_type，这也不是编译错误
    len(p); 

    // allocator 有 size_type，但没有 size() 函数
    // 所以会选择 T::size_type 的版本
    // 但 实例化 时，发现没有 size() 函数，就会报错
    std::allocator<int> x;
    len(x); // ERROR: len() function found, but can’t size()
}
```

下面看看 再加一个 万能重载 ... 的情况：

加了 万能重载 ... 后，指针是不报错了：

``` cpp
int len(...) {

}

int* p;
len(p); // 调用了 len(...)
```

但 std::allocation 的问题没解决；

点击 [这里](https://godbolt.org/z/q9173Evvd) 查看编译错误

``` cpp
#include <vector>

// 数组会用这个重载
template <typename T, unsigned N>
int len (T(&)[N])
{
    return N;
}

// T 有个 size_type 的 关联类型就会 用这个重载
template<typename T>
typename T::size_type len (T const& t)
{
    return t.size();
}

int len(...) {

}

int main() {
    int a[10];
    // 用数组版本
    // 当用 T::size_type 版本的时，函数原型会有编译错误
    // 但这个并不认为是错误，而是直接忽略这个函数；
    len(a);
    
    // 用数组版本
    // 当用 T::size_type 版本的时，函数原型会有编译错误
    // 但这个并不认为是错误，而是直接忽略这个函数；
    len("tmp"); 

    std::vector<int> v;
    // 用 T::size_type 的版本
    len(v);

    int* p;
    // 注意：指针 也没有 size_type，这也不是编译错误
    len(p); // 调用了 len(...)
    
    // allocator 有 size_type，但没有 size() 函数
    // 所以会选择 T::size_type 的版本
    // 但 实例化 时，发现没有 size() 函数，就会报错
    std::allocator<int> x;
    len(x); // ERROR: has no member named 'size'
}
```

如何通过 `SFINAE` 同时判断 T::size_type 有没有 size() 函数？

除了 enable_if 外，还可以用 decltype

``` cpp
// 函数原型 符合 `SFINAE` 范畴：decltype( (void)(t.size()), T::size_type() )
// 逗号表达式，第一个是 t.size()，所以必须判断 有没有 size
// 返回值类型是 逗号表达式 最后一个类型；
// 转换为 void，是为了避免表达式使用了用户重载定义的逗号操作符
template<typename T>
auto len (T const& t) -> decltype( (void)(t.size()), T::size_type() )
{
    return t.size();
}
```

点击 [这里](https://godbolt.org/z/rdxYa4b5x) 运行

``` cpp
#include <vector>

// 数组会用这个重载
template <typename T, unsigned N>
int len (T(&)[N])
{
    return N;
}

// T 有个 size_type 的 关联类型就会 用这个重载
template<typename T>
auto len (T const& t) -> decltype( (void)(t.size()), T::size_type() )
{
    return t.size();
}

int len(...) {
    return 0;
}

int main() {
    int a[10];
    // 用数组版本
    // 当用 T::size_type 版本的时，函数原型会有编译错误
    // 但这个并不认为是错误，而是直接忽略这个函数；
    len(a);
    
    // 用数组版本
    // 当用 T::size_type 版本的时，函数原型会有编译错误
    // 但这个并不认为是错误，而是直接忽略这个函数；
    len("tmp"); 

    std::vector<int> v;
    // 用 T::size_type 的版本
    len(v);

    int* p;
    // 注意：指针 也没有 size_type，这也不是编译错误
    len(p); // 调用了 len(...)
    
    std::allocator<int> x;
    len(x); // 调用了 len(...)

    return 0;
}
```

# 05. C++ 17: constexpr if 

编译器在编译时决定是走 if分支 还是走 else 分支，因此不需要借助 结构体+偏特化 来选择策略了

点击 [这里](https://godbolt.org/z/zznqcj8E6) 运行

``` cpp
template<typename T, std::size_t SZ>
int foo (std::array<T,SZ> const& coll)
{
    if constexpr(isPrime(SZ)) {
        return 0;
    } else {
        // 一段很复杂的很耗时的代码在这里。
        return 1;
    }
}

int main() {
    std::array<int, 10> coll;
    assert(1 == foo(coll));
}
```