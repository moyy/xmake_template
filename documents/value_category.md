# 值类别 Value Category

## 1. 值类别 是 针对 表达式 的，并不是针对 对象，类型 

例子：

``` cpp

void f(int &);
void f(int &&);

int &&r = 4;
f(r); // 调用 f(int &)，因为 r 可以 取地址
```

## 2. 表达式 类型 和 值类别的作用

``` cpp
int i = 42;
i = 77; // OK
77 = i; // 77 不能放在左边
```

## 3. 《K & R C》，只有 lvalue 和 rvalue

+ `lvalue`: 可以出现 在 赋值 左边
+ `rvalue` 只能出现 在 赋值 右侧

``` cpp
int x; // x 是左值
x = 42;

42 = x ; // ERROR: 常量是右值，不能出现在左边

int ∗ p1 = &x ; // OK: 左值可以取地址
int ∗ p2 = &42; // ERROR: 右值不能取地址
```

## 4. ANSI-C 出现了 const

``` cpp
const int c = 42; // 按上面的定义，c 是 左值 还是 右值？

c = 42;             // ERROR: c不能出现在左边，应该是右值？ 
const int *pl = &c; // OK: 左值可以取地址？
```

所以，为了适应这个，ANSI-C 修改了 左右值的判断标准：是否可以取地址

+ `lvalue`: 可以取地址的，是 左值表达式
+ `rvalue` 不可以取地址的，是 右值表达式
    - 按这个说法，除 字符串 外的 字面量，都是 右值

## 5. C++ 11 

有了 std::move，就变的更复杂了。

对于 类类型：

``` cpp
std::string s;

std::move(s) = "hello";  // OK，类似 左值
auto ps = &std::move(s); // ERROR，类似 右值，不能取地址
```

对于 内置类型：

``` cpp
int i;

std::move(i) = 42;        // ERROR, ???
auto pi = &std::move(i);  // ERROR，类似 右值，不能取地址
```

引入了 新 类别 `xvalue(“eXpire value”)` 临时的将亡值

+ `prvalue` 纯右值，就是 以前的 `rvalue`: 可移动，`不能 取地址`
    - 除 字符串 外的 `字面量` 组成的 表达式 （如，42、true 或 nullptr）
    - 函数调用，如果 该函数按 值 返回 的 话；
    - 取地址的 表达式
    - Lambda 表达式
    - 后置递增，递减 运算符
+ `lvalue` 还是以前的 `lvalue`, 不可移动，可取地址
    - 变量，函数，数据成员 名称
    - 仅 字符串字面量 表达式 (如，"hello")
    - 函数调用，如果 该函数按 Type& 返回 的 话
    - 任何对函数的引用，即使标记为 std::move
    - 解引用 *
+ `xvalue` 将亡值，可移动，可取地址；
    - std::move() 的返回值
    - 对 对象类型 (不是函数类型) 的 rvalue 引用 的 强制转换
    - 函数声明返回 rvalue 引用 (返回类型 type &&)
    - 右值 的 非静态值成员 (见下面)
+ `rvalue` = prvalue + xvalue; 可移动
+ `glvalue` = `lvalue` + `xvalue`; 可取地址

经验法则：

+ 所有 名称 表达式 都是 lvalue
+ 所有 字符串 字面值 都是 lvalue
+ 所有 非字符串 字面值 都是 prvalue
+ 所有 没有名称 的 临时对象 都是 prvalues
+ 所有 std::move() 的 `对象 及其 值成员` 都是 xvalues

## 6. C++ 17

解释 值类别:

+ glvalue: 用于 长生命周期 对象 或 函数位置 的 表达式
+ prvalue: 用于 `短生命周期 对象 的 初始化表达式`
+ `xvalue`: 表示 不再需要 其资源 的 (长期存在的) 对象
+ `通过 值传递 prvalue`

有了这个更改，即使 没有定义 拷贝构造 / 移动构造，也可以将 prvalue 作为 未命名的 初始值 按值传递

+ 有利于 RVO

