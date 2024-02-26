// 列表初始化
// 可以防止类型窄化，避免精度丢失的隐式类型转换

#include <vector>
#include <iostream>

std::vector<int> func() {
    // 列表初始化 用于 返回值
    return {};
}

// 自定义 列表初始化
struct CustomVec {
    std::vector<int> data;
    
    // 接收 列表初始化 用 std::initializer_list<T>
    CustomVec(std::initializer_list<int> list) {
        for (auto &elem: list) {
            data.push_back(elem);
        }
    }
};

class Base {
    virtual void f() {}
};

class D: public Base {
public:
    int a;
    int b;

    explicit D(double) { 
        std::cout << "D(double)" << std::endl;
    }

    D(int) { }
    D(int, int) { }
    
    D(const D&) =delete;
private:
    int c;
};

int main() {
    int a = 123;
    D b = 1; 

    D c2 = 2.3;   // 注: 隐式 D(int) , 很危险
    D c1 {2.3};   // 显示 D(double)
    // D c3 = {2.3}; // 编译不过, 隐式 D(double) 

    D c {12};
    D d {1, 2};

    int e = { 123 };
    D f = { 123 };
    // D f1 = f; // 编译不过, 调用 拷贝构造, =delete 了
    
    D f2 {12};
    f2 = f;   // 调用 拷贝赋值

    int g {123}; // c++11
    D h { 123 }; // c++11

    D as[] = {{123}, {456}};

    CustomVec cv {1, 2, 3, 4, 5};

    return 0;
}