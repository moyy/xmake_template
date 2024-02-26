#include <memory>
#include <iostream>

struct A {
    int m_a;

    A(int a) {
        m_a = a;
        std::cout << "A(" << a << ")" << std::endl;
    }

    ~A() {
        std::cout << "~A(), m_a = " << m_a << std::endl;
    }

    void print() {
        std::cout << "A::print(), m_a = " << m_a << std::endl;
    }
};

void foo(std::shared_ptr<A> ptr) {
    ptr->print();
}

// + 不要用 裸指针 初始化 多个shared_ptr
// + 通过 shared_from_this() 返回this，不要把this指针作为shared_ptr返回出来，因为this指针本质就是裸指针，通过this返回可能 会导致重复析构，不能把this指针交给智能指针管理。
// + 尽量 用make_shared，少用 new。
// + 不要delete get()返回 的 裸指针。
// + 不是 new出来的空间 要自定义 删除器。
// + 要 避免 循环引用，使用 std::weak_ptr 解开
int main() {
    {
        auto sp = std::make_shared<A>(1);
        std::cout << "ref count = " << sp.use_count() << std::endl; // ref count = 1
        
        auto p = sp; // ref +1
        p->print();
        std::cout << "ref count = " << sp.use_count() << std::endl; // ref count = 2

        foo(sp);
        sp->print();
        std::cout << "ref count = " << sp.use_count() << std::endl; // ref count = 2
    }

    {
        auto sp = std::make_shared<A>(2);

        A *p = sp.get(); // 可以通过get获取裸指针
        p->print();
    }    
    
    {
        auto sp = std::make_shared<A>(3);
        sp.reset(); 
        std::cout << "ref count = " << sp.use_count() << std::endl; // ref count = 0
    }

    {
        auto p1 = std::make_shared<A>(4);
        auto p2 = std::make_shared<A>(5);
        
        std::cout << "before swap: " << p1->m_a << ", " << p2->m_a << std::endl;
        p1.swap(p2); // 或 std::swap(p1, p2);
        std::cout << "after swap: " << p1->m_a << ", " << p2->m_a << std::endl;
    }
    
    // VS2017+ 修改 /Zc:__cplusplus
    
    // 仅对 MSVC 编译器添加 /Zc:__cplusplus 选项
    // if is_plat("windows") then
    //     add_cxflags("/Zc:__cplusplus", {force = true})
    // end

    {
        // #if __cplusplus >= 202002L
        #if __cplusplus >= 201703L
            std::cout << "C++ 17, __cplusplus = " << __cplusplus << std::endl;
            // C++17 或更高版本的代码
            std::shared_ptr<int[]> sp(new int[10]);
            for (size_t i = 0; i != 10; ++i) {
                sp[i] = i;
            }
        #elif __cplusplus >= 201103L
            std::cout << "C++ 11, __cplusplus = " << __cplusplus << std::endl;

            // C++11 代码
            std::shared_ptr<int> sp(new int[10], [](int *p) { delete[] p; });
            for (size_t i = 0; i != 10; ++i) {
                *(sp.get() + i) = i;
            }
        #else
            #error "This code requires at least C++11."
        #endif
    }

    return 0;
}
