// + 模板 的 右尖括号 可以 连写了 >>
// + 模板 别名
// + 函数模板 的 默认模板参数

#include <vector>
#include <iostream>

// ============= 类模板 和 函数模板 默认参数

template <typename T, typename U=int>
class A {
    T value;  
};

template <typename R, typename U=int>
R func1(U val) {
    return val;
}

template <typename R=int, typename U>
R func2(U val) {
    return val;
}

// ============== 变长模板

template <typename T>
void func(const T& t){
    std::cout << t << std::endl;
}

template <typename T, typename ... Args>
void func(const T& t, Args ... args){
    std::cout << t << ',';
    func(args...);
}

// ============= C++ 14 变量模板

// pi<int>   // 3
// pi<float> // 3.141592
template<class T>
constexpr T pi = T(3.1415926535897932385L);


int main() {
    // c++ 11 之前, >> 被视作 右移
    std::vector<std::vector<int>> a; // error
    
    // ========== 模板别名: using 名 = 

    // before c++11
    typedef std::vector<std::vector<int>> vvi;
    
    // c++11 
    using vvi = std::vector<std::vector<int>>;
    
    return 0;
}