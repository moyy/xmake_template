#! https://zhuanlan.zhihu.com/p/685154638
#  笔记: C++ 基础 之 五之准则

## 01. 笔记来源

+ [B站 CppCon2023: Back to Basics The Rule of Five in C++](https://www.bilibili.com/video/BV1US421A7y8)
+ [上面视频 的 PPT](https://github.com/CppCon/CppCon2023/blob/main/Presentations/B2B_Rule_of_Five.pdf)

# 02. 回顾 [《C++ Core GuideLines》](https://github.com/lynnboy/CppCoreGuidelines-zh-CN/blob/master/CppCoreGuidelines-zh-CN.md) 

零之准则 和 五之准则，都是 针对 `构造 / 析构 / 赋值` 的 一些启发式的 设计要点；

+ [C.20: 零之准则 The Rule of Zero](https://github.com/lynnboy/CppCoreGuidelines-zh-CN/blob/master/CppCoreGuidelines-zh-CN.md#c21-%E5%A6%82%E6%9E%9C%E5%AE%9A%E4%B9%89%E6%88%96%E8%80%85-delete-%E4%BA%86%E4%BB%BB%E4%BD%95%E5%A4%8D%E5%88%B6%E7%A7%BB%E5%8A%A8%E6%88%96%E6%9E%90%E6%9E%84%E5%87%BD%E6%95%B0%E8%AF%B7%E5%AE%9A%E4%B9%89%E6%88%96%E8%80%85-delete-%E5%AE%83%E4%BB%AC%E5%85%A8%E9%83%A8)
+ [C.21: 五之准则 The Rule of Five](https://github.com/lynnboy/CppCoreGuidelines-zh-CN/blob/master/CppCoreGuidelines-zh-CN.md#c21-%E5%A6%82%E6%9E%9C%E5%AE%9A%E4%B9%89%E6%88%96%E8%80%85-delete-%E4%BA%86%E4%BB%BB%E4%BD%95%E5%A4%8D%E5%88%B6%E7%A7%BB%E5%8A%A8%E6%88%96%E6%9E%90%E6%9E%84%E5%87%BD%E6%95%B0%E8%AF%B7%E5%AE%9A%E4%B9%89%E6%88%96%E8%80%85-delete-%E5%AE%83%E4%BB%AC%E5%85%A8%E9%83%A8)

## C.20: （零之准则）只要可能，请避免定义任何的默认操作

+ 默认 构造函数: 逐成员 按声明顺序 调用 默认 构造函数
+ 默认 析构函数: 逐成员 按声明相反顺序 调用 析构函数
+ 默认 拷贝构造: 逐成员 按声明顺序 调用 拷贝构造函数
+ 默认 拷贝赋值: 逐成员 按声明顺序 调用 拷贝赋值
+ 默认 移动构造: 逐成员 按声明顺序 调用 移动构造
+ 默认 移动赋值: 逐成员 按声明顺序 调用 移动赋值

这样最简单，而且能提供最清晰的语义。

示例：下面代码中，由于 std::map 和 string 都带有全部的特殊函数，这里并不需要做别的事情。

``` cpp
struct Named_map {
public:
    // ... 不要 声明 任何默认操作 ...
private:
    string name;
    map<int, int> rep;
};

Named_map nm;        // 默认构造
Named_map nm2 {nm};  // 复制构造
```

## C.21:（五之准则）如果 定义 / =delete 了 拷贝 / 移动 / 析构函数，就要 定义 / =delete / =default 全部的 五个

复制、移动和析构的语义互相之间是紧密相关的，一旦需要声明其中一个，麻烦的是其他的也需要予以考虑。

+ 注1：缺省的话就 =default; 不想要就 =delete;
+ 注2：含 析构函数 的 类，依靠隐式生成的复制操作的做法已经被摒弃。

``` cpp
class X {
public:
    // 析构函数
    // 如果 X 是基类，用 virtual
    virtual ~X() { 
        // 释放对应的资源 
    }               

    // 复制构造函数
    // 注意：const &
    X(const X&) = default;                    
    
    // 拷贝赋值
    // 注意：X& operator=(const X&)
    X& operator=(const X&) = default;     

    // 移动构造函数
    // 注意：X&& noexcept
    X(X&&) noexcept = default;            
    
    // 移动赋值
    // 注意：X operator=(X&&) noexcept
    X& operator=(X&&) noexcept = default; 
};
```

不好示例：既然对于析构函数需要“特殊关照”（这里是要进行回收操作），隐式定义的复制和移动赋值运算符仍保持正确性的可能是很低的（此处会导致双重删除问题）。

``` cpp
// 不好: 不完整的复制/移动/析构操作集合
struct M2 {   
public:
    // ...
    // ... 没有复制和移动操作 ...
    ~M2() { delete[] rep; }
private:
    pair<int, int>* rep;  // pair 的以零终止的集合
};

void use()
{
    M2 x;
    M2 y;
    // ...
    x = y;   // 缺省的赋值
    // ...
}
```

# 03. 简单字符串

点击 [这里](https://godbolt.org/z/MEhz6d6bn) 运行代码

``` cpp
#include <cstring>

class SimpleString {
public:
    SimpleString() : data_(new char[1]) { 
        *data_ = '\0'; 
    }
    
    SimpleString(char const *cp): data_(new char[strlen(cp) + 1]) {
        strcpy(data_, cp);
    }
private:
    char *data_;
};

int main() {
    // 想想看：这么写有没有问题？
    SimpleString s {"hello"};
    return 0;
}
```

# 04. 释放内存：析构函数

上面代码，有内存泄漏啊；

有 new[]， 没有 delete[] 也没有 智能指针 ！

为了释放，加入析构函数：

点击 [这里](https://godbolt.org/z/xace84bP7) 运行代码

``` cpp
#include <cstring>

class SimpleString2 {
public:
    SimpleString2() : data_(new char[1]) { 
        *data_ = '\0'; 
    }
    
    SimpleString2(char const *cp): data_(new char[strlen(cp) + 1]) {
        strcpy(data_, cp);
    }

    // 添加析构函数，释放内存
    ~SimpleString2() { 
        if (data_ != nullptr) {
            delete[] data_; 
        }
    }
private:
    char *data_;
};

int main() {
    SimpleString2 s {"hello"};
   
    // 想想看，这里有没有问题？
    SimpleString2 s2 {s};
    return 0;
}
```

# 05. 拷贝构造

上面代码，用 默认拷贝构造的话，编译器默认实现是 按字节拷贝

就是同一份 data_内容拷贝了两次，析构函数调用了 两次之后，就是两次 delete [] 同一段地址；

`delete 了 dangling pointer` ！

为了解决这个，添加 拷贝构造函数：

点击 [这里](https://godbolt.org/z/bbbK4Wfe5) 运行代码

``` cpp
#include <cstring>

class SimpleString3 {
public:
    SimpleString3() : data_(new char[1]) { 
        *data_ = '\0'; 
    }
    
    SimpleString3(char const *cp): data_(new char[strlen(cp) + 1]) {
        strcpy(data_, cp);
    }

    // 析构函数，释放内存
    ~SimpleString3() { 
        if (data_ != nullptr) {
            delete[] data_; 
        }
    }

    // 拷贝构造函数，按内容拷贝
    SimpleString3(SimpleString3 const &other) : data_(new char[strlen(other.data_) + 1]) {
        strcpy(data_, other.data_);
    }
private:
    char *data_;
};

int main() {
    SimpleString3 s {"hello"};
   
    SimpleString3 s2 {s};
   
    // 想想看，这里有没有问题？
    SimpleString3 s3;
    s3 = s;

    return 0;
}
```

# 06. 拷贝赋值运算符

同理，当默认的 拷贝赋值运算符 这里也会导致 `delete 了 dangling pointer` 

为了解决这个，添加 拷贝赋值运算符：

点击 [这里](https://godbolt.org/z/5TvavTPxG) 运行代码

``` cpp
#include <cstring>

class SimpleString4 {
public:
    SimpleString4() : data_(new char[1]) { 
        *data_ = '\0'; 
    }
    
    SimpleString4(char const *cp): data_(new char[strlen(cp) + 1]) {
        strcpy(data_, cp);
    }

    // 析构函数，释放内存
    ~SimpleString4() { 
        if (data_ != nullptr) {
            delete[] data_; 
        }
    }

    // 拷贝构造函数，按内容拷贝
    SimpleString4(SimpleString4 const &other) : data_(new char[strlen(other.data_) + 1]) {
        strcpy(data_, other.data_);
    }

    // 拷贝赋值运算符，按内容拷贝
    SimpleString4& operator=(SimpleString4 const &other) {
        // 如果是同一个引用，啥都不要做
        if (this != &other) {
            auto data = new char[strlen(other.data_) + 1];
            strcpy(data, other.data_);
            
            // 保证 异常安全
            // 因为 new 可能抛异常，所以要等新数据准备好后，才删旧数据
            if (data_ != nullptr) {
                delete[] data_; 
            }
            data_ = data;
        }
        return *this;
    }
private:
    char *data_;
};

int main() {
    SimpleString4 s {"hello"};
   
    SimpleString4 s2 {s};
   
    SimpleString4 s3;
    s3 = s;

    return 0;
}
```

# 07. 移动复制函数

下面代码调用了哪个构造函数，为什么？

点击 [这里](https://godbolt.org/z/vPEGjh69c) 运行代码

``` cpp
#include <cstring>
#include <iostream>

class SimpleString5 {
public:
    SimpleString5() : data_(new char[1]) { 
        *data_ = '\0'; 
    }
    
    SimpleString5(char const *cp): data_(new char[strlen(cp) + 1]) {
        strcpy(data_, cp);
    }

    // 析构函数，释放内存
    ~SimpleString5() { 
        if (data_ != nullptr) {
            delete[] data_; 
        } 
    }

    // 拷贝构造函数，按内容拷贝
    SimpleString5(SimpleString5 const &other) : data_(new char[strlen(other.data_) + 1]) {
        std::cout << "SimpleString5(SimpleString5 const &)"<< std::endl;
        
        strcpy(data_, other.data_);
    }

    // 拷贝赋值运算符，按内容拷贝
    SimpleString5& operator=(SimpleString5 const &other) {
        // 如果是同一个引用，啥都不要做
        if (this != &other) {
            auto data = new char[strlen(other.data_) + 1];
            strcpy(data, other.data_);
            
            // 保证异常安全；
            // 因为 new 可能抛异常，所以要等新数据准备好后，才删旧数据
            if (data_ != nullptr) {
                delete[] data_; 
            }
            data_ = data;
        }
        return *this;
    }
private:
    char *data_;
};

int main() {
    SimpleString5 s {"hello"};
   
    // 想想看，为什么这里会打印 SimpleString5(SimpleString5 const &)
    SimpleString5 s4 { std::move(s) };

    return 0;
}
```

为了让效率提高，还得实现 移动构造 和 移动赋值运算符

点击 [这里](https://godbolt.org/z/on1rs8Y78) 运行代码

``` cpp
#include <cstring>
#include <iostream>

class SimpleString6 {
public:
    SimpleString6() : data_(new char[1]) { 
        *data_ = '\0'; 
    }
    
    SimpleString6(char const *cp): data_(new char[strlen(cp) + 1]) {
        strcpy(data_, cp);
    }

    // 析构函数，释放内存
    ~SimpleString6() { 
        if (data_ != nullptr) {
            delete[] data_; 
        }
    }

    // 拷贝构造函数，按内容拷贝
    SimpleString6(SimpleString6 const &other) : data_(new char[strlen(other.data_) + 1]) {
        std::cout << "SimpleString6(SimpleString6 const &)"<< std::endl;
        
        strcpy(data_, other.data_);
    }

    // 移动构造函数
    SimpleString6(SimpleString6 &&other) noexcept: data_(other.data_) {
        std::cout << "SimpleString6(SimpleString6 &&)" << std::endl;
        other.data_ = nullptr;
    }

    // 拷贝赋值运算符，按内容拷贝
    SimpleString6& operator=(SimpleString6 const &other) {
        // 如果是同一个引用，啥都不要做
        if (this != &other) {
            auto data = new char[strlen(other.data_) + 1];
            strcpy(data, other.data_);
            
            // 保证异常安全；
            // 因为 new 可能抛异常，所以要等新数据准备好后，才删旧数据
            if (data_ != nullptr) {
                delete[] data_; 
            }
            data_ = data;
        }
        return *this;
    }

    // 移动赋值运算符
    SimpleString6& operator=(SimpleString6 &&other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;

            other.data_ = nullptr;
        }
        return *this;
    }

private:
    char *data_;
};

int main() {
    SimpleString6 s {"hello"};
   
    // 想想看，为什么这里会打印 SimpleString6(SimpleString6&&)
    SimpleString6 s2 { std::move(s) };

    return 0;
}
```

# 08. 小结：5个函数，只要实现一个，基本都要全部实现；

我们来看看 这张表格，将 6个特殊函数分成 4类：

+ 编译器默认实现 defaulted
+ 编译器 默认删除 deleted
+ 编译器 默认 不会声明 not declared
+ 用户实现 user declared

![](https://www.foonathan.net/images/special-member-functions.png)

可以看到，除了 前两行，剩下的规则很复杂：

+ 只要实现拷贝，移动默认是不会声明的；
+ 只要实现移动，拷贝默认是被删除的； 

干脆就不记了，只要实现了上面5个之一，就实现所有的，大不了用 =default 和 =delete 标注；

你以为这就完了吗？别急，还早！

# 09. 如果不想用 原始指针实现

考虑下，智能指针用 shared 还是 unique

点击 [这里](https://godbolt.org/z/4eK9KTj14) 运行代码

``` cpp
#include <cstring>
#include <memory>

class SimpleString7 {
public:
    SimpleString7() : data_(new char[1]) { 
        *data_.get() = '\0'; 
    }

    SimpleString7(char const * cp) : data_(new char[strlen(cp) + 1]) {
        strcpy(data_.get(), cp);
    }
private:
    // C++ 17 支持 shared_ptr<[]>
    std::shared_ptr<char[]> data_;
};

int main() {
    SimpleString7 s {"hello"};
    return 0;
}
```

# 10. 独占指针

+ 用 unique 有个问题：它本身不支持 拷贝语义，所以要实现 拷贝语义的函数！
+ 虽然 unique 支持 析构 / 移动 语义，但是根据“五之准则”，既然实现了拷贝，就要 全部声明
    - 否则，根据上面的表格，实现了拷贝之后，移动语义会变成 not-declared
    - 记不住记不住，直接实现了吧。

点击 [这里](https://godbolt.org/z/h6WKTrM3c) 运行代码

``` cpp
#include <cstring>
#include <memory>

class SimpleString8 {
public:
    SimpleString8() : data_(new char[1]) { 
        *data_.get() = '\0'; 
    }

    SimpleString8(char const * cp) : data_(new char[strlen(cp) + 1]) {
        strcpy(data_.get(), cp);
    }

    // 析构
    ~SimpleString8() = default;

    // 拷贝构造
    SimpleString8(SimpleString8 const & rhs): data_(new char[strlen(rhs.data_.get()) + 1]) {
        strcpy(data_.get(), rhs.data_.get());
    }
    
    // 拷贝赋值
    SimpleString8& operator=(SimpleString8 const & rhs) {
        if (this != &rhs) {
            data_.reset(new char[strlen(rhs.data_.get()) + 1]);
            strcpy(data_.get(), rhs.data_.get());
        }
        return *this;
    }

    // 移动构造
    SimpleString8(SimpleString8 && rhs) noexcept = default;

    // 移动赋值
    SimpleString8& operator=(SimpleString8 && rhs) noexcept = default;
private:
    // C++ 14 支持 unique_ptr<[]>
    std::unique_ptr<char[]> data_;
};

int main() {
    SimpleString8 s {"hello"};

    SimpleString8 s2 {s};

    SimpleString8 s3 {std::move(s)};

    return 0;
}
```

# 11. 零之准则

下面看看什么情况用 零之准则

考察：默认实现

如果 5个都能满足，就全部不实现不声明；

点击 [这里](https://godbolt.org/z/EY4Wr9roG) 运行代码

``` cpp
#include <string>

class SimpleString9 {
public:
    SimpleString9() = default;
    
    SimpleString9(char const * cp) : data_(cp) { }
private:
    std::string data_;
};

int main() {
    SimpleString9 s {"hello"};

    SimpleString9 s2 {s};

    SimpleString9 s3 {std::move(s)};

    return 0;
}
```