``` cpp

class C {
public:
	C()=default;
	C(const C&) =delete;
	C(C&&) =delete;
};

C createC() {
	return C{} ; // 临时对象
} // 注意：在 C++ 17，这里 没有 临时值

void takeC (C val) {

}

auto n = createC();    // OK since C++17 ( error prior to C++17)

takeC(createC());      // OK since C++17 ( error prior to C++17)

```

C++17 之前，如果没有复制或移动支持，传递 prvalue( 例如：createC() 的创建和初始化返回值 ) 是不可能的。

从 C++17 开始，只要是 不需要有地址 的 对象，就可以按值传递 prvalues

### 具象化: 未命名的临时对象

C++17 引入 新术语，`具象化` (未命名的临时对象)，此时 `prvalue 变成了临时对象`。因此，临时物化转换是 prvalue --> xvalue

需要 glvalue(左值或 xvalue) 的地方使用 prvalue，就会创建临时对象，并使用该 prvalue 初始化 (记住，prvalue 主要是“初始化值”)，并且该 prvalue 会指定临时对象的 xvalue。

因此，在上面的例子中:

``` cpp
void f ( const X& p ) ; // accepts an expression of any value category but expects a glvalue

f (X{}) ; // creates a temporary prvalue and passes it material ized as an xvalue
```

因为本例中的 f() 有一个引用形参，所以需要一个 glvalue 参数。然而，表达式 X{} 是 prvalue。

因此，“临时具象化”的规则开始起作用，表达式 X{} 被“转换”为 xvalue，该 xvalue 指定使用默认构造函数初始化的临时对象。

请注意，具象化并不意味着我们创建新的/不同的对象。左值引用 p 仍然绑定到 xvalue 和 prvalue，尽管后者现在会涉及到向 xvalue 的转换。

## 7. 特殊规则

+ lvalue 的 数据成员 是 lvalue
+ rvalue 的 引用和静态数据成员 是 lvalue
+ rvalue 的 普通数据成员 是 `xvalue`

Cpp 值的种类划分
本博文会介绍移动语义的形式术语和规则。并且会正式的介绍值的类别，如 lvalue、rvalue、prvalue和 xvalue，并讨论了在绑定对象引用时的作用。也会讨论移动语义不会自动传递的细节，以及decltype 在表达式调用时的微妙行为。

作为《Cpp Move Semantics》书中最复杂的一章。可能会看到一些事实和特征，可能很难相信或理解。当您再次阅读值类别、对象的绑定引用和 decltype 时，可以再回到这里看看。

1. 值的种类
要编译表达式或语句，所涉及的类型是否合适并不重要。例如，如果在赋值符的左边使用了 int 型字面值，则不能将 int 型赋值给 int 型字面值:

int i = 42;
i = 77; // OK
77 = i; // Obvious ERROR
因此，C++ 程序中的每个表达式都有值类别。除了类型之外，值类别对于决定表达式可以做什么也很重要。

然而，值类别在 C++ 中随着时间的推移而改变。

1.1 值类型的历史
从历史上看 (引用 Kernighan&Ritchie C, K&R C)，最初只有 lvalue 和 rvalue:

lvalue 可以出现在赋值的左边
rvalue 只能出现在赋值的右侧
根据这个定义，当使用 int 对象/变量时，使用的是 lvalue，但当使用 int 字面值时，使用的是 rvalue:

int x; // x i s an lva lue when used in an expression

x = 42; // OK, because x i s an lva lue and the type matches
42 = x ; // ERROR: 42 i s an rvalue and can be only on the right−hand s ide o f an assignment
然而，这些类别不仅重要，并且通常用于指定表达式是否以及在何处可以使用。例如:

int x ; // x i s an lva lue when used in an expression

int ∗ p1 = &x ; // OK: & i s f in e for lva lues ( object has a sp e c i f i ed locat ion )
int ∗ p2 = &42; // ERROR: & i s not allowed for rvalues ( object has no sp e c i f i ed location)
然而，在 ANSI-C 中，事情变得更加复杂。，因为声明为 const int 的 x 不能放在赋值函数的左边，但仍然可以在其他只能使用左值的地方使用:

const int c = 42; // Is c an lva lue or rvalue?

