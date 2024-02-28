# 01. 函数模板

# 1. 函数模板初相识

## 1.1. 定义模板

+ 下面片段，T 需要 实现 < 符号
+ C++ 17 之后，T 不需要 满足 拷贝 / 移动
+ `typename` 可以 换成 `class`，但建议只用 `typename`
+ ::max(...) 在 全局作用域找 max函数
+ 具体类型 替换 模板参数 的 过程 称为 `模板实例化`，生成 `模板实例`

点击 [这里](https://godbolt.org/z/aranf6Yvz) 运行

``` c++
#include <iostream>

// 返回 两个参数 中的 大者
template <typename T>
T max(T a, T b) {
    // Equivalence 要求 满足 <，相等 为 !(a < b) && !(b < a)
    // Equality    要求 满足 ==，相等 定义为 a == b。
    
    // C++ 标准规定 Returns the first argument when the arguments are equivalent.
    // 见：https://www.zhihu.com/question/266917342
    return b < a ? a : b;
    // return a < b ? b : a; //
}

int main()
{
    int i = 42;
    
    // 实例化 int max(int a, int b);
    // max(7,i): 42
    std::cout << "max(7,i): " << ::max(7,i) << std::endl; 

    double f1 = 3.4;
    double f2 = -6.7;

    // 实例化 double max(double a, double b);
    // max(f1,f2): 3.4
    std::cout << "max(f1,f2): " << ::max(f1,f2) << std::endl; 

    std::string s1 = "mathematics";
    std::string s2 = "math";
   
    // 实例化 std::string max(std::string a, std::string b);
    // max(s1,s2): mathematics
    std::cout << "max(s1,s2): " << ::max(s1,s2) << std::endl; 

    return 0;
}
```

## 1.2. 模板的两阶段编译 

+ 若在定义时不进行实例化，则会 忽略模板参数 去 检查模板代码的正确性
  - 语法错误，如: 缺少分号。
  - 使用 未知名 的 模板参数 (类型名、函数名......)。
  - 检查 不依赖于模板参数 的 静态断言
+ 实例化时，再次检查模板代码，确保生成的代码有效，如 T对应的具体类型是否满足 < 运算符

注意：有些编译器，在定义处不会进行语法检查，全部弄到实例化再检查！

点击 [这里](https://godbolt.org/z/6h1Psvv9P) 查看编译错误

```c++
#include <iostream>

template <typename T>
T foo(T a) {
    // gcc 13.2 中，即使没有实例化模板，也会编译报错；
    t = a
    
    return t;
}

int main() {
    // std::cout << foo(0) << std::endl;
    return 0;
}
```