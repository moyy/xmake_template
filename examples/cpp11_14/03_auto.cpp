#include <vector>

class A {
    // auto a = 1; // error，在类中, auto不能 用作 非静态成员变量
    inline static const auto c = 5.0; // ok
};

// C++ 后面可以 将 auto 用作 参数 和 返回值
auto f(auto a) { 
    return a; 
}

// + auto 必须马上初始化
// + 在类中, auto 不能用作 非静态成员变量
// + auto 不能定义 数组，可以定义指针
// + auto 无法推导 模板参数
void test_auto() {
    auto ra = f(4); // int ra;
    auto rb = f(4.8); // double rb;

    // lambda 参数 auto，C++ 14 支持
    auto lf = [] (auto a) { return a; };
    auto la = lf(4);   // int la;
    auto lb = lf(4.2); // double lb;

    int a[10] {0};
    auto b = a; // int *b;

    // auto c[10] = 0; // error，auto不能定义数组，可以定义指针

    std::vector<int> d;
    // std::vector<auto> f = d; // error，auto无法推导出模板参数
}

// CV 指 const / volatile
// 不声明 引用 或 指针，auto 忽略右边的 引用 和 CV限定
//   声明 引用 或 指针，auto 会 保留 右边的 引用和CV限定
void test_cv_auto(void) {
    int i = 0;
    auto *a = &i; // int *a 
    auto &b = i;  // int& b
    auto c = b;   // int c

    const auto d = i; // const int d
    auto e = d;       // int e

    const auto& f = e; // const int &f
    
    auto &g = f;       // 注意：const int &g
}   

int main() {

    test_auto();

    test_cv_auto();

    return 0;
}