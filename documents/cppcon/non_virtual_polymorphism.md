#! https://zhuanlan.zhihu.com/p/685359271
# （笔记）CppCon 2023: 不用虚函数的多态

## 01. 笔记来源

+ [B站 CppCon 2023: A Journey Into Non-Virtual Polymorphism in C++](https://www.bilibili.com/video/BV1Ft42187PT)
+ [上面视频 的 PPT](https://github.com/CppCon/CppCon2023/blob/main/Presentations/A_Journey_into_Non_Virtual_Polymorphism_Rud_Merriam.pdf)

# 02. 背景

问题：有个软件写了一段时间，已经有了很多现成的类，现在想将现有的对象统一放到一个地方调用，应该怎么处理？

传统的做法：就是重构代码，弄个抽象类，然后将需要放到统一地方调用的函数统统修改为继承自抽象类的override虚函数实现；

下面先看看 不用虚函数 实现的一些做法

事先说明，技术无分好坏，所谓：“文章做到极处，无有它奇，只是恰好”

# 03. 具体问题

狗会跑：

``` cpp
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};
```

鸟会飞：

``` cpp
#include <iostream>

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};
```

鱼会游：

``` cpp
#include <iostream>

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};
```

现在想把 这些动作放到统一的地方调用，怎么处理？

``` cpp

int main() {
    Dog dog;
    Bird bird;
    Fish fish;
    
    // 问题1：容器该填啥类型
    std::vector<??? *> outputs {&dog, &bird, &fish};

    // 问题2：这里调用什么函数？
    for (auto o: outputs) {
        // ???
    } 
    return 0;
}

```

或曰：你一个程序员 干嘛不 事先设计好接口和继承体系，比如继承自Animal等到现在写完代码才说？

欸，我混了很多年才体会到一个道理：**全世界都是草创班子**；再说这些类又不是全出自你手，你想改别人不乐意怎么办？

# 04. 可选方案

+ 添加继承，用虚函数改写代码（不是这个讲座的主题，略）
+ std::function && std::bind （不是这个讲座的主题，略）
+ std::any: 基本不会用，太复杂；
+ std::variant && std::fvisit
+ std::tuple && std::apply
+ `CRTP`: Curiously Recurring Template Pattern

# 05. std::any

用 `std::any_cast<T>()` 做类型转换

难点：

+ 没有简单的方式 判断类型
+ 没有简单的方式 根据类型调用函数
+ 用户必须知道要用的所有类型

坏处：大家都知道，这段代码 充满了 `坏味道`，因为它对已有代码不是封闭的，就是每次添加一个类，都要改 out_any 的实现

点击 [这里](https://godbolt.org/z/bWe3jhshe) 运行代码

``` cpp
#include <any>
#include <vector>
#include <iostream>
#include <typeindex>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

void out_any(std::any const& val) {
    static auto dog_type { std::type_index(typeid(Dog)) };
    static auto fish_type { std::type_index(typeid(Fish)) };
    static auto bird_type { std::type_index(typeid(Bird)) };

    // val.type() 返回 type_info&
    const auto val_type { std::type_index(val.type())};
    
    if (val_type == dog_type) {
        std::any_cast<Dog>(val).run();
    } else if (val_type == fish_type) {
        std::any_cast<Fish>(val).swim();
    } else if (val_type == bird_type) {
        std::any_cast<Bird>(val).fly();
    } else {
        std::cout << "Unknown type." << std::endl;
    }
}

int main() {
    Dog dog;
    Fish fish;
    Bird bird;

    std::vector<std::any> animals { dog, fish, bird };

    for (auto& animal : animals) {
        out_any(animal);
    }

    return 0;
}
```

# 04. std::variant

std::variant 就是 类型安全 的 union

+ 提供 类型安全 union; 在同一内存位置存储多种类型
+ 缺点：其占用的内存大小 取决于 其所有候选成员中 最大的那个
    - 这里用指针，就缓解这个问题

点击 [这里](https://godbolt.org/z/x8r1Eqz77) 运行代码

``` cpp
#include <variant>
#include <vector>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

using Animal = std::variant<Dog*, Fish*, Bird*>;

void out_any(Animal const& val) {
    switch (val.index()) {
        case 0: {
            auto dog = std::get<Dog*>(val);
            dog->run();
            break;
        }
        case 1: {
            auto fish = std::get<Fish*>(val);
            fish->swim();
            break;
        }
        case 2: {
            auto bird = std::get<Bird*>(val);
            bird->fly();
            break;
        }
        default: {
            std::cout << "Unknown type." << std::endl;
        }
    }
}

int main() {
    Dog dog;
    Fish fish;
    Bird bird;

    std::vector<Animal> animals { &dog, &fish, &bird };

    for (auto& animal : animals) {
        out_any(animal);
    }

    return 0;
}
```

# 05. std::get_if

如果不想用 index，那么用 std::get_if 也可以，结果都是写 if

下面的例子，get_if 接受 Animal*，返回 Dog**，Fish**，Bird**

点击 [这里](https://godbolt.org/z/hjW8j3qGT) 运行代码

``` cpp
#include <variant>
#include <vector>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

using Animal = std::variant<Dog*, Fish*, Bird*>;

void out_any(Animal const& val) {
    if (auto dog = std::get_if<Dog*>(&val)) {
        (*dog)->run();
    } else if (auto fish = std::get_if<Fish*>(&val)) {
        (*fish)->swim();
    } else if (auto bird = std::get_if<Bird*>(&val)) {
        (*bird)->fly();
    } else {
        std::cout << "Unknown type." << std::endl;
    }
}

int main() {
    Dog dog;
    Fish fish;
    Bird bird;

    std::vector<Animal> animals { &dog, &fish, &bird };

    for (auto& animal : animals) {
        out_any(animal);
    }

    return 0;
}
```

# 06. std::visit

为了遍历 std::vector<std::variant>，标准库提供了 std::visit

+ std::visit(fun, o): 判断类型并调用重载函数 `fun(cast<T>(o))`，具体看注释
+ 参数可以有多个
+ 因为 std::visit 内部其实也是由类型判断的，所以实际上 if-else 的性能还是没有省掉；
+ std::visit 是模板，所以其实实例化会展开代码

点击 [这里](https://godbolt.org/z/87oce9ExW) 运行代码

``` cpp
#include <variant>
#include <vector>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

using Animal = std::variant<Dog*, Fish*, Bird*>;

void animal_func(Dog *dog) {
    dog->run();
}

void animal_func(Fish *fish) {
    fish->swim();
}

void animal_func(Bird *bird) {
    bird->fly();
}

int main() {
    Dog dog;
    Fish fish;
    Bird bird;

    std::vector<Animal> animals { &dog, &fish, &bird };

    for (auto& animal : animals) {
        // 重载函数，必须用 lambda 表达式 包装
        // 否则 visit 不知道调用哪个重载函数
        std::visit([](auto&& arg) {
            animal_func(arg);
        }, animal);
    }

    return 0;
}
```

# 07. std::visit 和 仿函数

点击 [这里](https://godbolt.org/z/vzo6rqef9) 运行代码

``` cpp
#include <variant>
#include <vector>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

using Animal = std::variant<Dog*, Fish*, Bird*>;

struct AnimalAccess {
    void operator()(Dog *dog) {
        dog->run();
    }

    void operator()(Fish *fish) {
        fish->swim();
    }

    void operator()(Bird *bird) {
        bird->fly();
    }
};

int main() {
    Dog dog;
    Fish fish;
    Bird bird;

    std::vector<Animal> animals { &dog, &fish, &bird };

    for (auto& animal : animals) {
        std::visit(AnimalAccess(), animal);
    }

    return 0;
}
```

# 08. 重载：惯用法（idiom）

上面的重载是不是每个函数都要写一次，下面看看有没有别的写法


点击 [这里](https://godbolt.org/z/7qvvYMYW4) 运行代码

``` cpp
#include <variant>
#include <vector>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

using Animal = std::variant<Dog*, Fish*, Bird*>;

// 定义一个可以接受多种lambda表达式的结构体，用于std::visit

// Overload: Ls ... 表示 继承 所有 Lambda表达式
template<typename... Ls>
struct Overload : Ls ... {
    using Ls::operator()...; // 引入基类的 operator() 到 Overload 中
};

int main() {
    Dog dog;
    Fish fish;
    Bird bird;

    std::vector<Animal> animals { &dog, &fish, &bird };

    // 最终，编译器会展开成：
    // template<>
    // struct Overload<l_digital, l_analog, l_serial> :
    //     public l_digital, public l_analog, public l_serial {
    //     using l_digital::operator();
    //     using l_analog::operator();
    //     using l_serial::operator();
    // } out_overload;
    auto out_overload = Overload {
        [](Dog *dog) { dog->run(); },
        [](Fish *fish) { fish->swim(); },
        [](Bird *bird) { bird->fly(); },
    };

    for (auto& animal : animals) {
        std::visit(out_overload, animal);
    }

    return 0;
}
```

# 09. std::tuple

上面的重载是不是每个函数都要写一次，下面看看有没有别的写法


点击 [这里](https://godbolt.org/z/rff7aP8so) 运行代码

``` cpp
#include <tuple>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

void animal_func(Dog *dog) {
    dog->run();
}

void animal_func(Fish *fish) {
    fish->swim();
}

void animal_func(Bird *bird) {
    bird->fly();
}

int main() {
    Dog dog;
    Dog dog2;
    Fish fish;
    Bird bird;

    using Animal = std::tuple<Dog*, Dog*, Bird*, Fish*>;

    Animal animals {&dog, &dog2, &bird, &fish};

    std::get<0>(animals)->run(); // Dog
    
    animal_func(std::get<1>(animals));
    animal_func(std::get<Fish*>(animals));
    animal_func(std::get<Bird*>(animals));
    
    // 注意：这个 通不过编译，因为有两个 Dog*
    // animal_func(std::get<Dog*>(animals)->run(); 
    
    return 0;
}
```

# 10. std::apply

点击 [这里](https://godbolt.org/z/on8febTEh) 运行代码

``` cpp
#include <tuple>
#include <iostream>

class Dog {
public:
    void run() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird {
public:
    void fly() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish {
public:
    void swim() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

void animal_func(Dog *dog) {
    dog->run();
}

void animal_func(Fish *fish) {
    fish->swim();
}

void animal_func(Bird *bird) {
    bird->fly();
}

int main() {
    Dog dog;
    Dog dog2;
    Fish fish;
    Bird bird;

    auto tup = std::make_tuple(
        &dog, &dog2, &fish, &bird
    );

    auto tup_apply = []<typename... Ts>(Ts const& ... tupleArgs) {
        (animal_func(tupleArgs),...);
    };

    std::apply(tup_apply, tup);
    
    return 0;
}
```

# 11. `CRTP`: Curiously Recurring Template Pattern

CRTP: 通过让一个类继承自一个模板基类，并将派生类自身作为模板参数传递给基类，来实现静态多态（编译时多态）和代码复用。

这种模式的名称源于 定义方式颇为奇特：定义类时，看似递归地继承自一个模板实例化版本的自己。

问题：没办法放到统一的容器里面

下面另外换一个例子：三角形，四边形的 draw

点击 [这里](https://godbolt.org/z/91qezT9ff) 运行代码

``` cpp
#include <iostream>

// Shape 是基类，D是模板化的子类
template<typename D>
struct Shape {
    void draw() {
        auto& derived{static_cast<D&>(*this)};
        
        derived.draw_impl();
    }
};

struct Rectangle : public Shape<Rectangle> {
    void draw_impl() const { std::cout << "Rectangle\n"; }
};

struct Square : public Shape<Square> {
    void draw_impl() const { std::cout << "Square\n"; }
};

struct Triangle : public Shape<Triangle> {
    void draw_impl() const { std::cout << "Triangle\n"; }
};

int main() {
    Rectangle rect;
    rect.draw();

    Square sqr;
    sqr.draw();

    Triangle tri;
    tri.draw();

    return 0;
}
```

# 12. `CRTP`: 通过 友元 封装细节

注意：上面的 void draw_impl() 是 public的，这个不应该！

点击 [这里](https://godbolt.org/z/s7M5YTj4x) 运行代码

``` cpp
#include <iostream>

template<typename D>
struct Shape {
    void draw() const { 
        derived().draw_impl(); 
    }
    
    void erase() const {
        derived().draw_impl(true); 
    }
    
    D const& derived() const {
        return static_cast<D const&>(*this); 
    };
};

struct Rectangle : public Shape<Rectangle> {
private:
    friend Shape;
    
    void draw_impl( bool const erase = false) const {
        std::cout << "Rectangle\n";
    }
};

struct Square : public Shape<Square> {
private:
    friend Shape;
    
    void draw_impl( bool const erase = false) const {
        std::cout << "Square\n";
    }
};

struct Triangle : public Shape<Triangle> {
private:
    friend Shape;
    
    void draw_impl( bool const erase = false) const {
        std::cout << "Triangle\n";
    }
};

int main() {
    Rectangle rect;
    rect.draw();

    Square sqr;
    sqr.draw();

    Triangle tri;
    tri.draw();

    return 0;
}
```

# 13. 需要容器时候，还是得用 std::variant

点击 [这里](https://godbolt.org/z/3Y6YsKqoG) 运行代码

``` cpp
#include <vector>
#include <variant>
#include <iostream>

template<typename D>
struct Shape {
    void draw() const { 
        derived().draw_impl(); 
    }
    
    void erase() const {
        derived().draw_impl(true); 
    }
    
    D const& derived() const {
        return static_cast<D const&>(*this); 
    };
};

struct Rectangle : public Shape<Rectangle> {
private:
    friend Shape;
    
    void draw_impl( bool const erase = false) const {
        std::cout << "Rectangle\n";
    }
};

struct Square : public Shape<Square> {
private:
    friend Shape;
    
    void draw_impl( bool const erase = false) const {
        std::cout << "Square\n";
    }
};

struct Triangle : public Shape<Triangle> {
private:
    friend Shape;
    
    void draw_impl( bool const erase = false) const {
        std::cout << "Triangle\n";
    }
};

int main() {
    Rectangle rect;
    Square sqr;
    Triangle tri;
    
    using ShapeVar = std::variant<Rectangle*, Square*, Triangle*>;

    std::vector<ShapeVar> shapes{&rect, &sqr, &tri};

    for (auto& s: shapes) {
        std::visit([](auto* v) { 
            v->draw(); 
        }, s);
    }

    return 0;
}
```

# 14. C++23: this推导

这允许成员函数的this参数被显式地声明在函数参数列表中，从而使得成员函数可以像普通函数那样被重载和模板化。

注：gcc 13.2 尚不支持 this推导，下面网址用了 最新主干 gcc编译

点击 [这里](https://godbolt.org/z/zdr6cGWsq) 运行代码

``` cpp
#include <iostream>

template<typename D>
struct Shape {
    // C++ 23 引入 this推导
    // 不需要自己 static_cast<D&>(*this);
    template<typename T>
    void draw(this T&& self) { 
        self.draw_impl();
    }
};

struct Rectangle : public Shape<Rectangle> {
    void draw_impl() const {
        std::cout << "Rectangle\n";
    }
};

struct Square : public Shape<Square> {
    void draw_impl() const {
        std::cout << "Square\n";
    }
};

struct Triangle : public Shape<Triangle> {
    void draw_impl( bool const erase = false) const {
        std::cout << "Triangle\n";
    }
};

int main() {
    Rectangle rect;
    rect.draw();

    Square sqr;
    sqr.draw();

    Triangle tri;
    tri.draw();

    return 0;
}
```

# 15. `TODO` 最后一个例子

暂时没弄明白，作者想表达什么特性!

点击 [这里](https://godbolt.org/z/ecTYnrjxK) 运行代码

``` cpp
#include <iostream>
#include <thread>
#include <chrono>

template <typename DerivedT>
struct WaitFor {
    template<typename Func>
    void wait_for(Func member, int16_t timeout = 100) {
        auto& derived = *static_cast<DerivedT*>(this);
        while (!(derived.*member)() && timeout > 0) { // 修改这里
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            --timeout;
        }
        if (timeout <= 0) {
            std::cout << "Timeout!" << std::endl;
        } else {
            std::cout << "Condition met!" << std::endl;
        }
    }
};

class Power : public WaitFor<Power> {
public:
    bool isAwake() const {
        // 这里只是一个示例条件，实际条件应该根据实际情况设计
        static int counter = 0;
        ++counter;
        return counter > 5; // 假设在一定次数后条件满足
    }
};

int main() {
    Power pow;
   
    // 使用wait_for等待Power的isAwake条件成立
    pow.wait_for(&Power::isAwake);  
    return 0;
}
```