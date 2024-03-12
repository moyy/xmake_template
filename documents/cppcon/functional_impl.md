#! https://zhuanlan.zhihu.com/p/685839521
# 第二版：东施效颦 之 `std::function` 的 简化版实现

笔记来源：

+ [Blog: std::function 基本实现](http://blog.bitfoc.us/p/525)
+ [油管: C++ Weekly - Ep 333 - A Simplified std::function Implementation](https://www.youtube.com/watch?v=xJSKk_q25oQ)

导游表：

|章节|知识点|说明|
|--|--|--|
|02节|模板化函数调用||
|03-04节|用虚函数进行类型擦除 / 模板化的工厂模式||
|05-06节|C++ 17 推断指引（`Deduction Guildes`）||
|07节|可变参数 & 完美转发||

# 01. `std::function` 使用

**注：** 本小节代码可到 [Godbolt](https://godbolt.org/z/5ajz94G1K) 运行 / 拷贝；

std::function 可以封装统一的函数调用接口

+ 参数 可以是多个，类型任意；
+ 返回值 类型任意；

1. 全局函数 / 函数指针

``` cpp
int id(int x)
{
    return x;
}

void test_global_function() {
    // std::function<int(int)> f { id };
    std::function f {id};
    assert(f(4) == 4);

    // 函数指针
    int(*id_ptr)(int) = id;
    assert(id_ptr(4) == 4);
    
    // std::function<int(int)> f2 {id_ptr};
    std::function f2 {id_ptr};
    assert(f2(4) == 4);
}
```

2. 函数对象片段:

``` cpp
void test_function_object() {
    struct IdFunc {
        int operator()(int x)
        {
            return x;
        }
    };

    // std::function<int(int)> f { IdFunc() };
    std::function f { IdFunc() };
    assert(f(4) == 4);
}
```

3. lambda表达式片段：

``` cpp
void test_lambda() {
    auto id = [](int x) {return x;};
    
    // std::function<int(int)> f { id };
    std::function f { id };
    assert(f(4) == 4);
}
```

4. 静态成员函数

``` cpp
void test_static_function() {
    struct Foo {
        static int id(int x) {
            return x;
        }
    };

    // std::function<int(int)> f { Foo::id };
    std::function f { Foo::id };
    assert(f(4) == 4);
}
```

5. 成员函数，绑定到 bind

``` cpp
void test_member_bind() {
    struct Foo {
        int id(int x) {
            return x;
        }
    };

    Foo foo;
    auto member = std::bind(&Foo::id, &foo, std::placeholders::_1);
    
    // 注：编译报错 std::function f { member }; 
    std::function<int(int)> f { member };
    assert(f(4) == 4);
}
```

6. 成员函数，直接绑定

``` cpp
void test_member_function() {
    struct Foo {
        int id(int x) {
            return x;
        }
    };

    Foo foo;
    
   // 注：编译报错 std::function f { &Foo::id };
    std::function<int(Foo&, int)> f { &Foo::id };
    assert(f(foo, 4) == 4);
}
```

# 02. 实现功能：模板化的函数对象

**注：** 本小节代码可到 [Godbolt](https://godbolt.org/z/YTGaTbobE) 运行 / 拷贝；

不考虑接口，我们先考虑怎么封装一个对象，能容纳各种 int(int) 原型的函数调用，包括 全局函数 / 静态函数 / 成员函数 / 函数对象 / std::bind / lambda

很容易想到，用 模板化的 函数对象，即可封装所有

注：CallableType 是 可调用 的 类型

``` cpp
template <typename CallableType>
class MyFunction {
public:
    MyFunction(CallableType f): f_(f) {}

    int operator()(int x) {
        // 无论是 全局函数 / 静态函数 / 成员函数
        // 还是 函数对象 / std::bind / lambda
        // 都是可以调用的
        return f_(x);
    }
private:
    CallableType f_;
};
```

# 03. 适配接口: 虚函数擦除类型

**注：** 本小节代码可到 [Godbolt](https://godbolt.org/z/zT89qoazx) 运行 / 拷贝；

上一节实现的缺点是 参数类型 CallableType 是 各顾各的，没有统一的参数；

我们期望声明是 `MyFunction f`，这样可以将f扔到std::vector等容器，不用关心`CallableType`具体是什么。

所以，我们下面用 抽象类`ICallable`，将`MyFunction<CallableType>`实现为`ICallable`的子类，从而 实现`CallalbeType`的类型擦除

抽象类是一个函数对象:

``` cpp
class ICallable {
public:
    virtual ~ICallable() {}

    virtual int operator()(int x)=0;
};
```

将 `Callable<Src>`实现为`ICallable`的子类:

``` cpp
template <typename Src>
class Callable: public ICallable {
public:
    Callable(Src f): f_(f) {}

    int operator()(int x) override {
        return f_(x);
    }
private:
    Src f_;
};
```

然后 MyFunction 实现 `ICallable` 的封装，注：虚函数的“类型擦除”的技巧就是，用基类的 指针/引用 指向 子类对象！

首先是：模板化的构造函数

``` cpp
class MyFunction {
public:
    // 模板构造函数
    // 过了这里，就 擦除类型 Src 了
    template <typename Src>
    MyFunction(Src src) {
        base_ = new Callable(src);
    }

    ~MyFunction() {
        if (base_) {
            delete base_;
        }
    }
private:
    // 基类指针
    ICallable *base_;
};
```

然后，实现 operator() 就很简单了：

``` cpp
class MyFunction {
public:
    // ... 构造 / 析构 代码

    int operator()(int x) {
        return base_->operator()(x);
    };
private:
    // 基类指针
    ICallable *base_;
};
```

使用就跟 `std::function<int(int)>` 没区别:

``` cpp
int id(int x) {
    return x;
}

int main() {
    MyFunction f(id);
    assert(f(4) == 4);
}
```

# 04. 重构：内部类 + Big Five

**注：** 本小节代码可到 [Godbolt](https://godbolt.org/z/KqrGWx5dn) 运行 / 拷贝；

上面一节代码，有两个点需要重构：

1. ICallable / Callable 都是 MyFunction 内部的细节，不应该暴露给外部；

为此，将 ICallable / Callable 实现为 MyFunction的内部类：

``` cpp
class MyFunction {
    // 前置声明
    class ICallable;

    template <typename Src>
    class Callable;
    
    // ...

private:
    // 内部类: 抽象类
    class ICallable {
        // ...
    };

    // 内部类: 具体类型
    template <typename Src>
    class Callable: public ICallable {
        // ...
    };
};
```

2. 根据 Big Five，有了析构函数，就要实现 拷贝 / 移动 相关的函数；

先看 `ICallable` 不好实现拷贝构造，因为不知道具体类型；用 clone代替（Rust有 Clone Trait 可以借鉴）

``` cpp
class ICallable {
public:
    virtual ~ICallable() {}

    // 注意：此节多了这个函数
    virtual ICallable* clone() const = 0;

    virtual int operator()(int x) = 0;
};
```

然后 Callable 的 clone 实现也就很简单：

``` cpp
template <typename Src>
class Callable: public ICallable {
public:
    // ...

    ICallable* clone() const override {
        return new Callable<Src>(*this);
    }
};
```

接下来，MyFunction 的 Big Five，就可以基于clone方法实现了。

下面仅以 拷贝 / 移动 赋值运算符 / 为例，进行代码展示：

拷贝赋值运算符：

``` cpp
// 异常安全的，处理自赋值的 拷贝构造运算符
MyFunction& operator=(const MyFunction& other) {
    // 由于 自赋值 的情况很少见
    // 就算为了性能，也没必要 加 if 了
    auto temp_ptr = other.base_->clone();
 
    std::swap(base_, temp_ptr);
 
    if (temp_ptr) {
        delete temp_ptr;
    }
 
    return *this;
}
```

移动赋值运算符：

``` cpp
// 注：other.base_ 由othter析构函数自行处理就好；
MyFunction& operator=(MyFunction&& other) noexcept {
    std::swap(base_, other.base_);
    return *this;   
}
```

# 05. 模板：单参调用

上面我们有了一个 int(int) 的 调用功能的MyFunction实现了，下面集中精力将它类型化。

分三个小节走：

+ 首先，先将 int(int) 变成 Ret(Arg)
+ 然后。完成 自动推导的 模板实现
+ 最后，实现通用版本的类型 Ret(Args ...)

本节先看 Ret(Arg)，相对来说比较简单；

**注：** 第一版代码，可到 [Godbolt](https://godbolt.org/z/er9YqKn3K) 运行 / 拷贝；

相对上一版本的代码，只需要改动几个地方

1. 给 MyFunction 添加模板参数 Ret，Arg，表示 返回值类型，参数类型

``` cpp
template <typename Ret, typename Arg>
class MyFunction {
    // ...
};
```

2. 将 MyFunction 的 int operator()(int) 接口模板化

``` cpp
template <typename Ret, typename Arg>
class MyFunction {
    // ...

    Ret operator()(Arg x) {
        return base_->operator()(x);
    }; 

    // ...
};
```

3. 将 内部类 Callable 的接口 int operator()(int) 也模板化

``` cpp
template <typename Src>
class Callable: public ICallable {
public:
    // ...

    Ret operator()(Arg x) override {
        return f_(x);
    }
    
    // ...
};
```

4. 应用的地方，现在就变成

``` cpp
float id(float x)
{
    return x;
}

int main() {
    MyFunction<float, float> f {id};
    assert(f(4.2f) == 4.2f);
}
```

这一版本的问题在于模板应用，`MyFunction<float, float>` 不直观看不清楚，哪个是返回值，哪个是参数。

如果要仿照 std::function, 就要改成 `MyFunction<float(float)>`

**注：** 第二版代码，可到 [Godbolt](https://godbolt.org/z/jde4Eefrh) 运行 / 拷贝；

怎么做到特定形式的模板声明，就是 偏特化

+ 主模板：啥都不干
+ 特定模板改为 `MyFunction<Ret(Arg)>`

``` cpp
template <typename T>
class MyFunction;

// 偏特化
template <typename Ret, typename Arg>
class MyFunction<Ret(Arg)> {
    // ...
};
```

调用也变成：

``` cpp
float id(float x)
{
    return x;
}

int main() {
    MyFunction<float(float)> f {id};
    assert(f(4.2f) == 4.2f);
}
```

# 06. 模板：自动类型推导

**注：** 本小节代码，可到 [Godbolt](https://godbolt.org/z/9fqEe917d) 运行 / 拷贝；

上一小节的代码，无法做到自动类型推导；具体来说，就是要显式声明 `MyFunction<float(float)> f {id};`，如果你想这样写 `MyFunction f {id};`就会得到编译器错误如下所示。

``` txt
<source>:error: class template argument deduction failed:
  |     MyFunction f {id};
<source>:error: no matching function for call to 'MyFunction(float (&)(float))'
```

但 id 的声明就说明了这是 float(float) 的函数了啊！

我们先梳理下，总的来说，函数调用常见方式有几种，归为两大类：

+ 函数指针类：普通函数 / 类静态函数 / 类成员函数 可衰变为函数指针
+ 带operator()的函数对象类：函数对象 / lambda表达式 / std::funciton

下面，我准备 参（抄）考（袭） [gcc 的 std::function 的实现](https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/bits/std_function.h)，介绍下 C++ 17 的推断指引（`Deduction Guildes`）声明，解决这个问题。

首先是 函数指针类，重点是让编译器知道 让 Res(*)(Arg) 变成 Res(Arg)。

在实现完 class MyFunction 之后，加上 下面的声明

``` cpp
template <typename T>
class MyFunction;

// 偏特化
template <typename Ret, typename Arg>
class MyFunction<Ret(Arg)> {
    // ...
};

// Deduction Guildes: 发现 函数指针的模板，要推断出返回值和参数类型
template <typename Res, typename Arg>
MyFunction(Res(*)(Arg)) -> MyFunction<Res(Arg)>;
```

调用就可以：

``` cpp
float id(float x)
{
    return x;
}

// 测试 普通函数
void test_global_function() {
    // MyFunction<float(float)> f {id};
    MyFunction f {id};
    assert(f(4.2f) == 4.2f);
}

// 测试 静态成员函数
void test_static_function() {
    struct Foo {
        static int id(int x) {
            return x;
        }
    };

    // MyFunction<int(int)> f { Foo::id };
    MyFunction f { Foo::id };
    assert(f(4) == 4);
}
```

下面我们 来看 函数对象类：函数对象 / lambda表达式 / std::funciton

都可以看成是带有 Ret operator()(Arg) 的 类型；

重点是让 Ret operator()(Arg) 变成 Ret(Arg)

``` cpp
template <typename T>
class MyFunction;

// 偏特化
template <typename Ret, typename Arg>
class MyFunction<Ret(Arg)> {
    // ...
};
```

我们分三段代码，把这个推导解释一下：

下面这一段，是总纲：如果 MyFunction里的参数是 函数对象 FunctionObject，那么就提取 operator() 的原型 OperatorSign 作为 MyFunction的参数；

那么：怎么识别函数对象呢，如果 `MyFunction<T>` 的 T，可以取到 operator() 那么 T就算是函数对象。 

提取operator函数的完整类型，可以用：decltype(),其中 FunctionObject::operator() 要取 指针，变成函数指针之后好赋值。

``` cpp
// 如果 MyFunction里的参数是 函数对象 FunctionObject，那么就提取 operator() 的原型 OperatorSign 作为 MyFunction的参数
template <
    typename FunctionObject, 
    typename OperatorSign = __member_function_t<FunctionObject, decltype(&FunctionObject::operator())>
> MyFunction(FunctionObject) -> MyFunction<OperatorSign>;
```

第二段代码，是一个辅助函数，从 Tp::operator() 函数原型 Op 中，提取出 类型 __member_funciton_t = Ret(Arg)

``` cpp
// Op = Tp::operator() 函数原型
template<typename Fn, typename Op>
using __member_function_t = typename __member_function<Op>::type;
```


第三段代码，是模板元编程的主要部分，Ret, ClassTpe, Arg 都是 Ret operator(Arg) 的 原型拆解；就要提取出 type = Ret(Arg)

模板元编程技术，请参考：[走马观花：从Type Traits看模板元编程](https://zhuanlan.zhihu.com/p/684051862)

还是老套路，主模板啥都没干

4个偏特化模板，都别对象成员函数 operator() 指针的四种类型：

+ Ret(Tp::*)(Arg)
+ Ret(Tp::*)(Arg) const
+ Ret(Tp::*)(Arg) &
    - 不是很清楚啥时候会是这种原型
+ Ret(Tp::*)(Arg) const&
    - 不是很清楚啥时候会是这种原型

``` cpp
// 通用模板，啥都没干
template<typename>
struct __member_function { };

// 偏特化: Ret(Tp::*)(Arg) --> Ret(Arg)
template<typename Ret, typename ClassType, typename Arg>
struct __member_function<Ret (ClassType::*) (Arg)> { 
    using type = Ret(Arg); 
};

// 偏特化: Ret(Tp::*)(Arg)& --> Ret(Arg)
template<typename Ret, typename ClassType, typename Arg>
struct __member_function<Ret (ClassType::*) (Arg) &> {
    using type = Ret(Arg); 
};

// 偏特化: Ret(Tp::*)(Arg) const --> Ret(Arg)
template<typename Ret, typename ClassType, typename Arg>
struct __member_function<Ret (ClassType::*) (Arg) const> { 
    using type = Ret(Arg); 
};

// 偏特化: Ret(Tp::*)(Arg) const& --> Ret(Arg)
template<typename Ret, typename ClassType, typename Arg>
struct __member_function<Ret (ClassType::*) (Arg) const &> { 
    using type = Ret(Arg); 
};
```

然后，应用方面，就可以：

``` cpp

// 函数对象
void test_function_object() {
    struct IdFunc {
        IdFunc() {}

        int operator()(int x)
        {
            return x;
        }
    };

    // MyFunction<int(int)> f { IdFunc() };
    MyFunction f { IdFunc() };
    assert(f(4) == 4);
}


// lambda
void test_lambda() {
    auto id = [](int x) {return x;};

    MyFunction f { id };
    // MyFunction<int(int)> f { id };
    assert(f(4) == 4);
}

// 成员函数，绑定到 bind
void test_member_bind() {
    struct Foo {
        int id(int x) {
            return x;
        }
    };

    Foo foo;
    auto member = std::bind(&Foo::id, &foo, std::placeholders::_1);

    // 编译报错 std::function f { member };    
    // 编译报错 MyFunction f { member };
    MyFunction<int(int)> f { member };
    assert(f(4) == 4);
}
```

`注：` 对 std::bind，就算 std::function 也无法做到自动类型推导，这里我就不尝试了（主要是无法抄袭。。。）

# 07. 模板：可变参数 & 完美转发

**注：** 本小节代码，可到 [Godbolt](https://godbolt.org/z/vo4jf91df) 运行 / 拷贝；

终于到了 最后，我们将 Ret(Arg) 改为 可变参数模板，让它适配任意参数吧。

同时，可以将 std::forward 将完美转发应用到函数调用中，让其更完善，适配左值/右值引用。

首先，修改 MyFunction 的定义：typename ... Args 表示变参模板参数，应用就是 Args ...

``` cpp
template <typename Ret, typename ... Args>
class MyFunction<Ret(Args ...)> {
    // ...
};
```

然后，我们将 operator() 改为 应用变参，同时注意 && 在模板中表示 “万有引用”，配合 std::forward 实现 完美转发，能转发 左值和右值。

``` cpp
template <typename Ret, typename ... Args>
class MyFunction<Ret(Args ...)> {
    // ...

    Ret operator()(Args && ... args) {
        return base_->operator()(std::forward<Args>(args)...);
    }

    // ...
};
```

对基类 ICallable 也要对应修改：

``` cpp
class ICallable {
public:
    // ...    
    virtual Ret operator()(Args && ... args) = 0;
};
```
最后，是派生类的调用：

``` cpp
template <typename Src>
class Callable: public ICallable {
public:
    // ...

    Ret operator()(Args && ... args) override {
        return f_(std::forward<Args>(args)...);
    }
private:
    Src f_;
};
```
