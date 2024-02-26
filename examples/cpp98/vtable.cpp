// [虚函数表](https://blog.csdn.net/haoel/article/details/1948051)
// 如果 类有虚函数继承，则 该类的对象的第一个 指针 指向虚函数表

#include <iostream>

class Base {
    virtual void f() {
        std::cout << "    Base::f" << std::endl;
    }
    
    virtual void g() {
        std::cout << "    Base::g" << std::endl;
    }

    virtual void h() {
        std::cout << "    Base::h" << std::endl;
    }
};

class Derived : public Base {

    // overide，替代 基类 虚表的 g 位置
    void g() {
        std::cout << "    override Derived::g" << std::endl;
    }
    
    virtual void f1() {
        std::cout << "    Derived::f1" << std::endl;
    }

    // h1 不是 虚函数，不进入虚表
    void h1() {
        std::cout << "    Derived::h1" << std::endl;
    }
};

typedef void(*Fun)(void);

void test_base_vtable() {
    Base b;
    Base *pb = &b;
    
    // 只有 虚表指针，大小就是 sizeof(size_t)
    std::cout << "    size of Base = " << sizeof(b) << std::endl;

    // 虚表，就是 对象 的 首地址
    // 对象的 剩下对象 放 成员
    size_t **arr = (size_t**)pb;
 
    // vptr = [&Base::f, &Base::g, &Base::h]
    size_t *vptr = arr[0];

    // 调用 虚函数：Base::h()
    Fun f = (Fun)vptr[2];
    f();
}


void test_derived_vtable() {
    Derived d;
    Base *pd = &d;
    
    // 虚表，就是 对象 的 首地址
    // 对象的 剩下对象 放 成员
    size_t **arr = (size_t**)pd;

    // vptr = [&Base::f, &Derived::g, &Base::h, &Derived::f1]
    size_t *vptr = arr[0];

    // 调用 Derived::g()
    Fun f = (Fun)vptr[1];
    f();
} 

class Base1 {
    virtual void f() {
        std::cout << "    Base1::f" << std::endl;
    }
    
    virtual void g() {
        std::cout << "    Base1::g" << std::endl;
    }
};

class Base2 {
    virtual void f() {
        std::cout << "    Base2::f" << std::endl;
    }
    
    virtual void h() {
        std::cout << "    Base2::h" << std::endl;
    }
};

class Derived2 : public Base1, public Base2 {
    void f() {
        std::cout << "    override Derived2::f" << std::endl;
    }

    // 普通函数，虚表没有
    void a() {
        std::cout << "    Derived2::a" << std::endl;
    }

    // 子类的 虚函数 被放到了 第一个父类的表中（按照声明顺序）
    virtual void d() {
        std::cout << "    Derived2::d" << std::endl;
    }
};

void test_derived2_vtable() {
    Derived2 d;
    
    // arr = [Base1虚表, Base2虚表]
    size_t **arr = (size_t**)&d;

    // vptrBase1 = [&Derived2::f, &Base1::g, &Derived::d]
    size_t *vptrBase1 = arr[0];

    // vptrBase2 = [&Derived2::f, &Base2::h]
    size_t *vptrBase2 = arr[1];

    // 调用 Derived2::d()
    Fun f = (Fun)vptrBase1[2];
    f();

    // 调用 Base2::h()
    f = (Fun)vptrBase2[1];
    f();
} 

int main() {

    std::cout << "test_base_vtable: " << std::endl;
    test_base_vtable();

    std::cout << std::endl;
    
    std::cout << "test_derived_vtable: " << std::endl;
    test_derived_vtable();

    std::cout << std::endl;
    
    std::cout << "test_derived2_vtable: " << std::endl;
    test_derived2_vtable();
    
    return 0;
}