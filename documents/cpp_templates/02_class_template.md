#! https://zhuanlan.zhihu.com/p/684742717
# 《cpp template 2nd》02. 类模板

# 1. 定义

点击 [这里](https://godbolt.org/z/s1xK753hs) 运行

``` cpp
#include <vector>
#include <cassert>

template<typename T>
class Stack {
public:
    Stack() = default;

    // 注意：这里的 Stack 可以不用模板参数，表示它类型也是T
    Stack(const Stack&) = default; 

    // 成员函数的实现可以内联
    void push(T const& elem) {
        elems.push_back(elem); 
    }

    void pop() {
        elems.pop_back();
    }
    
    bool empty() const {
        return elems.empty();
    }

    T const& top() const; 
private:
    std::vector<T> elems; 
};

// 类外实现成员函数
template<typename T>
T const& Stack<T>::top () const
{
    return elems.back(); 
}

int main() {
    Stack<int> intStack;
    
    intStack.push(1);
    intStack.push(2);
    assert(intStack.top() == 2);
    
    intStack.pop();
    assert(intStack.top() == 1);
    
    intStack.pop();
    assert(intStack.empty());
    
    return 0;
}
```

# 2. 成员函数 实例化

+ 对类模板，只有使用成员函数时，才对该成员函数实例化

下面程序 测试 这一点，如果 T 没有实现 operator <<，则 会报编译错误

测试所见，如果不调用 print，就不会有编译错误； 

点击 [这里](https://godbolt.org/z/M1MPxWsKn) 运行

``` cpp
#include <vector>
#include <iostream>

template<typename T>
class MyClass {
public:
    void push(T const& elem) {
        elems.push_back(elem);
    }

    // 注意，对没有实现 operator << 的类型，调用这个函数，会报编译错误
    void print() const {
        for (const auto& elem : elems) {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }
private:
    std::vector<T> elems; 
};

int main() {
    // 注意：std::pair 没有实现 operator <<
    MyClass<std::pair<int, int>> ps; 

    ps.push(std::make_pair(1, 2));

    // ps.print(); // 去掉注释，编译失败
    
    return 0;
}
```

# 3. 友元：声明友元函数，注意 后面加个 `<T>`

因为友元函数不是模板类的一部分，所以友元函数必须重新声明类型；

点击 [这里](https://godbolt.org/z/YGPesMK7d) 运行

``` cpp
#include <vector>
#include <iostream>

template<typename T>
class MyClass {
public:
    // 注：声明时 运算符后面加个 <T>
    friend std::ostream& operator<< <T>(std::ostream& os, const MyClass& s);

    void push(T const& elem) {
        elems.push_back(elem);
    }
private:
    std::vector<T> elems; 
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const MyClass<T>& s) {
    for (const auto& elem : s.elems) {
        os << elem << ' ';
    }
    return os;
}

int main() {
    MyClass<int> is; 

    is.push(1);
    is.push(2);

    std::cout << is << std::endl;
    
    return 0;
}
```

# 4. 特化

点击 [这里](https://godbolt.org/z/7G6rGqrv7) 运行

``` cpp
#include <deque>
#include <string>
#include <vector>
#include <iostream>

template<typename T>
class MyClass {
public:
    void push(T const& elem) {
        elems.push_back(elem);
    }
private:
    std::vector<T> elems; 
};

// 特化版本: std::string
template<>
class MyClass<std::string> {
public:
    void push(std::string const& elem) {
        std::cout << "calling MyClass<std::string>::push" << std::endl;
        elems.push_back(elem); 
    }

private:
    // 注意：这里 换了 数据结构
    std::deque<std::string> elems;
};

int main() {
    MyClass<std::string> ss;
    
    // 打印 calling MyClass<std::string>::push
    ss.push("a"); 
    
    return 0;
}
```

# 3. 偏特化

这里要注意：匹配规则不能有二义性；

+ T* 要比 T 更具体；因为 它 更明确
+ 当参数 是 int, int 时，T, T 和 T, int 二义性就来了。

点击 [这里](https://godbolt.org/z/hqaEeKsvq) 运行

``` cpp
#include <iostream>

template<typename T1, typename T2>
class MyClass {
public:
    MyClass() {
        std::cout << "uses MyClass<T1, T2>" << std::endl;
    }
};

// 偏特化
template<typename T>
class MyClass<T, T> {
public:
    MyClass() {
        std::cout << "uses MyClass<T, T>" << std::endl;
    }
};

// 偏特化
template<typename T>
class MyClass<T, int> {
public:
    MyClass() {
        std::cout << "uses MyClass<T, int>" << std::endl;
    }
};

// 偏特化
template<typename T1, typename T2>
class MyClass<T1*, T2*> {
public:
    MyClass() {
        std::cout << "uses MyClass<T1*, T2*>" << std::endl;
    }
};

// 偏特化
// 匹配 MyClass<int*, int*> mip; 
// 注：如果 注释掉这个偏移化，则：MyClass<int*, int*> mip; 编译错误
//    因为：MyClass<T, T> 和 MyClass<T1*, T2*> 都能匹配 
template<typename T>
class MyClass<T*, T*> {
public:
    MyClass() {
        std::cout << "uses MyClass<T*, T*>" << std::endl;
    }
};

int main() {
    // 打印 uses MyClass<T1, T2>
    MyClass<int, float> mif;   

    // 打印 uses MyClass<T, T>
    MyClass<float, float> mff; 

    // 打印 uses MyClass<T, int>
    MyClass<float, int> mfi; 

    // 打印 uses MyClass<T1*, T2*>
    MyClass<int*, float*> mp;  

    // 打印 uses MyClass<T*, T*>
    MyClass<int*, int*> mip; 

    // 注: 去掉注释，则报 编译错误
    // 因为 MyClass<T, T> 和 MyClass<T, int> 都能匹配
    MyClass<int, int> mi;

    return 0;
}
```

# 4. 默认参数

下面，把容器也变成类型

点击 [这里](https://godbolt.org/z/cx9e555sv) 运行

``` cpp
#include <vector>
#include <cassert>

// 默认的模板参数 是 std::vector<T>
template<typename T, typename Cont = std::vector<T>>
class Stack {
public:
    void push(T const& elem);

    void pop() {
        elems.pop_back();
    }
    
    bool empty() const {
        return elems.empty();
    }

    T const& top() const {
        return elems.back();
    } 
private:
    Cont elems; 
};

template<typename T, typename Cont>
void Stack<T, Cont>::push(T const& elem) {
    elems.push_back(elem); 
}

int main() {
    Stack<int> intStack;
    
    intStack.push(1);
    intStack.push(2);
    assert(intStack.top() == 2);

    return 0;
}
```

# 5. 类型别名：用 using 替代 typedef

让 类模板 写的简单点：

``` cpp
#include <deque>

template<typename T>
using DequeStack = Stack<T, std::deque<T>>;

int main() {
    DequeStack<int> ds;
    return 0;
}
```

让 迭代器 写的简单点：

``` cpp
#include <vector>

template<typename T>
using VecIter = typename std::vector<T>::iterator;

int main() {
    VecIter<int> it;
    
    return 0;
}
```

标准库 xxx_t, xxx_v 举例：

``` cpp
#include <type_traits>

namespace std {
    template<typename T> using add_const_t = typename add_const<T>::type;
}
```

# 6. cpp 17 类模板 自动参数推导

点击 [这里](https://godbolt.org/z/vf167Mdqo) 运行，可以将编译选项改为 `--std=c++14` 试试；

``` cpp
template <typename T>
class MyClass {
public:
    MyClass() = default;

    MyClass(T arg) {}
};

int main() {
    MyClass c1 = 0;     // C++ 17: MyClass<int> c1;
    MyClass c2 = c1;    // C++ 17: MyClass<int> c2;
    return 0;
}
```

# 7. 让 字符串字面量 -> MyClass<std::string>

点击 [这里](https://godbolt.org/z/1vTqocE4h) 运行

``` cpp
#include <type_traits>

template<typename T>
class MyClass {
public:
    MyClass() = default;
    
    MyClass(const T &elem) {}
};

int main() {
    MyClass s = "bottom"; // 推导为 MyClass<char[7]>
    
    static_assert(std::is_same_v<decltype(s), MyClass<char[7]>>);

    static_assert(not std::is_same_v<decltype(s), MyClass<char*>>);

    return 0;
}
```

当构造函数是值传递，会发生 衰变：

点击 [这里](https://godbolt.org/z/vnfrz9dqs) 运行

``` cpp
#include <type_traits>

template<typename T>
class MyClass {
public:
    MyClass() = default;
    
    MyClass(T elem) {}
};

int main() {
    MyClass s = "bottom"; // 推导为 MyClass<char const*>
    
    static_assert(not std::is_same_v<decltype(s), MyClass<char[7]>>);

    static_assert(std::is_same_v<decltype(s), MyClass<char const*>>);

    return 0;
}
```

想将 字符串字面量 变 MyClass<std::string> 怎么办？

+ 加声明：类模板 参数推导 指引

点击 [这里](https://godbolt.org/z/7h6bGP1vf) 运行

``` cpp
#include <string>
#include <type_traits>

template<typename T>
class MyClass {
public:
    MyClass() = default;
    
    MyClass(T) {}
};

// 类模板参数推导指引，无需特化语法
MyClass(char const*) -> MyClass<std::string>;

int main() {
    MyClass s {"bottom"};
    
    // 注: 这个 不能工作;
    // MyClass s ="bottom";
    
    static_assert(std::is_same_v<decltype(s), MyClass<std::string>>);

    return 0;
}
```
