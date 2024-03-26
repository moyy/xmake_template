#include <string>
#include <type_traits>

template <typename T>
void foo (T arg) { 
    static_assert(std::is_same_v<T, std::string>);
}

template <typename T>
void foo2 (T arg) { 
     static_assert(std::is_same_v<T, char const*>);
}

template <typename T>
void foo3 (T arg) { 
     static_assert(std::is_same_v<T, int*>);
}

int main() {
    std::string const c = "hi";
    foo(c); // T = std::string，衰变去掉 const

    auto &p = "hi";
    static_assert(std::is_same_v<decltype(p), char const[]>);

    foo2(p); // T = char const*, 不是字符数组，注意 const 是修饰 char，不是指针，所以const保留
    foo2("hi"); // T = char const*, 不是字符数组，注意 const 是修饰 char，不是指针，所以const保留


    int arr[4];
    foo3(arr); // T = int*, 衰变，数组变指针
}