// C++ 14 开始，constexpr 可以有 分支，循环
constexpr int fibonacci(const int n) {
    if(n == 1) return 1;
    if(n == 2) return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

int compute(int n) {
    return fibonacci(n);
}

int main() {
    // C++ 17 支持 consntexpr lambda
    constexpr auto lamb = [] (int n) { return n * n; };
    static_assert(lamb(3) == 9, "a");

    // 编译时 就会求值
    constexpr int r = fibonacci(10);
    static_assert(r == 55);

    // 可能会运行时求值
    int r1 = compute(10);

    // char arr[r1];  // 编译不过
    char arr[r];      // 合法

    return 0;
}