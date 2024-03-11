#! https://zhuanlan.zhihu.com/p/685839521
# 邯郸学步 之 `std::function` 的 简化版实现 v2

笔记来源：

+ [Blog: std::function 基本实现](http://blog.bitfoc.us/p/525)
+ [油管: C++ Weekly - Ep 333 - A Simplified std::function Implementation](https://www.youtube.com/watch?v=xJSKk_q25oQ)

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

# 05. 性能优化

**注：** 本小节代码可到 [Godbolt](https://godbolt.org/z/G79785Wja) 运行 / 拷贝；

上面的代码，MyFunction 存储了 基类的指针，其子类有个虚表指针，里面每个类型Src都对应有三个函数地址项：~ICallable, clone, operator()

空间方面：对于一个确定的类型Src而言，MyFunction就占用了 `5 * sizeof(void*)`的空间，虽然 sizeof(MyFunction) 等于 sizeof(void*);

时间方面：调用一次 MyFunction 的 operator()，相当于 调用 一次 base_->operator()，其实现跳到虚表的第3项进行调用，最后调用 Src 的 operator()；

如果是一些简单的函数对象，可能 Src 的大小比一个`sizeof(void*)`还小，所以一个Src*可以装很多东西了。

# 06. 模板：单参调用

# 07. 模板：自动类型推导

# 08. 模板：可变参数 & 完美转发