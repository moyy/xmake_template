#! https://zhuanlan.zhihu.com/p/685093351
# 《C++ template 2nd》03.非类型模板参数

# 1. 类模板参数

下面弄一个 限定 最大长度 的 栈，重点是参数：`Maxsize`

点击 [这里](https://godbolt.org/z/nYdazn46W) 运行

``` cpp
#include <array>
#include <cassert>

template<typename T, std::size_t Maxsize>
class Stack {
public:
    void push(T const& elem);
    
    void pop() {
        assert(!empty());
        --numElems;
    }
    
    const T& top() const {
        assert(!empty());
        return elems[numElems - 1];
    }
    
    bool empty() const {
        return numElems == 0;
    }
    
    std::size_t size() const {
        return numElems;
    }
private:
    std::array<T, Maxsize> elems;
    std::size_t numElems = 0;
};

template<typename T, std::size_t Maxsize>
void Stack<T,Maxsize>::push(T const& elem) {
    assert(numElems < Maxsize);
    elems[numElems++] = elem;
}

int main() {
    Stack<int, 3> s;
    assert(s.size() == 0);

    s.push(10);
    s.push(20);
    s.push(30);

    s.pop();
    s.pop();
    assert(s.top() == 10);

    return 0;
}
```

# 2. 函数模板参数

点击 [这里](https://godbolt.org/z/b8MY6Pnaa) 运行

``` cpp
template<int Val = 10, typename T>
constexpr T addValue (T x)
{
    return x + Val;
}

int main() {
    constexpr auto b = addValue(5);
    static_assert(b == 15);

    constexpr auto  c = addValue<100>(3.14);
    static_assert(c == 103.14);
    
    return 0;
}
```

根据值推导类型：decltype

点击 [这里](https://godbolt.org/z/K6WdKnaWv) 运行

``` cpp
#include <type_traits>

template<auto Val, typename T = decltype(Val)>
constexpr T foo() {
    return Val;
}

int main() {
    constexpr auto a = foo<10>();
    static_assert(std::is_same_v<decltype(a), const int>);

    constexpr auto b = foo<3.14f>();
    static_assert(std::is_same_v<decltype(b), const float>);
    
    return 0;
}
```

根据类型推导默认值 T(): 

点击 [这里](https://godbolt.org/z/61Gh8K5EW) 运行

``` cpp
template<typename T, T Val = T{}>
constexpr T bar() {
    return Val;
}

int main() {
    constexpr auto a = bar<int>();
    static_assert(a == 0);

    constexpr auto b = bar<float>();
    static_assert(b == 0.0f);
    
    return 0;
}
```

# 3. 限制

+ 可以是 整型常量值 (包括枚举)
+ 可以是 指向 对象 / 函数 / 成员的指针
+ 可以是 指向 对象 / 函数 的 左值引用
+ 可以是 std::nullptr_t (nullptr 的类型)
+ C++ 20 可以 是 浮点数；
+ 结构体 / 对象 非类型模板参数，必须是 结构化类型

浮点数：C++ 17 不允许啊

点击 [这里](https://godbolt.org/z/73eYM1ee4) 运行

请将编译参数改为 `--std=C++17` 看看编译错误

``` cpp
#include <cassert>

// C++ 17 编译错误：不能是 浮点数
template<double VAT>
double process (double v)
{
    return v * VAT;
}

int main() {
    auto r = process<10.0>(2.0);
    assert(r == 20.0);
    
    return 0;
}
```

结构体 / 对象 非类型模板参数，必须是 结构化类型

点击 [这里](https://godbolt.org/z/eWWo198r4) 看编译错误

``` cpp
struct S {
    int a = 1;
    int b = 2;
};

class C {
private:
    int c = 5;
};

// 报错：非类型模板参数，必须是 结构化类型
// 不能有 构造，赋值
// 所有成员public
// 不能有 虚函数 / 虚基类
template<C name>
class CTemplate {
};

template<S name>
class STemplate {
};

int main() {
    STemplate<S{}> s;

    CTemplate<C{}> c;

    return 0;
}
```

必须是有效的表达式：

点击 [这里](https://godbolt.org/z/Yj1sce3o9) 看编译错误

``` cpp
template<int I, bool B>
class C {};

int main() {
    
    C<sizeof(int) + 4, sizeof(int)==4> c1;
    
    C<42, (sizeof(int) > 4)> c2;

    // 编译错误：first > ends the template argument list
    C<42, sizeof(int) > 4> c3; 
    
    return 0;
}
```

# 4. C++ 17: auto 非类型模板参数

点击 [这里](https://godbolt.org/z/bjK6Ea164) 运行

``` cpp
#include <array>
#include <string>
#include <cassert>
#include <type_traits>

// 注意 auto Maxsize
template<typename T, auto Maxsize>
class Stack {
    // 注意：定义类型
    using size_type = decltype(Maxsize);
public:
    Stack(): numElems(0) {}

    void push(T const& elem) {
        assert(numElems < Maxsize);
        elems[numElems++] = elem;
    }

    void pop() {
        assert(!empty());
        --numElems;
    }

    T const& top() const {
        assert(!empty());
        return elems[numElems-1];
    }
    
    bool empty() const {
        return numElems == 0;
    }

    auto size() const {
        return numElems;
    }
private:
    std::array<T, Maxsize> elems;
    size_type numElems;
};

int main() {
    Stack<int, 20u> int20Stack;
    int20Stack.push(7);
    assert(7 == int20Stack.top());

    Stack<std::string, 40> stringStack;
    stringStack.push("hello");
    assert(stringStack.top() == "hello");
    
    auto size1 = int20Stack.size();
    static_assert(std::is_same_v<decltype(size1), unsigned int>);

    auto size2 = stringStack.size();
    static_assert(std::is_same_v<decltype(size2), int>);

    return 0;
}
```

注意: 下面代码 允许将 N 实例化为引用

+ `auto`：推导时，衰变；
+ `decltype(auto)`：推导时，会保留完整类型信息，包括 左右值 / cv-限定符

点击 [这里](https://godbolt.org/z/bjK6Ea164) 运行

``` cpp
#include <type_traits>

template<decltype(auto) N>
struct C {
    static_assert(std::is_same_v<decltype(N), const int&>);
};

const int i = 0;
const int &ri = i;

int main() {
    C<ri> x; 
    return 0;
}
```