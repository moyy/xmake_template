# C++ Primer 5th

# 2. 变量 & 基本类型

## 2.1 基本内置类型

+ void
+ 算数类型：布尔型，整形，浮点型

注意：

+ `unsigned int` 可以 缩写成 `unsigned`
+ `char` 不总是 `signed char`
    - `char` 在某些编译器上是 `signed char`，另一些则是 `unsigned char`

建议：

+ 整型 尽量用 int
+ 浮点型 尽量用 double
+ 8字节的 整型，不要用 char，尽量用 signed char / unsigned char
+ 需要 固定长度 的 假设，加上断言 `static_assert<sizeof(int) == 32>;`

||最小尺寸||
|--|--|--|
|`bool`|未定义||
|`char`|8位||
|wchat_t|16 位||
|`char16_t`|16 位||
|`char32_t`|32 位||
|`short`|16 位||
|`int`|`16 位`||
|long|32 位||
|`long long`|64 位||
|float|一般 32位||
|`double`|一般 64位||
|long double|精度不小于 double||

# 3. 字符串，向量，数组

# 4. 表达式

# 5. 语句

# 6. 函数

# `7. 类`

## 7.1. 类定义

+ 数据抽象 和 封装 encapsulation
    - 数据抽象：分离 接口 和 实现
    - 封装后的类，隐藏了 实现细节，使用时只能看到 接口
+ 在类内部 实现 的 方法 都是 隐式的 inline
+ class A 的 方法中，this原型: `A* const this`，不能改变this的指向
+ class A 的 `const`方法中，void A::foo() const 的 this 的原型: `const A* const this`
    - const方法 如果返回 \*this，那么类型一定是 const A*
+ `const 和 非const` 也是 方法重载
+ 成员 后置声明了 `mutable`，即使是`const`方法 也可以 修改
+ 前向声明 类: class A; / struct A;
    - 一般 用于 声明 类 的 指针，引用
    - 无法知道 A 占多少空间，也无法知道 A 有哪些 成员；

`构造，析构，赋值，移动`：

+ 默认构造函数，无参
    - 编译器合成：如果没有定义，则自动生成一个
    - 如果已有构造函数的重载版本，则不会合成默认构造；
    - A::A()=default 显示合成
    - 合成规则：成员有默认值用默认值，没有就调用成员的默认构造函数
+ 析构，拷贝构造，赋值
    - A::~A(); 没有 则 合成，按声明顺序相反析构
    - A::A(const A&); 没有 则 合成，按声明顺序复制
    - A& A::operator=(const A&); 没有 则 合成，按声明顺序赋值
    - `移动语义，没有就没有，不会 自动 合成`

## 7.2. 访问控制

+ public 类外可访问，private 仅类内可访问
+ struct 默认成员都是 public
+ class 默认成员都是 private

## 7.3. 友元

最好在类一开始或结束的地方集中声明 友元

+ 友元函数：friend void add(const A &a);
+ 成员函数 也可以作为友元 friend void A::add(int a);
+ 友元函数一般用于说明访问权限，最好在 外部再独立声明一遍！
+ 友元类: friend class B; 无对称性，无传递性；
 
## 7.4. 类作用域 

编译器 对 类 处理：

+ 先编译 类的 成员 声明
+ 当 类全部成员声明 对 编译器完全可见后，再编译 成员方法的函数体；
+ 变量作用域
    - 成员函数内部找
    - 类内部找
    - 成员函数外部找

## 7.5. 构造函数

初始值列表：不管怎么写，成员初始化的顺序取决于其在类声明的顺序

+ 一个成员的初始化，最好不要依赖另一个成员；

默认构造

+ 注：如果一个类提供默认构造，所有成员都要有默认构造
+ 注：调用时候，不要加括号，这么写是个函数，不是调用默认构造 A a(); 要这么写 A a;

转换构造函数：隐式的类型转换

+ 只有一个参数的构造函数，会提供隐式转换
+ 但编译器只提供一步转换
+ A::A(const std::string &s)
+ void foo(const A& a)
+ foo("hello"); // 编译错误，char* --> string --> A(string)
+ foo(std::string("hello")); // 正确，string --> A(string)
+ foo(A("hello")); // 正确，string --> A(string)

