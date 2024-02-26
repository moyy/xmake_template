// C++17 内联变量
// 用 内联变量，类的静态成员变量 在头文件中 可以 初始化

struct A {
    inline static const int value = 10;
};

int main() {
    return 0;
}