c = 42; // now an Error (so that c should no longer be an lvalue)
const int *pl = &c; // still OK (so that c should be an lvalue)
C 语言中，声明为 const int 的 c 仍然是 lvalue，因为对于特定类型的 const 对象，仍然可以调用大多数 lvalue 操作。唯一不能做的就是在赋值函数的左边有一个 const 对象。

因此，在 ANSI-C 中，l 的含义变成了定位。lvalue 现在是程序中具有指定位置的对象 (例如，以便您可以获取地址)。以同样的方式，rvalue 现在只是一个可读的值。

C++98 采用了这些值类别的定义。然而，随着移动语义的引入，问题出现了: 用 std::move() 标记的对象应该有哪些值类别，用 std::move() 标记的类的对象应该遵循以下规则:

std::string s;
...
std::move(s) = "hello";  // OK (behaves like an lva lue )
auto ps = &std::move(s); // ERROR (behaves like an rvalue )
但是，请注意基本数据类型 (FDT) 的行为

int i;
...
std::move(i) = 42; // ERROR
auto pi = &std::move(i); // ERROR
除了基本数据类型之外，标记为 std::move() 的对象仍应该像 lvalue 一样，允许修改它的值。

另一方面，也存在一些限制，比如不能获取该地址。

因此，引入了一个新的类别 xvalue(“eXpire value”) 来为显式标记的对象指定规则，因为这里不再需要这个值 (主要是用 std::move() 标记的对象)。

大多数 C++11 前的 rvalue 的规则也适用于 xvalue。因此，以前的 rvalue 变成了一个复合值类别，现在表示新的主值类别 prvalue(对于以前的所有 rvalue) 和 xvalue。关于提出这些改变的论文，请参阅 http://wg21.link/n3055。

1.2 C++11 的值类别
C++11 的值类别如图所示。

C++11 的值类别
C++11 的值类别

有以下主要类别:

lvalue ("定位值")
prvalue("纯可读值")
xvalue("过期值")
综合类别为

gvalue ("广义 lvalue") 作为 "左 lvalue" 或 "xvalue" 的常用术语
rvalue 作为 “xvalue” 和 “prvalue” 的常用术语
基本表达式的值分类

lvalue 的例子有:

仅为变量、函数或数据成员 (除右值的普通值成员外) 的名称的表达式
只是字符串字面量的表达式 (例如，"hello")
如果函数声明返回左值引用，则返回函数的值 (返回类型 type &)
任何对函数的引用，即使标记为 std::move()(参见下面)
内置的一元操作符 * 的结果 (即，对原始指针进行解引用所产生的结果)
prvalue 的例子有:

由非字符串字面量的内置字面量组成的表达式 (例如，42、true 或 nullptr)
如果函数声明为按值返回，则按类型返回值 (返回类型为 Type)。
内置的一元操作符 & 的结果 (即，获取表达式的地址所产生的结果)
Lambda 表达式
xvalues 的示例如下:

用 std::move() 标记对象的结果
对对象类型 (不是函数类型) 的 rvalue 引用的强制转换
函数声明返回 rvalue 引用 (返回类型 type &&)
右值的非静态值成员 (见下面)
例如:

class X
{};

X v;
const X c;

f(v); // passes a modifiable lva lue
f(c); // passes a non−modifiable lva lue
f(X()); // passes a prvalue (old syntax o f creat ing a temporary)
f(X{}); // passes a prvalue (new syntax o f creat ing a temporary)
f(std::move(v)); // passes an xvalue
说些经验法则:

所有用作表达式的名称都是 lvalue。
所有用作表达式的字符串字面值都是 lvalue。
所有非字符串字面值 (4.2、true 或 nullptr) 都是 prvalue。
所有没有名称的临时对象 (特别是回的对象) 都是 prvalues。
所有标记为 std::move() 的对象，及其值成员都是 xvalues。
严格来说，glvalues、prvalues 和 xvalues 是表达式的术语，而不是值的术语 (这意味着这些术语用词不当)。例如，变量本身不是 lvalue，只有表示该变量为 lvalue 的表达式:

