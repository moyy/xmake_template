// std::weak_ptr();
// 
// + 作用1：返回this指针，上面介绍的shared_from_this()其实就是通过weak_ptr返回的this指针
// + 作用2：循环引用；

#include <memory>
#include <iostream>

struct A;
struct B;

struct A {
    std::shared_ptr<B> bptr;
    ~A() {
    std::cout << "A::~()" << std::endl;
    }
    void print() {
        std::cout << "A::print()" << std::endl;
    }
};

struct B {
    std::weak_ptr<A> aptr; 

    ~B() {
        std::cout << "~B()" << std::endl;
    }
    void print() {
        if (!aptr.expired()) { // 监视shared_ptr的生命周期
            auto ptr = aptr.lock(); // shared_ptr<A> ptr;
            ptr->print();
        }
    }
};

int main() {
    auto aaptr = std::make_shared<A>();
    auto bbptr = std::make_shared<B>();
 
    aaptr->bptr = bbptr;
    bbptr->aptr = aaptr;
 
    bbptr->print();
 
    return 0;
}