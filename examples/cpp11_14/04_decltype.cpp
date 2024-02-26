int func() { 
    return 0; 
}

void test_decltype() {
    decltype(func()) i; // int i;

    int x = 0;
    decltype(x) y;      // int y
    decltype(x + y) z;  // int z 
}

// decltype 会保留 表达式的 引用和C/V属性
void test_decltype_cv() {
    const int &i = 1;
    int a = 2;
    decltype(i) b = 2; // const int& b
}

// C++ 11 模板约束 应用
// 值后置类型 语法 为了解决 函数返回制类型依赖于参数, 却难以确定返回值类型的问题。
template<typename T, typename U>
auto add11(T t, U u) -> decltype(t + u) {
    return t + u;
}

// C++ 14, 简化成了 deduced return type
template<typename T, typename U>
auto add14(T t, U u) {
    return t + u;
}

int main() {
    test_decltype();

    test_decltype_cv();

    double d = add11(1, 2.0);
    d = add14(1, 2.0);
}