int x = 3 ; // here , x i s a variable , not an lva lue
 int y = x ; // here , x i s an lva lue
第一个语句中，3 是一个初始化变量 x 的 prvalue(不是 lvalue)。第二个语句中，x 是一个 lvalue(它的计算值指定了一个包含值 3 的对象)。lvalue x 用作 rvalue，它初始化变量 y。

1.3 C++17 新加的值类别
C++17 具有相同的值类别，图 8.2 中描述了值类别的语义。

现在解释值类别有两种主要的表达方式:

glvalues: 用于长生命周期对象或函数位置的表达式

prvalues: 用于短生命周期对象的初始化表达式

然后，xvalue 是表示不再需要其资源/值的 (长期存在的) 对象。

C++17 新加的值类别
C++17 新加的值类别

通过值传递 prvalues

有了这个更改，即使没有定义有效的副本和有效的移动构造函数，现在也可以将 prvalue 作为未命名的初始值按值传递:

class C {
public :
	C( . . . ) ;
	C( const C&) = de lete ; // th is c l a s s i s neither copyable . . .
	C(C&&) = de lete ; // . . . nor movable
} ;

C createC ( ) {
	return C{ . . . } ; // Always creates a conceptual temporary pr ior to C++17.
} // In C++17, no temporary object i s created at th is point .

void takeC (C v al ) {
...
}

auto n = createC();    // OK s ince C++17 ( error pr ior to C++17)
takeC(createC());      // OK s ince C++17 ( error pr ior to C++17)
C++17 之前，如果没有复制或移动支持，传递 prvalue(例如：createC() 的创建和初始化返回值) 是不可能的。但是，从 C++17 开始，只要是不需要有地址的对象，就可以按值传递 prvalues。

具象化

C++17 随后引入了一个新术语，称为具象化 (未命名的临时对象)，此时 prvalue 变成了临时对象。因此，临时物化转换是 prvalue 到 xvalue 转换 (通常是隐式的)。

需要 glvalue(左值或 xvalue) 的地方使用 prvalue，就会创建临时对象，并使用该 prvalue 初始化 (记住，prvalue 主要是“初始化值”)，并且该 prvalue 会指定临时对象的 xvalue。因此，在上面的例子中:

void f ( const X& p ) ; // accepts an expression o f any value category but expects a glvalue

f (X{}) ; // creates a temporary prvalue and passes i t material ized as an xvalue
因为本例中的 f() 有一个引用形参，所以需要一个 glvalue 参数。然而，表达式 X{} 是 prvalue。

因此，“临时具象化”的规则开始起作用，表达式 X{} 被“转换”为 xvalue，该 xvalue 指定使用默认构造函数初始化的临时对象。

请注意，具象化并不意味着我们创建新的/不同的对象。左值引用 p 仍然绑定到 xvalue 和prvalue，尽管后者现在会涉及到向 xvalue 的转换。

2 值类别的特殊规则
对于影响移动语义的函数和成员的值类型，有特殊的规则。

2.1 函数的值类型
C++ 标准中的特殊规则是，所有引用函数的表达式都是 lvalue。

例如:

void f(int) {}

void (&fref1)(int) = f;  // fref1 is an lvalue
void (&&fref2)(int) = f; // fref2 is also an lvalue

auto& ar = std::move(f); // OK: ar is lvalue of type void(&)(int)
如果使用对象的数据成员(例如，使用std::pair<>的第一个和第二个成员时)，将使用特殊规则。

2.2 数据成员的值类型
如果使用对象的数据成员(例如，使用std::pair<>的第一个和第二个成员时)，将使用特殊规则。

通常，数据成员的值类型如下:

lvalue的数据成员是lvalue。
rvalue的引用和静态数据成员是lvalue。
rvalue的普通数据成员是xvalue。

``` cpp
std::pair<std::string, std::string&> foo(); // note: member second is reference

std::vector<std::string> coll;

coll.push_back(foo().first); // moves because first is an xvalue here
coll.push_back(foo().second); // copies because second is an lvalue here

// coll.push_back(std::move(foo().second)); // moves

```

