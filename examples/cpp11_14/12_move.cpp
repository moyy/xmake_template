// 左值, 左值引用
// 右值, 纯右值, 将亡值
// 右值引用, 移动语义

// + 左值
//      - 可以 放到等号左边
//      - 可以 取地址 且 有名字
// + 右值
//      - 不可以 放到 等号左边
//      - 不能 取地址
//      - 不具名 对象
// + 纯右值
//      - 除 字符串字面值 外 的 字面值
//      - 返回 非引用类型 的 函数调用
//      - 后置 自增 自减 表达式 i++、i--
//      - 算术表达式 (a+b, a*b, a&&b, a==b等)
//      - 取地址表达式 (&a)
// + 将亡值: C++ 11 新增,与 右值引用相关,可理解成即将销毁的值
//      - 将要被移动的对象
//      - T&& 函数 返回值
//      - std::move 函数返回值
// + 注意
//      - const 引用 可以 指向 右值

#include <memory>
#include <utility>
#include <iostream>

void test_lvalue_ref() {
    int a = 5;
    int &b = a; // b是左值引用

    b = 4;
    
    // int &c = 10; // error，10无法取地址，无法进行引用
    const int &d = 10; // 注意: const 引用 可以指向右值
}

void test_rvalue_ref() {
    int a = 4;

    // int &&b = a; // error, a是左值

    // c 是 右值
    int &&c = std::move(a); // ok
}

class A {
public:
    static A create(unsigned int num) {
        return A(num);
    }

    explicit A() {
        std::cout << "A()" << std::endl;
        m_num = 0;
        m_p = nullptr;
    }

    explicit A(unsigned int num) {
        std::cout << "A(unsigned int num)" << std::endl;
        m_num = num;
        if (num == 0) {
            m_p = nullptr;
        } else {
            m_p = new int[num];
            std::memset(m_p, 0, sizeof(m_p[0]) * num);
        }
    }

    ~A() {
        std::cout << "~A()" << std::endl;
        if (m_p != nullptr) {
            delete[] m_p;
            m_num = 0;
            m_p = nullptr;
        }
    }

    // 拷贝构造函数
    A(const A &a) {
        std::cout << "A(const A &a)" << std::endl;
    
        m_num = a.m_num;
        if (m_num == 0) {
            m_p = nullptr;
        } else {
            m_p = new int[m_num];
            std::memcpy(m_p, a.m_p, sizeof(m_p[0]) * m_num);
        }
    }
    
    // 拷贝赋值运算
    A& operator=(const A &a) {
        std::cout << "A& operator=(const A &a)" << std::endl;
        
        // 引用一样, 返回
        if (this == &a) {
            return *this;
        }

        // 释放 原数据
        if (m_p != nullptr) {
            delete[] m_p;        
        }

        m_num = a.m_num;
        
        if (m_num == 0) {
            m_p = nullptr;
        } else {
            m_p = new int[m_num];
            std::memcpy(m_p, a.m_p, sizeof(m_p[0]) * m_num);
        }

        return *this;
    }

    // 移动构造函数
    A(A &&a) {
        std::cout << "A(A &&a)" << std::endl;
    
        m_num = a.m_num;
        m_p = a.m_p;

        a.m_num = 0;
        a.m_p = nullptr;
    }

    // 移动赋值运算
    A& operator=(A &&a) {
        std::cout << "A& operator=(A &&a)" << std::endl;

        // 引用一样, 返回
        if (this == &a) {
            return *this;
        }

        // 释放 原数据
        if (m_p != nullptr) {
            delete[] m_p;
        }

        m_num = a.m_num;
        m_p = a.m_p;
        
        a.m_num = 0;
        a.m_p = nullptr;

        return *this;
    }

    int* data() const {
        return m_p;
    }

    int size() const {
        return m_num;
    }
private:
    unsigned int m_num;
    int *m_p;
};

