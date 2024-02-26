// C++ 11 OOP 新特性
// 
// + expolicit
// + 继承构造 函数
// + 委托构造 函数
// + 移动构造 & 移动赋值
// + override & final
// + =defualt & =delete

#include <iostream>

class Base {
public:
    Base(): m_a(0), m_b(0) {}

    // explicit 用法
    explicit Base(int a): m_a(a) {}

    // 委托构造 
    Base(int a, int b): Base(a) {
        m_b = b;
    }

    virtual void foo() {
        std::cout << "Base::foo()" << std::endl;
    }
private:
    int m_a;
    int m_b;
};

// final 用法
class Derived final: Base {
public:
    // 继承构造函数
    using Base::Base; 

    // 用 =delete 禁止生成 拷贝构造 和 拷贝赋值
    Derived(const Derived& other) = delete;
    Derived& operator=(const Derived& other) = delete;

    // override 用法
    void foo() override {
        std::cout << "Derived::foo()" << std::endl;
    }
};

// 编译报错
// class E: Derived {}

class A {
public:
    friend std::ostream& operator<< (std::ostream& os, const A& a);

    A()=default;

    // 一旦声明为 explicit，就不能用 = 初始化 A
    explicit A(int a);

    A& get_this() {
        std::cout << "A::get_this()" << std::endl;
        return *this;
    }

    const A& get_this() const {
        std::cout << "A::get_this() const" << std::endl;
        return *this;
    }

    int& get_a() {
        return a;
    }

    const int& get_a() const {
        return a;
    }

private:
    int a = 10;
    int b = 100;
};

A::A(int a): a(a) {
    
}

// 必须声明是友元，否则就要 曝露 a b 的 访问属性
// 输出运算符 重载
std::ostream& operator<< (std::ostream& os, const A& a) {
    os << "A::a = " << a.a << ", A::b = " << a.b;
    return os;
}

int main() {
    Derived d(1, 2);
    d.foo();

    A a;
    // A::a = 10, A::b = 100
    std::cout << a << std::endl;
    
    A b(20);
    // A::a = 20, A::b = 100
    std::cout << b << std::endl;

    A c(30);
    A d2 {40};
    // A e = 50;    // 编译错误，因为 explicit 
    // A f = {60};  // 编译错误，因为 explicit

    A g {70};
    // A::get_this()
    g.get_this();

    const A h {80};
    // A::get_this() const
    h.get_this();

    auto aa = h.get_a();

    return 0;
}

