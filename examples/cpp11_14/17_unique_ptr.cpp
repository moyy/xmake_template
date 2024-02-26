#include <memory>
#include <iostream>

struct A {
    int m_a;

    explicit A(int a) {
        m_a = a;

        std::cout << "A::A(" << a << ")" << std::endl;
    }
    ~A() {
        std::cout << "A::~A(), m_a = " << m_a << std::endl;
    }

    void print() {
        std::cout << "A::print(" << m_a << ")" << std::endl;
    }
};


int main() {
    {
        auto pa = new A(1);
        std::unique_ptr<A> p(pa);
    }
    
    {
        // C++ 14
        auto p = std::make_unique<A>(2);
    }

    {
        // 使用自定义删除器
        auto customDeleter = [](A* p) { 
            std::cout << "Custom delete" << std::endl; 
            delete p;
        };
        std::unique_ptr<A, decltype(customDeleter)> p(new A(3), customDeleter);

        // 编译 error, 不可 移动
        // std::unique_ptr<A> tem = p3; 
        
        p->print();
    }

    {
        auto p = std::make_unique<A>(4);
        
        auto ptr = p.get(); // A* ptr
    }

    {
        auto p = std::make_unique<A>(5);
        p.reset();
    }
        
    {
        auto p = std::make_unique<A>(6);
        auto ptr = p.release();
        delete ptr;
    }

    {
        auto p1 = std::make_unique<A>(7);
        auto p2 = std::make_unique<A>(8);
        
        std::cout << "before swap: " << p1->m_a << ", " << p2->m_a << std::endl;
        p1.swap(p2); // 或 std::swap(p1, p2);
        std::cout << "after swap: " << p1->m_a << ", " << p2->m_a << std::endl;
    }

    return 0;
}