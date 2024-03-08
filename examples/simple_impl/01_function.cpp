#include <cassert>
#include <functional>

// 一般模板，啥都没有，调用就是编译报错
template <typename T>
class MyFunction;

template <typename Ret, typename Arg1, typename Arg2>
class MyFunction<Ret(Arg1, Arg2)> {
public:
    template <typename Src>
    MyFunction(Src src) { }

    Ret operator()(Arg1 arg1, Arg2 arg2) { 
        return Ret();
    }
};

// ======================================= 2. 模板推导 辅助函数

template<typename>
struct __member_function { };

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2)> { 
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) &> {
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) const> { 
    using type = Res(Arg1, Arg2); 
};

template<typename Res, typename _Tp, typename Arg1, typename Arg2>
struct __member_function<Res (_Tp::*) (Arg1, Arg2) const &> { 
    using type = Res(Arg1, Arg2); 
};

template<typename _Fn, typename _Op>
using __member_function_t = typename __member_function<_Op>::type;

// ======================================= 3. C++ 17 模板推导 guide

// 函数指针，比如 全局函数 等，用这个将指针类型忽略
template<typename Res, typename Arg1, typename Arg2>
MyFunction(Res(*)(Arg1, Arg2)) -> MyFunction<Res(Arg1, Arg2)>;

// 含有operator() 成员函数的：lambda，bind，函数对象
// Signature = Ret(Arg1, Arg2)
template<typename MemberFunction, typename Signature = __member_function_t<MemberFunction, decltype(&MemberFunction::operator())>>
MyFunction(MemberFunction) -> MyFunction<Signature>;

// ======================================= 4. 应用

// 全局函数
float global_function_float(float x, int y) {
    return x * float(y);
}

struct BinaryFunction {
    BinaryFunction(int v): v_(v) {}

    int operator()(int x, int y) {
        return v_ + x + y;
    }

    int v_;
};

class C {
public:
    static int staticFunc(int x, int y) {
        return x + y;
    }

    float f(int x, int y) {
        return float(x) * float(y);
    }
};

int main() {
    // 普通函数
    MyFunction f1 { global_function_float };
    f1(3, 4);

    // 函数对象
    MyFunction f2 { BinaryFunction(10) };
    f2(3, 4);

    // Lambda
    auto g = [](int x, int y ) -> int { return x + y; };
    MyFunction f3 { g };
    f3(3, 4);

    // 静态函数
    MyFunction f4 { C::staticFunc };
    f4(3, 4);

    // 成员函数
    C obj;
    auto wrap = std::bind(&C::f, obj, std::placeholders::_1, std::placeholders::_2);
    // 用bind后还是要指明类型
    MyFunction<float(int, int)> f5 { wrap };
    f5(3, 4);
    
    return 0;
}