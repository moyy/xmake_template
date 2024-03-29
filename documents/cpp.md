# 1. C++ 11 / 14

## 1.1. 语言特性

+ `constexpr` / `static_assert`
+ 初始化列表：大括号
+ `char16_t` / `char32_t` / `long long`
+ `alignof` / `alignas`
+ 原始字符串
    - R"delimiter(原始字符串)delimiter"
+ Lambda 表达式
+ `noexcept`
+ `auto` / `decltype`
+ 基于范围for循环: for (auto &item: v) { }
+ 属性
    - [[noreturn]] 指示函数不可能返回
    - [[deprecated(“reason”)]] 指示 废弃
+ 枚举类 `enum class`
+ 面向对象
    - `explicit`
    - 委托 构造函数
    - 继承 构造函数
    - 移动构造 / 移动赋值
    - 默认函数 / 删除函数：`=default` / `=delete`
    - `final` / `override`
    - 非静态数据成员 默认初始化

## 1.2. 标准库

+ 移动语义 / 完美转发
    - std::move / std::forward
+ 智能指针
    - std::unique_ptr / std::make_unique
    - std::shared_ptr / std::make_shared
    - std::weak_ptr
+ 并发 / 多线程
    - `thread_local`
    - std::thread / std::call_once
    - std::mutex / std::lock
    - std::atomic
    - std::condition_variable
    - std::async / std:future
+ 函数 std::function / std::bind
+ 类型特征 <type_traits>
    - std::enable_if /std::is_base_of 根据编译时布尔表达式 启用/禁用 模板，C++ 20后 被 Concepts 代替。
    - 类型关系：std::is_same，用于检查两个类型是否相同。
    - 类型属性：std::is_const，std::is_volatile，std::is_integral，std::is_floating_point
    - 类型修改：提供转换类型的方式，std::remove_const，std::add_pointer
    - 类型分类：判断类型的类别，std::is_pointer，std::is_array，std::is_enum
    - 支持的操作 std::is_constructible，std::is_assignable
    - 属性查询：std::alignment_of，std::rank
+ 正则 std::regex
+ 时间 std::chrono
+ 随机 random
+ 容器
    - std::array / std::forward_list
    - std::unordered_map、std::unordered_set
    - std::tuple / std::tie / std::get / std::make_tuple
+ 算法
    - std::all_of / std::any_of / std::none_of
    - std::copy_if
    - std::exchange

# 2. C++ 17

## 2.1. 语言特性

+ if / switch 能用 初始化器
+ 枚举 直接列表初始化
+ 对 强类型枚举 直接列表初始化
+ `constexpr` if 基于编译时条件的分支
+ constexpr Lambda
+ Lambda 按值捕获 `*this` 
+ 类模板参数推导
+ 非类型模板参数的 `auto`
+ 折叠表达式（Folding Expressions）
+ 从 初始化列表 自动推断 `auto`
+ 用 `inline` 声明变量
+ 嵌套命名空间 namespace A::B::C
+ 结构化绑定（Structured Bindings）
+ `__has_include` 预处理指令，用于检测特定头文件是否存在
+ 属性
    - [[fallthrough]]：指示编译器 忽略switch语句中的 case 贯穿警告
    - [[nodiscard]]：强调 函数返回值 不应被忽略
    - [[maybe_unused]]：指示编译器 忽略未使用 实体 警告

## 2.2. 标准库

+ std::filesystem
+ std::optional
+ std::variant / std::any
+ std::invoke / std::apply
+ std::string_view
+ std::sample / std::clamp / std::reduce
+ 某些 并行算法

# 3. C++20

## 3.1. 语言特性 

+ `Concept`：引入了对模板类型约束的支持。
+ `Ranges`：提供了更现代化的方法来处理序列。
+ `协程`：引入了原生协程支持。
+ （不成熟）`模块`：引入模块的概念来替代传统的头文件和源文件模型，改善编译时间和代码组织。

## 3.2. 标准库

+ Concepts 库
+ std::span
+ std::is_constant_evaluated
+ string: starts_with / ends_with
+ std::bit_cast
+ std::midpoint
+ std::to_array

# 4. 库

## 4.1. 特性

+ [C++ 中文 周刊](https://github.com/wanghenshui/cppweeklynews)
+ [`CppCoreGuidelines`](https://github.com/lynnboy/CppCoreGuidelines-zh-CN/blob/master/CppCoreGuidelines-zh-CN.md) 规范指南
+ [modern-cpp-features](https://github.com/AnthonyCalandra/modern-cpp-features) C++ 11/14/17/20 特性

## 4.2. 数据结构

+ [Facebook `Folly`](https://github.com/facebook/folly)，基于C++ 17
+ [Google `Abseil`](https://abseil.io/docs/cpp/)

## 4.3. 实用库

+ [`google test`](https://github.com/google/googletest) 测试框架
+ [`spdlog`](https://github.com/gabime/spdlog)快速、异步 日志库

## 4.4. 存储

+ [Google `LevelDB`](https://github.com/google/leveldb)和 [rocksdb ](https://github.com/facebook/rocksdb)
    - lsm / sst
    - Skiplist / Arena

## 4.5. 异步

+ [`Asio`](https://think-async.com/Asio/)
+ [阿里 `async_simple`](https://github.com/alibaba/async_simple) C++20

## 4.6. 网络

+ [chenshuo/`muduo`](https://github.com/chenshuo/muduo) 事件驱动的Linux 网络库
+ [30天自制C++服务器](https://github.com/yuesong-feng/30dayMakeCppServer)
+ [搜狗 `Workflow`](https://github.com/sogou/workflow)，并行计算，异步网络框架
+ [百度 `bRPC` 框架](https://github.com/apache/brpc)

## 4.7. 渲染

+ DX12
    - [d3d12-book](https://github.com/d3dcoder/d3d12book.git)
    - [DirectX-Graphics-Samples](https://github.com/microsoft/DirectX-Graphics-Samples)
+ Vulkan
    - [Vulkan Sampler](https://github.com/KhronosGroup/Vulkan-Samples)
    - [Vulkan Scene Graph](https://github.com/vsg-dev/VulkanSceneGraph)
+ NVida
    - [`nvrhi`](https://github.com/NVIDIAGameWorks/nvrhi) DX11 / DX12 / Vulkan
    - [`Donut`](https://github.com/NVIDIAGameWorks/donut) 实时渲染器 DX11 / DX12 / Vulkan
    - [`Falcor`](https://github.com/NVIDIAGameWorks/Falcor) 实时渲染器 DX12 / Vulkan
+ [FrameGraph](https://github.com/azhirnov/FrameGraph) 基于 Vulkan的渲染图
+ [`Sakura` Engine](https://github.com/SakuraEngine/SakuraEngine)
+ [Google `Filament`](https://github.com/google/filament) PBR 渲染器
+ [腾讯 `tgfx`](https://github.com/Tencent/tgfx) 2D图形框架 
+ [`The-Forge`](https://github.com/ConfettiFX/The-Forge.git) 跨平台渲染框架

## 4.8. 游戏

+ [`box2d`](https://github.com/erincatto/box2d) 2D物理引擎
+ [`flecs`](https://github.com/SanderMertens/flecs) ecs框架
+ [`acl`](https://github.com/nfrechette/acl) 动画压缩
+ [`godot`](https://github.com/godotengine/godot) 游戏引擎
+ [`Unreal`](https://github.com/EpicGames/UnrealEngine) 游戏引擎
+ [`recast-navigation`](https://github.com/recastnavigation/recastnavigation) 网格导航