`explicit` 作用：禁止隐式转换

+ 只提供在类的声明处，仅对 只有一个参数的构造函数有效，
+ 用了 explicit 就 只能 直接初始化，不能赋值初始化
+ A a(string("hello")); // 正确
+ A a = string("hello"); // 错误

聚合类：特殊的初始化语法

+ 所有成员都是 public
+ 没有定义任何构造函数
+ 无 类内 初值
+ 无基类 和 virtual

``` cpp

struct Data {
    int val;
    string s;
};

// 可以用花括号对成员逐值初始化
Data d = {0, "hello"};

```

constexpr 构造函数

``` cpp
class Debug {
public:
    constexpr Debug(bool b = true) : hw(b), io(b), other(b) {}
    constexpr Debug(bool h, bool i, bool o) : hw(h), io(i), other(o) {}
    constexpr bool any() const { return hw || io || other; }
    
    void set_io(bool b) { io = b; }
    void set_hw(bool b) { hw = b; }
    void set_other(bool b) { other = b; }
private:
    bool hw, io, other;
};

constexpr Debug io_debug(false, true, false);
if (io_debug.any()) // 相当于 if(true)
    std::cerr << "print error message";
```

## 7.6 类 静态成员 / 静态成员函数

+ static 只能出现在 类内声明处
+ 一般 静态成员的 初始化，都放到 类外 的 cpp文件中
+ constexpr 声明的 静态成员可以在 类内初始化 constexpr int period = 8;
    - 即使在类内部已经初始化了，但是外部还是要再定义一次该成员（不能设值了）
    - constexpr int A::period;
+ 静态成员 和 指针，引用 一样，都是 不完全类型
    - class A { static A a; } // OK的
+ 静态成员可以做成员函数的默认参数

# 8. IO库

# 9. 顺序容器

# 10. 泛型算法

# 11. 关联容器

# `12. 动态内存`

+ 局部变量：定义时创建，离开块作用域时销毁
+ 全局变量：程序启动时分配，程序结束前销毁
+ static变量：第一次使用分配，程序结束前销毁
+ 动态分配：自由存储区（free store） 或 叫 heap（堆）
    - new / delete
    - auto a = new A[]; / delete []a;
    - 智能指针 #include <memory>

## 12.1. 智能指针

``` cpp

share_ptr<A> p;
share_ptr<A> q;

// 以下函数 为 shared_ptr<T> 和 unique_ptr<T> 共有

if (p) {

}

*p;          // 解引用
p->mem;      // 等价于 (*p).mem
p.get();     // 返回 A*
swap(p, q);  // 交换 p 和 q 所指向的对象
p.swap(q);   // 交换 p 和 q 所指向的对象

// 以下函数 为 shared_ptr<T> 独有

auto a = std::make_shared<A>(args);

auto b = shared_ptr<A>(new A(args));

p = q; // 先递减 p，再递增 q
p.use_count(); // p的引用计数

if (p.unique()) {
    // p.use_count() == 1
}

```

## new / delete

``` cpp

int *p1 = new int;   // 值 未定义
int *p2 = new int(); // 会 值初始化为 0
```

+ #include <new>
+ 内存耗尽会抛异常 std::bad_alloc
+ 用 定位new（placement new）阻止抛异常 auto a = new(std::nothrow) int(); 分配失败返回 NULL

常见问题：

+ 内存泄漏，忘记 delete
+ delete过的指针 叫 dangling 指针
    - 解引用 dangling 指针
    - delete dangling 指针

写时复制：

``` cpp

auto p = std::make_shared<int>(10);

if (!p.unique()) {
    // 不是唯一用户，则 拷贝个新的
    p.reset(new int(*p));
}

*p += 20;

```

## 删除器

``` cpp
auto a = shared_ptr<int>(10, [](int *p) { cout << "delete for int" <<endl; });
```

陷阱：

+ 不使用 相同的 指针 多次初始化 智能指针 / 调用多次 reset
+ 不要 delete get()出来的值
+ 不使用 get() 的值 初始化 另一个 智能指针 / 调用 reset
+ 使用 get(),记得他的有效周期，只能到 智能指针结束
+ 如果要用 智能指针管理资源，记得要传 删除器

