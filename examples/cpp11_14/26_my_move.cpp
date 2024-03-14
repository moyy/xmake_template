#include <utility>

template <typename T>
struct my_remove_reference {
    using type = T;
};

template <typename T>
struct my_remove_reference<T&> {
    using type = T;
};

template <typename T>
struct my_remove_reference<T&&> {
    using type = T;
};

template <typename T>
using my_remove_reference_t = typename my_remove_reference<T>::type;

template <class _Ty>
constexpr my_remove_reference<_Ty>&& my_move(_Ty&& _Arg) noexcept {
    return static_cast<my_remove_reference_t<_Ty>&&>(_Arg);
}

template <class>
constexpr bool is_lvalue_reference_v = false;

template <class _Ty>
constexpr bool is_lvalue_reference_v<_Ty&> = true;

// 如果不调用 forward，可以转发左值，但无法转发右值，因为右值变量的使用也被看成左值？

template <class _Ty>
constexpr my_remove_reference_t<_Ty>& my_forward(my_remove_reference_t<_Ty>& _Arg) noexcept {
    return static_cast<my_remove_reference_t<_Ty> &>(_Arg);
}

template <class _Ty>
constexpr my_remove_reference_t<_Ty>&& my_forward(my_remove_reference_t<_Ty>&& _Arg) noexcept {
    // 调用到这里的 类型 都不是 左值
    static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
     
    return static_cast<my_remove_reference_t<_Ty>&&>(_Arg);
}

int main() {
    int a = 4;    
    auto r = std::move(a);

    return 0;
}