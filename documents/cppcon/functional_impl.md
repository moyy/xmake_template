#! https://zhuanlan.zhihu.com/p/685839521
# 邯郸学步 / 东施效颦 之 std::function 的 简化版实现

# 01. 背景

笔记来源：

+ [Blog: std::function 基本实现](http://blog.bitfoc.us/p/525)
+ [油管: C++ Weekly - Ep 333 - A Simplified std::function Implementation](https://www.youtube.com/watch?v=xJSKk_q25oQ)

# 02. std::function 基本使用

点击 [这里](https://godbolt.org/z/1TPPv63ej) 运行代码

``` cpp
#include <cassert>
#include <functional>

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

// 函数对象
struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y)
    {
        return v_ + x + y;
    }

    int v_;
};

int main()
{
    // 包装 全局函数
    std::function f(global_function);
    assert(f(1, 2) == 3);

    // 包装 函数对象
    std::function g(BinaryFunction(10));
    assert(g(1, 2) == 13);
   
    return 0;
}
```

# 03. 实现开始的地方

+ 函数对象 = 实现了 operator() 的 类 / 结构体； 
+ std::function = 模板化 的 函数对象；

我们先考虑一个简化版本的问题：用什么抽象 去 封装 (int, int) 的 调用，让他能同时容纳 函数 / 函数对象？

不考虑模板的话：

+ 构造时，将具体的 类型传进来
+ 调用的时候 operator(int, int) 不需要 考虑类型；
+ 妥妥的 虚函数 实现多态 的 套路 啊！

点击 [这里](https://godbolt.org/z/dYsTcM8z5) 运行代码

``` cpp
#include <cassert>

// 抽象类
class CallableBase {
public:
    virtual int operator()(int, int) = 0;

    virtual ~CallableBase() = default;
};

// ==================== 针对 全局函数的

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

// 接函数指针的版本
class FunctionCall: public CallableBase {
public:
    FunctionCall(int(*f)(int, int)): f_(f) {}

    virtual int operator()(int x, int y) override {
        return f_(x, y);
    }
private:
    int (*f_)(int, int);
};

// ==================== 针对 特定的 BinaryFunction 类 的

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

// 接函数指针的版本
class BinaryCall: public CallableBase {
public:
    BinaryCall(BinaryFunction f): f_(f) {}

    int operator()(int x, int y) override {
        return f_(x, y);
    }
private:
    BinaryFunction f_;
};

int main()
{
    // 包装 全局函数
    FunctionCall f(global_function);
    CallableBase &base_1 = f;

    BinaryCall g(BinaryFunction(10));
    CallableBase &base_2 = g;

    // 特别提醒：这里的 base_1, base_2 都是 CallableBase 的 引用
    // 调用函数时，已经 不知道 原始类型；
    // 有点像 std::function 的 统一 了；
    assert(base_1(1, 2) == 3);

    // 包装 函数对象
    assert(base_2(1, 2) == 13);
   
    return 0;
}
```

# 04. std::function = 模板化 的 函数对象

上面的代码，虽然用 虚函数 弄了个 CallableBase 统一了 两参数 的调用，让其调用时，不需要知道具体来源

但是 总不能一个 具体 类型 就实现 一个 CallableBase 的 子类吧？

+ 全局函数
+ 函数指针
+ 类成员函数
+ 类静态成员函数
+ 类成员函数指针
+ 类静态成员函数指针
+ 函数对象
+ lambda函数

这时候，模板就出场了；仔细观察以上的例子，无非就是构造时候传值，所以可以写个统一的模板：

``` cpp
template <typename Src>
class Callable: public CallableBase {
public:
    Callable(Src src): src_(src) {}

    int operator()(int x, int y) override {
        return src_(x, y);
    }
private:
    Src src_;
};

```

点击 [这里](https://godbolt.org/z/T8an5bT31) 运行代码:

``` cpp
#include <cassert>

// 抽象类
class CallableBase {
public:
    virtual int operator()(int, int) = 0;

    virtual ~CallableBase() = default;
};

template <typename Src>
class Callable: public CallableBase {
public:
    Callable(Src src): src_(src) {}

    int operator()(int x, int y) override {
        return src_(x, y);
    }
private:
    Src src_;
};

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

int main()
{
    // 包装 全局函数
    Callable f(global_function);
    CallableBase &base_1 = f;

    Callable g(BinaryFunction(10));
    CallableBase &base_2 = g;

    assert(base_1(1, 2) == 3);

    // 包装 函数对象
    assert(base_2(1, 2) == 13);
   
    return 0;
}
```

# 05. 从 Callable 到 std::function ?

std::function 可以指定 任何参数类型，返回值类型；

我们先拿 两个参数为例子！

``` cpp
template <typename Ret, typename Arg1, typename Arg2, typename Src>
class Callable: public CallableBase<Ret, Arg1, Arg2> {
public:
    Callable(Src src): src_(src) {}

    // 将 返回类型 和 参数 模板化
    Ret operator()(Arg1 x, Arg2 y) override {
        return src_(x, y);
    }
private:
    Src src_;
};
```

点击 [这里](https://godbolt.org/z/q47PEK57v) 运行代码:

``` cpp
#include <cassert>

// 抽象类
template <typename Ret, typename Arg1, typename Arg2>
class CallableBase {
public:
    virtual Ret operator()(Arg1, Arg2) = 0;

    virtual ~CallableBase() = default;
};

template <typename Ret, typename Arg1, typename Arg2, typename Src>
class Callable: public CallableBase<Ret, Arg1, Arg2> {
public:
    Callable(Src src): src_(src) {}

    // 将 返回类型 和 参数 模板化
    Ret operator()(Arg1 x, Arg2 y) override {
        return src_(x, y);
    }
private:
    Src src_;
};

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

int main()
{
    // 包装 全局函数
    Callable<int, int, int, int (*)(int, int)> i { global_function };
    CallableBase<int, int, int> &base_1 = i;

    Callable<float, float, int, float (*)(float, int)> f {global_function_float};
    CallableBase<float, float, int> &base_2 = f;

    Callable<int, int, int, BinaryFunction> g(BinaryFunction(10));
    CallableBase<int, int, int> &base_3 = g;

    assert(base_1(1, 2) == 3);

    assert(base_2(2.5f, 2) == 5.0f);

    assert(base_3(1, 2) == 13);
   
    return 0;
}
```

# 06. 擦除 Src 类型

从上面的使用就可以看到：

+ std::function 更类似 CallableBase，Callable 的最后一个 模板参数 Src 更像是要擦除的类型；
+ 基类指针不是可以指向派生类，这样就可以在构造后不需要指定Src了嘛？
+ 那么 根本就不是 从 Callable 到 std::function，而是 从 CallableBase 到 std::function

我们试试 写个函数 make_callable 模板函数，将 具体的 Src 传进来，返回 CallableBase*

这样子，就能彻底隔开 Src 类型了；

``` cpp
// 用 模板 + 返回基类的指针，去 “擦除类型”
template <typename Ret, typename Arg1, typename Arg2, typename Src>
CallableBase<Ret, Arg1, Arg2>* make_callable(Src src) {
    return new Callable<Ret, Arg1, Arg2, Src>(src);
}
```

点击 [这里](https://godbolt.org/z/9xrnoq3qM) 运行代码:

``` cpp
#include <cassert>

// 抽象类
template <typename Ret, typename Arg1, typename Arg2>
class CallableBase {
public:
    virtual Ret operator()(Arg1, Arg2) = 0;

    virtual ~CallableBase() = default;
};

template <typename Ret, typename Arg1, typename Arg2, typename Src>
class Callable: public CallableBase<Ret, Arg1, Arg2> {
public:
    Callable(Src src): src_(src) {}

    // 将 返回类型 和 参数 模板化
    Ret operator()(Arg1 x, Arg2 y) override {
        return src_(x, y);
    }
private:
    Src src_;
};

// 用 模板 + 返回基类的指针，去 “擦除类型”
template <typename Ret, typename Arg1, typename Arg2, typename Src>
CallableBase<Ret, Arg1, Arg2>* make_callable(Src src) {
    return new Callable<Ret, Arg1, Arg2, Src>(src);
}

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

int main() {
    auto *f = make_callable<int, int, int>(global_function);
    assert(f->operator()(1, 2) == 3);
    delete f;

    auto *g = make_callable<float, float, int>(global_function_float);
    assert(g->operator()(1.5f, 2) == 3.0f);
    delete g;

    auto *h = make_callable<int, int, int>(BinaryFunction(10));
    assert(h->operator()(1, 2) == 13);
    delete h;

    return 0;
}
```

# 07. 封装 function

到这里，将 make_callable, CallableBase, Callable 封装到一个类中，以隐藏其实现细节;

点击 [这里](https://godbolt.org/z/dExrxMeqW) 运行代码:

``` cpp
#include <cassert>

template <typename Ret, typename Arg1, typename Arg2>
class function {
public:
    // 模板化的构造函数，就是上面的 make_callable
    // 用 模板 + 返回基类的指针，去 “擦除类型”
    template <typename Src>
    function(Src src) : ptr_(new Callable<Src>(src)) { }

    ~function() { delete ptr_; }

    Ret operator()(Arg1 arg1, Arg2 arg2) {
        return ptr_->operator()(arg1, arg2);
    }
private:
    class CallableBase {
    public:
        virtual Ret operator()(Arg1, Arg2) = 0;

        virtual ~CallableBase() = default;
    };

    template <typename Src>
    class Callable: public CallableBase {
    public:
        Callable(Src src): src_(src) {}

        // 将 返回类型 和 参数 模板化
        Ret operator()(Arg1 x, Arg2 y) override {
            return src_(x, y);
        }
    private:
        Src src_;
    };
private:
    CallableBase* ptr_;
};

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

int main() {
    ::function<int, int, int> f {global_function};
    assert(f(1, 2) == 3);
    
    ::function<float, float, int> g { global_function_float };
    assert(g(1.5f, 2) == 3.0f);
    
    ::function<int, int, int> h { BinaryFunction(10) };
    assert(h(1, 2) == 13);
    
    return 0;
}
```

到这里，我们 终于 有了一个 2参数的 初步可用的 泛型 函数调用了！

# 08. 按 std::function 特化 类型声明

用模板偏特化技术；

``` cpp
// 一般模板，啥都没有，调用就是编译报错
template <typename T>
class function;

// 偏特化，将 函数原型写上
template <typename Ret, typename Arg1, typename Arg2>
class function<Ret(Arg1, Arg2)> {
    // ...
}
```

点击 [这里](https://godbolt.org/z/zWv6vhbK9) 运行代码:

``` cpp
#include <cassert>

// 一般模板，啥都没有，调用就是编译报错
template <typename T>
class function;

// 偏特化，将 函数原型写上
template <typename Ret, typename Arg1, typename Arg2>
class function<Ret(Arg1, Arg2)> {
public:
    // 模板化的构造函数，就是上面的 make_callable
    // 用 模板 + 返回基类的指针，去 “擦除类型”
    template <typename Src>
    function(Src src) : ptr_(new Callable<Src>(src)) { }

    ~function() { delete ptr_; }

    Ret operator()(Arg1 arg1, Arg2 arg2) {
        return ptr_->operator()(arg1, arg2);
    }
private:
    class CallableBase {
    public:
        virtual Ret operator()(Arg1, Arg2) = 0;

        virtual ~CallableBase() = default;
    };

    template <typename Src>
    class Callable: public CallableBase {
    public:
        Callable(Src src): src_(src) {}

        // 将 返回类型 和 参数 模板化
        Ret operator()(Arg1 x, Arg2 y) override {
            return src_(x, y);
        }
    private:
        Src src_;
    };
private:
    CallableBase* ptr_;
};

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

int main() {
    
    // 编译报错，并没有这样的模板
    // ::function<int, int, int> test;
    
    // 编译报错：无法推导 int(&)(int, int) ?
    // ::function f1 {global_function};
    // assert(f1(1, 2) == 3);
    
    ::function<int(int, int)> f {global_function};
    assert(f(1, 2) == 3);
    
    ::function<float(float, int)> g { global_function_float };
    assert(g(1.5f, 2) == 3.0f);
    
    // 注意这种 函数声明 同样适用于 函数对象；
    ::function<int(int, int)> h { BinaryFunction(10) };
    assert(h(1, 2) == 13);
    
    return 0;
}
```

# 09. 析构 / 拷贝 / 赋值：五之法则

既然 function 实现了 析构函数，按 Big Five 法则，就要实现 拷贝 / 赋值 / 移动，否则很容易出问题

在 Callablebase 中，要实现 一个 虚函数 clone，隐藏 掉具体的 Callable 类型

点击 [这里](https://godbolt.org/z/cqn4oahPs) 运行代码:

``` cpp
#include <cassert>

// 一般模板，啥都没有，调用就是编译报错
template <typename T>
class function;

// 偏特化，将 函数原型写上
template <typename Ret, typename Arg1, typename Arg2>
class function<Ret(Arg1, Arg2)> {
public:
    // 模板化的构造函数，就是上面的 make_callable
    // 用 模板 + 返回基类的指针，去 “擦除类型”
    template <typename Src>
    function(Src src) : ptr_(new Callable<Src>(src)) { }

    ~function() { 
        if (ptr_) {
            delete ptr_; 
        }
    }

    function(function const& rhs): ptr_(rhs.ptr_->clone()) { }

    function& operator=(function const& rhs) {
        if (&rhs != this) {
            auto ptr = rhs.ptr_->clone();
    
            if (ptr_) {
                delete ptr_;
            }
            ptr_ = ptr;
        }
        return *this;
    }

    function(function&& rhs) noexcept: ptr_(rhs.ptr_) {
        rhs.ptr_ = nullptr;
    }

    function& operator=(function&& rhs) noexcept {
        if (&rhs != this) {
            if (ptr_) {
                delete ptr_;
            }
            ptr_ = rhs.ptr_;
            rhs.ptr_ = nullptr;
        }
        return *this;
    }

    Ret operator()(Arg1 arg1, Arg2 arg2) {
        return ptr_->operator()(arg1, arg2);
    }
private:
    class CallableBase {
    public:
        virtual Ret operator()(Arg1, Arg2) = 0;

        virtual ~CallableBase() = default;

        // 追加用于 复制 的 虚函数
        virtual CallableBase* clone() const = 0;
    };

    template <typename Src>
    class Callable: public CallableBase {
    public:
        Callable(Src src): src_(src) {}

        CallableBase* clone() const
        {
            return new Callable<Src>(src_);
        }

        // 将 返回类型 和 参数 模板化
        Ret operator()(Arg1 x, Arg2 y) override {
            return src_(x, y);
        }
    private:
        Src src_;
    };
private:
    CallableBase* ptr_;
};

// 全局函数
int global_function(int x, int y)
{
    return x + y;
}

float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

int main() {
    
    ::function<int(int, int)> h { BinaryFunction(10) };
    static_assert(sizeof(h) == sizeof(void*));
    assert(h(1, 2) == 13);

    auto g = h;
    assert(g(1, 2) == 13);
    
    return 0;
}
```

# 10. 模板推导：概述

`::function<int(int, int)> h { BinaryFunction(10) };` 需要写明类型，否则就报错；

因为：全局函数推导时候，变成函数指针，`int(*)(int, int)` 并不是 `int(int, int)` 所以需要在处理一次；

在C++ 17，有个 模板推导指南（guide） 的 声明，可以做到，下面是 简化版本，大家看下：

代码抄袭自：gcc 的 源码；

点击 [这里](https://godbolt.org/z/qe9nET1Mb) 运行代码:

```cpp
#include <cassert>
#include <functional>

// 一般模板，啥都没有，调用就是编译报错
template <typename T>
class MyFunction;

template <typename Ret, typename Arg1, typename Arg2>
class MyFunction<Ret(Arg1, Arg2)> {
public:
    template <typename Src>
    MyFunction(Src src) { }

    Ret operator()(Arg1 arg1, Arg2 arg2) { 
        return Ret();
    }
};

// ======================================= 2. 模板推导 辅助函数

template<typename>
struct __member_function { };

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2)> { 
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) &> {
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) const> { 
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) const &> { 
    using type = Res(Arg1, Arg2); 
};

template<typename _Fn, typename _Op>
using __member_function_t = typename __member_function<_Op>::type;

// ======================================= 3. C++ 17 模板推导 guide

// 函数指针，比如 全局函数 等，用这个将指针类型忽略
template<typename Res, typename Arg1, typename Arg2>
MyFunction(Res(*)(Arg1, Arg2)) -> MyFunction<Res(Arg1, Arg2)>;

// 含有operator() 成员函数的：lambda，bind，函数对象
// Signature = Ret(Arg1, Arg2)
template<typename MemberFunction, typename Signature = __member_function_t<MemberFunction, decltype(&MemberFunction::operator())>>
MyFunction(MemberFunction) -> MyFunction<Signature>;

// ======================================= 4. 应用

// 全局函数
float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

class C {
public:
    static int staticFunc(int x, int y) {
        return x + y;
    }

    float f(int x, int y) {
        return float(x) * float(y);
    }
};

int main() {
    // 普通函数
    MyFunction f1 { global_function_float };
    f1(3, 4);

    // 函数对象
    MyFunction f2 { BinaryFunction(10) };
    f2(3, 4);

    // Lambda
    auto g = [](int x, int y ) -> int { return x + y; };
    MyFunction f3 { g };
    f3(3, 4);

    // 静态函数
    MyFunction f4 { C::staticFunc };
    f4(3, 4);

    // 成员函数
    C obj;
    auto wrap = std::bind(&C::f, obj, std::placeholders::_1, std::placeholders::_2);
    // 用bind后还是要指明类型
    MyFunction<float(int, int)> f5 { wrap };
    f5(3, 4);
    
    return 0;
}
```

# 11. 将模板推导结合到 function

结合 09 和 10 的知识点，可知：

点击 [这里](https://godbolt.org/z/P5qjrPzhz) 运行代码:

``` cpp
#include <cassert>
#include <functional>

// 一般模板，啥都没有，调用就是编译报错
template <typename T>
class function;

// 偏特化，将 函数原型写上
template <typename Ret, typename Arg1, typename Arg2>
class function<Ret(Arg1, Arg2)> {
public:
    // 模板化的构造函数，就是上面的 make_callable
    // 用 模板 + 返回基类的指针，去 “擦除类型”
    template <typename Src>
    function(Src src) : ptr_(new Callable<Src>(src)) { }

    ~function() { 
        if (ptr_) {
            delete ptr_; 
        }
    }

    function(function const& rhs): ptr_(rhs.ptr_->clone()) { }

    function& operator=(function const& rhs) {
        if (&rhs != this) {
            auto ptr = rhs.ptr_->clone();
    
            if (ptr_) {
                delete ptr_;
            }
            ptr_ = ptr;
        }
        return *this;
    }

    function(function&& rhs) noexcept: ptr_(rhs.ptr_) {
        rhs.ptr_ = nullptr;
    }

    function& operator=(function&& rhs) noexcept {
        if (&rhs != this) {
            if (ptr_) {
                delete ptr_;
            }
            ptr_ = rhs.ptr_;
            rhs.ptr_ = nullptr;
        }
        return *this;
    }

    Ret operator()(Arg1 arg1, Arg2 arg2) {
        return ptr_->operator()(arg1, arg2);
    }
private:
    class CallableBase {
    public:
        virtual Ret operator()(Arg1, Arg2) = 0;

        virtual ~CallableBase() = default;

        // 追加用于 复制 的 虚函数
        virtual CallableBase* clone() const = 0;
    };

    template <typename Src>
    class Callable: public CallableBase {
    public:
        Callable(Src src): src_(src) {}

        CallableBase* clone() const
        {
            return new Callable<Src>(src_);
        }

        // 将 返回类型 和 参数 模板化
        Ret operator()(Arg1 x, Arg2 y) override {
            return src_(x, y);
        }
    private:
        Src src_;
    };
private:
    CallableBase* ptr_;
};

// ================ 模板推导部分

template<typename>
struct __member_function { };

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2)> { 
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) &> {
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) const> { 
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) const &> { 
    using type = Res(Arg1, Arg2); 
};

template<typename _Fn, typename _Op>
using __member_function_t = typename __member_function<_Op>::type;

// 函数指针，比如 全局函数 等，用这个将指针类型忽略
template<typename Res, typename Arg1, typename Arg2>
function(Res(*)(Arg1, Arg2)) -> function<Res(Arg1, Arg2)>;

// 含有operator() 成员函数的：lambda，bind，函数对象
// Signature = Ret(Arg1, Arg2)
template<typename MemberFunction, typename Signature = __member_function_t<MemberFunction, decltype(&MemberFunction::operator())>>
function(MemberFunction) -> function<Signature>;

// ======================================= 应用

// 全局函数
float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

class C {
public:
    static int staticFunc(int x, int y) {
        return x + y;
    }

    int mem(int x, int y) {
        return x + y;
    }
};

int main() {
    // 普通函数
    ::function f1 { global_function_float };
    assert(f1(3.5f, 2) == 7.0f);

    // 函数对象
    ::function f2 { BinaryFunction(10) };
    assert(f2(3, 4) == 17);

    // Lambda
    auto g = [](int x, int y ) -> int { return x + y; };
    ::function f3 { g };
    assert(f3(3, 4) == 7);

    // 静态函数
    ::function f4 { C::staticFunc };
    assert(f4(3, 4) == 7);

    // 成员函数
    C obj;
    auto wrap = std::bind(&C::mem, obj, std::placeholders::_1, std::placeholders::_2);
    // 用bind后还是要指明类型
    ::function<float(int, int)> f5 { wrap };
    assert(f5(3, 4) == 7);
    
    return 0;
}
```

# 12. `TODO` 性能优化

# 13. `TODO` 变参模板

# 14. `TODO` 完美转发