template <typename T>
void swap(T &a, T &b) {
    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

// 模板实例声明
template void swap(int &a, int &b);

template void swap(A &a, A &b);

// 在 GCC / Clang 用 -fno-elide-constructors 可以 观察到 拷贝/移动构造函数 的 调用。
// MSVC 2022 直接就是 RVO NRVO 优化
void pass_by_value(const A a) {
}

class Base {
public:
    Base() =default;
    ~Base() =default;

    Base(const Base &b): m_a(A()) {
        std::cout << "Base(const Base &b)" << std::endl;
    }

    Base(Base &&b): m_a(std::move(b.m_a)) {
        std::cout << "Base(Base &&b)" << std::endl;
    }

    Base& operator=(const Base &b) {
        std::cout << "Base& operator=(const Base &b)" << std::endl;
        
        if (this != &b) {
            m_a = b.m_a;
        }

        return *this;
    }

    Base& operator=(Base &&b) {
        std::cout << "Base& operator=(Base &&b)" << std::endl;
        
        if (this != &b) {
            m_a = std::move(b.m_a);
        }
        return *this;
    }
private:
    A m_a;
};

class Derived : public Base {
public:
    using Base::Base;

    Derived(const Derived &d): Base(d) {
        std::cout << "Derived(const Derived &d)" << std::endl;
    }

    Derived(Derived &&d): Base(std::move(d)) {
        std::cout << "Derived(Derived &&d)" << std::endl;   
    }

    Derived& operator=(const Derived &d) {
        std::cout << "Derived& operator=(const Derived &d)" << std::endl;
        
        if (this != &d) {
            Base::operator=(d);
        }
        return *this;
    }

    Derived& operator=(Derived &&d) {
        std::cout << "Derived& operator=(Derived &&d)" << std::endl;
        
        if (this != &d) {
            Base::operator=(std::move(d));
        }
        return *this;
    }
};

// 推荐: 触发 RVO, 一次构造,一次析构
// 使用 A a = test_rvo_1();
A test_rvo_1() {
    A tmp(100);
    return tmp;
}

// 不推荐
// RVO 失效, 触发移动构造, 两次析构
A test_rvo_2(void) {
    A tmp(100);
    return std::move(tmp);
}

// 错误用法, 不能返回 局部变量的引用
A&& test_rvo_3(void) {
    A tmp(100);
    return std::move(tmp);
}

void pass_by_ref(A &a) {
    std::cout << "pass_by_ref(A &a)" << std::endl;
}

void pass_by_ref(A &&a) {
    std::cout << "pass_by_ref(A &&a)" << std::endl;
}

int main() {
    test_lvalue_ref();

    test_rvalue_ref();

    std::cout << "================= test pass_by_value: " << std::endl;

    pass_by_value(A::create(100));   

    std::cout << "================= test swap: " << std::endl;
    
    auto a = A::create(100);
    auto b = A::create(200);

    std::cout << "before swap, a.m_num = " << a.size() << ", b.m_num = " << b.size() << std::endl;
    swap(a, b);
    std::cout << "after swap, a.m_num = " << a.size() << ", b.m_num = " << b.size() << std::endl;
    
    {
        std::cout << "=================== A a1 = test_rvo_1" << std::endl;
        A a1 = test_rvo_1();
        pass_by_ref(a1);  // a1 左值, 打印: pass_by_ref(A &a)
    }
    
    {
        std::cout << "=================== A&& a2 = test_rvo_1" << std::endl;

        // 不推荐, 没必要
        A&& a2 = test_rvo_1();

        pass_by_ref(a2); // 注意: a2 也 是 左值, 打印: pass_by_ref(A &a)
    }

    {    
        std::cout << "=================== A a3 = test_rvo_2" << std::endl;
        // 触发移动构造语义
        A a3 = test_rvo_2();
    }   

    {
        std::cout << "=================== A&& a4 = test_rvo_3" << std::endl;
        
        // 注意: 行为未定义,连续调用三次 ~A
        A&& a4 = test_rvo_3();
    }

    return 0;
}

