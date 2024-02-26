// 重载：overload
//    同类 同名不同参
// 覆盖：override（运行时 多态 基础）
//    对 虚函数，子类 同名同参，基类 被覆盖
// 重定义 / 隐藏：redefining
//    对 非虚函数，子类 同名，基类函数 被隐藏
//    对   虚函数，子类 同名不同参，基类函数 被隐藏
 

#include <string>
#include <iostream>

class Base {
private:
    virtual void display() {
        std::cout << "Base::display()" << std::endl;
    }

    void say() {
        std::cout << "Base::say()" << std::endl;
    }
public:
    void exec() {
        say();
        display();
    }   

    void foo(int a) {
        std::cout << "Base::foo(int a)" << std::endl;
    }

    // overload, 方法重载
    void foo(double a) {
        std::cout << "Base::foo(double a)" << std::endl;
    }
};

class Derived : public Base {
public:
    // override
    void display() {
        std::cout << "override Derived::display()" << std::endl;
    }

    // redifining
    void foo(std::string s) {
        std::cout << "redefining Derived::foo(" << s << ")" << std::endl;
    }

    // redifining
    void say() {
        std::cout << "redifining Derived::say()" << std::endl;
    }
};

int main() {
    Derived d;
    Base *pb = &d;

    // Base::say()
    // override Derived::display()
    pb->exec();

    // Base::say()
    // override Derived::display()
    d.exec();
    
    // redifining Derived::say()
    d.say();

    // d.foo(10.0); // 编译错误
    // redefining Derived::foo(abc)
    d.foo("abc");

    Base b;
    
    // Base::say()
    // Base::display()
    b.exec();
    
    // Base::foo(int a)
    b.foo(10);

    return 0;
}