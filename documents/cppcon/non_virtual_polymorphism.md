#! https://zhuanlan.zhihu.com/p/685359271
# 走马观花：不用虚函数实现的多态技术

# 01. 背景

笔记来源：

+ [B站 CppCon 2023: A Journey Into Non-Virtual Polymorphism in C++](https://www.bilibili.com/video/BV1Ft42187PT)
+ [上面视频 的 PPT](https://github.com/CppCon/CppCon2023/blob/main/Presentations/A_Journey_into_Non_Virtual_Polymorphism_Rud_Merriam.pdf)

导游表：

|技术|说明|所在章节|
|--|--|--|
|继承 & 虚函数|运行时多态|非主题：略|
|lambda & `std::function` & `std::bind`|统一函数调用|非主题：略|
|`std::any`|类型转换 + if|全是if，没有技巧|03节|
|`std::variant` & `std::visit`|类型安全的union|04节-08节|
|`std::tuple` & `std::apply`|静态容器|09节-10节|
|`CRTP`|编译时多态|11节-14节|

## 01.1. [点这里：各种多态技术的性能测试](https://zhuanlan.zhihu.com/p/685712713)

GCC 13.2 性能：

![](https://pic3.zhimg.com/80/v2-817c95094d4059e9c7c3b0f8c1558ef6_720w.webp)

Clang 17.0 性能：

![](https://pic3.zhimg.com/80/v2-d2899f662b8556b55a23f4db18f257c2_720w.webp)

# 02. 问题 

有个实现了几年的系统，里面已经积累了一堆动物的各种行为的模块；

突然有一天，想将其中的 移动动作 集中到一个地方调用，就问你怎么办。

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

# 03. `std::any`

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

# 04. `std::variant` 类型安全 的 union

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

# 05. `std::get_if`

如果不想用 index，那么用 `std::get_if` 也可以，结果都是写 if

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

# 06. `std::visit`

为了遍历 std::vector<std::variant>，标准库提供了 `std::visit`

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

# 07. `std::visit` 和 仿函数

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

# 09. `std::tuple` 静态容器

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

# 10. `std::apply`

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

`CRTP`: 通过让一个类继承自一个模板基类，并将派生类自身作为模板参数传递给基类，来实现静态多态（编译时多态）和代码复用。

`CRTP`技术特点：

+ 定义类时，模板参数给定子类
+ 使用函数时，需要 `static_cast` 将基类转换为子类

点击 [这里](https://godbolt.org/z/sM3njEchj) 运行代码

``` cpp
#include <iostream>

// Animal 是基类，D是模板化的子类
template<typename D>
class Animal {
public:
    void move() {
        // 注意：静态转换为具体类型，才能调用成员函数
        auto& derived { static_cast<D&>(*this) };
        
        derived.move_impl();
    }
};

class Dog: public Animal<Dog> {
public:
    void move_impl() {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird: public Animal<Bird> {
public:
    void move_impl() {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish: public Animal<Fish> {
public:
    void move_impl() {
        std::cout << "Fish is swimming." << std::endl;
    }
};

int main() {
    Dog dog;
    dog.move();

    Bird bird;
    bird.move();

    Fish fish;
    fish.move();
    return 0;
}
```

# 12. `CRTP`: 通过 友元 封装细节

注意：上面的 void move_impl() 是 public的，这个不应该！

点击 [这里](https://godbolt.org/z/8d8WjGWdz) 运行代码

``` cpp
#include <iostream>

// Animal 是基类，D是模板化的子类
template<typename D>
class Animal {
public:
    void move() {
        derived().move_impl();
    }
private:
    D const& derived() const {
        // 注意：静态转换为具体类型，才能调用成员函数
        return static_cast<D const&>(*this); 
    };
};

class Dog: public Animal<Dog> {
private:
    friend Animal;
    
    void move_impl() const {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird: public Animal<Bird> {
private:
    friend Animal;
    
    void move_impl() const {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish: public Animal<Fish> {
private:
    friend Animal;

    void move_impl() const {
        std::cout << "Fish is swimming." << std::endl;
    }
};

int main() {
    Dog dog;
    dog.move();

    Bird bird;
    bird.move();

    Fish fish;
    fish.move();
    return 0;
}
```

# 13. `CRTP` + `std::variant`

如果需要装到容器，还得要 `std::variant` 一起用。

点击 [这里](https://godbolt.org/z/1hcM4ac3Y) 运行代码

``` cpp
#include <vector>
#include <variant>
#include <iostream>

// Animal 是基类，D是模板化的子类
template<typename D>
class Animal {
public:
    void move() {
        derived().move_impl();
    }
private:
    D const& derived() const {
        // 注意：静态转换为具体类型，才能调用成员函数
        return static_cast<D const&>(*this); 
    };
};

class Dog: public Animal<Dog> {
private:
    friend Animal;
    
    void move_impl() const {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird: public Animal<Bird> {
private:
    friend Animal;
    
    void move_impl() const {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish: public Animal<Fish> {
private:
    friend Animal;

    void move_impl() const {
        std::cout << "Fish is swimming." << std::endl;
    }
};

int main() {
    Dog dog;
    Bird bird;
    Fish fish;

    using AnimalVar = std::variant<Dog*, Bird*, Fish*>;

    std::vector<AnimalVar> shapes{&dog, &bird, &fish};

    for (auto& s: shapes) {
        std::visit([](auto* v) { 
            v->move(); 
        }, s);
    }

    return 0;
}
```

# 14. C++23: this推导

这允许成员函数的this参数被显式地声明在函数参数列表中，从而使得成员函数可以像普通函数那样被重载和模板化。

+ 注1: 用这个技术，就少了 static_cast
+ 注2: gcc 13.2 尚不支持 this推导，下面网址用了 最新主干 gcc编译
+ 注3: 注意链接里面的 编译选项 `--std=C++23`

点击 [这里](https://godbolt.org/z/1bTvjqYz8) 运行代码

``` cpp
#include <iostream>

// Animal 是基类，D是模板化的子类
template<typename D>
class Animal {
public:
    // C++ 23 引入 this推导
    // 不需要 static_cast<D&>(*this);
    template<typename T>
    void move(this T&& self) { 
        self.move_impl();
    }
};

class Dog: public Animal<Dog> {
private:
    friend Animal;
    
    void move_impl() const {
        std::cout << "Dog is running." << std::endl;
    }
};

class Bird: public Animal<Bird> {
private:
    friend Animal;
    
    void move_impl() const {
        std::cout << "Bird is flying." << std::endl;
    }
};

class Fish: public Animal<Fish> {
private:
    friend Animal;

    void move_impl() const {
        std::cout << "Fish is swimming." << std::endl;
    }
};

int main() {
    Dog dog;
    dog.move();

    Bird bird;
    bird.move();

    Fish fish;
    fish.move();
    return 0;
}
```