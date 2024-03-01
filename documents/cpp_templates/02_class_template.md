# 《C++ template 2nd》02. 类模板

# 1. 定义 

+ 对类模板，只有使用成员函数时，才对该成员函数实例化；看成员函数 print
+ 友元：类模板，声明友元，注意 后面加个<T>
    - friend std::ostream& operator<< <T>(std::ostream& strm, const Stack& s);

点击 [这里](https://godbolt.org/z/3h96bbxzv) 运行

``` c++
#include <vector>
#include <cassert>
#include <iostream>
#include <concepts> // C++ 20 提供 概念

// T 必须实现 operator <<
template<typename T>
concept StreamInsertable = requires(T a, std::ostream& os) {
    { os << a } -> std::convertible_to<std::ostream&>;
};

template<typename T>
class Stack {
public:
    // 注意这种声明方式，运算符后面加个 <T>
    friend std::ostream& operator<< <T>(std::ostream& strm, const Stack& s);
    
    Stack() =default;
    Stack(const Stack&) =default; // 注意：这里的 Stack 可以不用模板参数，表示它类型也是T
    Stack(Stack &&) =default;
    Stack& operator=(const Stack&) =default;
    Stack& operator=(Stack &&) =default;

    // 成员函数的实现可以内联
    // push element
    void push(T const& elem) {
        // append copy of passed elem
        elems.push_back(elem); 
    }

    // pop element
    void pop(); 
    
    // return top element
    T const& top() const; 
    
    // return whether the stack is empty
    bool empty() const { 
        return elems.empty();
    }

    // 隐含要求：T 必须实现了 operator <<
    // 因为模板的成员函数都是用到时候，才会展开
    // 因此，对于没有实现 operator << 的类型，只要没调用 print，就没没有编译错误
    // print elem
    void print() const requires StreamInsertable<T> {
        for (const auto& elem : elems) {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }
private:
    std::vector<T> elems; 
};

// Implement the template version of operator<<
template<typename T>
std::ostream& operator<<(std::ostream& strm, const Stack<T>& s) {
    for (const auto& elem : s.elems) {
        strm << elem << ' ';
    }
    return strm;
}

// 实现成员函数时候，必须 整个T一起写
template<typename T>
void Stack<T>::pop ()
{
    assert(!elems.empty());

    // remove last element
    elems.pop_back(); 
}

template<typename T>
T const& Stack<T>::top () const
{
    assert(!elems.empty());

    // return copy of last element
    return elems.back(); 
}

void test_int_stack() {
    Stack<int> is;
    
    is.push(1);
    is.push(2);
    is.push(3);

    is.print();
    std::cout << is << std::endl;
    
    assert(is.top() == 3);

    is.pop();
    assert(is.top() == 2);

    is.pop();
    assert(is.top() == 1);

    is.pop();
    assert(is.empty());
}

void test_print_stack() {
    Stack<std::pair<int, int>> ps; // 注意：std::pair 没有实现 operator <<

    ps.push(std::make_pair(1, 2));

    // 打开这句，编译失败
    // ps.print();
    // std::cout << ps << std::endl;
}

int main() {
    
    test_int_stack();
    
    test_print_stack();
    
    return 0;
}
```

# 2. 特化

``` c++
#include <deque>
#include <string>
#include <vector>
#include <cassert>

template <typename T>
class Stack<T> {
public:
    void push(T& const& elem) {
        elems.push_back(elem); 
    }

    void pop() {
        elems.pop_back();
    }
    
    T& const& top() const {
        return elems.back();
    } 
    
    bool empty() const { 
        return elems.empty();
    }
private:
    std::vector<T> elems; 
};

// 特化版本: std::string
template<>
class Stack<std::string> {
public:
    void push(std::string& const& elem) {
        elems.push_back(elem); 
    }

    void pop() {
        elems.pop_back();
    }
    
    std::string& const& top() const {
        return elems.back();
    }
    
    bool empty() const { 
        return elems.empty();
    }
private:
    std::deque<std::string> elems;  // 换了 数据结构
};

int main() {
    Stack<std::string> ss;
    
    ss.push("a");
    assert(is.top() == 1);

    ss.pop();
    assert(is.empty());

    return 0;
}
```

# 3. 偏特化

点击 [这里](https://godbolt.org/z/aG87Gjxon) 运行

``` c++
template<typename T1, typename T2>
class MyClass {

};

template<typename T>
class MyClass<T, T> {

};

template<typename T>
class MyClass<T, int> {

};

template<typename T1, typename T2>
class MyClass<T1*, T2*> {

};

// template<typename T>
// class MyClass<T*, T*> {
// };

MyClass<int, float> mif;   // uses MyClass<T1,T2>
MyClass<float, float> mff; // uses MyClass<T,T>
MyClass<float, int> mfi;   // uses MyClass<T,int>
MyClass<int*, float*> mp;  // uses MyClass<T1*,T2*>

// MyClass<int, int> mi;    // ERROR: matches MyClass<T,T> and MyClass<T,int>

MyClass<int*, int*> mip; // 打开上面的注视，就编译ok了；ERROR: matches MyClass<T,T> and MyClass<T1*,T2*> 

int main() {
    return 0;
}
```

# 4. 默认参数