## unique_ptr

无法拷贝，无法赋值

+ auto u = make_unique<int>(10);
+ int *ptr = u.release(); // 返回 指针，并 置为 NULL

## weak_ptr

+ auto sp = std::make_shared<int>(10);
+ weak_ptr<int> w (sp); 
+ w.use_count(); // sp 指向的 对象的 引用计数
+ w.expired(); // w.use_count() == 0 返回 true
+ w.lock(); // 如 w.expired() 返回 true，返回 空；否则 返回 sp（引用计数+1）

## 12.2. new[] / delete[]

``` cpp
int *p1 = new int[10];  // 值未定义
delete []p1;

int *p2 = new int[10](); // 值初始化
delete []p2;

char *cp = new char[0]; // 合法，但 cp 不能解引用；
delete []cp;
```

智能指针

``` cpp
unique_ptr<int[]> sp (new int[10]);

// VS2017+ 修改 /Zc:__cplusplus
    
    // 仅对 MSVC 编译器添加 /Zc:__cplusplus 选项
    // if is_plat("windows") then
    //     add_cxflags("/Zc:__cplusplus", {force = true})
    // end

    #if __cplusplus >= 201703L
        // C++17 或更高版本的代码
        std::shared_ptr<int[]> sp(new int[10]);
        for (size_t i = 0; i != 10; ++i) {
            sp[i] = i;
        }
    #elif __cplusplus >= 201103L
        // C++11 代码
        std::shared_ptr<int> sp(new int[10], [](int *p) { delete[] p; });
        for (size_t i = 0; i != 10; ++i) {
            *(sp.get() + i) = i;
        }
    #else
        #error "This code requires at least C++11."
    #endif

```

# `13. 拷贝控制` Copy Control

拷贝，移动，销毁

五大件：Big Five

+ 拷贝 构造函数
+ 拷贝 赋值 运算符
+ 移动 构造函数
+ 移动 赋值
+ 析构函数

## 13.1. 拷贝构造函数

+ Foo::Foo(const Foo&);
    - 参数总是 const，函数不会是 const 的
    - 一般不会声明为 explicit，因为需要 = 
+ 如果没有定义，编译器会合成一个（就算定义了其他构造函数，也会合成一个）
    - 逐成员 拷贝
        * 内置类型直接 按 字节拷贝
        * 类类型 调用 拷贝构造函数
    - 数组，逐元素 拷贝

+ MyString dots("abc");       // 直接初始化, MyString::MyString(const char *str)
+ MyString s(dots);           // 拷贝初始化，MyString::MyString(const MyString &other)
+ MyString s2 = dots;         // 拷贝初始化，MyString::MyString(const MyString &other)
+ MyString myBook = "9-999";  // 直接初始化，MyString::MyString(const char *str)
+ MyString lines = MyString("abcdefg"); // 直接初始化，MyString::MyString(const char *str)

拷贝初始化，意味着调用 拷贝构造 / 移动构造，其时机：

+ 上面 拷贝初始化
+ 非引用 参数传递，函数返回值
+ 初始化列表：数组元素 或 聚合类型；
+ STL：insert / push
+ `STL：emplace 成员函数，用直接初始化`；

# 14. `重载运算 与 类型转换`

# 15. `面向对象 程序设计`

# 16. `模板 与 泛型编程`

# 17. 标准库特殊设施

## 17.1. tuple

## 17.2. bitset

## 17.3. 正则表达式

## 17.4. 随机数

## 17.5. IO库 再探

# 18. `用于大型程序的工具`

## 18.1. 异常处理

## 18.2. 命名空间

## 18.3. 多重继承 与 虚继承

# 19. `特殊工具与技术`

## 19.1. 控制内存分配

## 19.2. RTTI: 运行时类型识别

+ dynamic_cast
+ typeid
+ type_info
+ RTTI 使用

## 19.3. 枚举 enum

## 19.4. 类成员指针

## 19.5. 嵌套类

## 19.6. 联合 union

## 19.7. 局部类

## 19.8. 固有不可移植的特性

+ 位域
+ volatile
+ extern "C"
