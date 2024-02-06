# C++

# 1. C++ 11 / 14

## 1.1. 语言特性

+ static_assert
+ 初始化列表：花括号 初始化对象
+ 右值引用 / 移动语义 / 完美转发
+ lambda 表达式
+ auto / decltype / decltype(auto)
+ 基于范围 for循环
+ 属性: 例 [[noreturn]] / [[deprecated(“reason”)]]
+ 面向对象
    - 委托 构造函数
    - 继承 构造函数
    - 移动构造 / 移动赋值
    - 默认函数 / 删除函数：= default / = delete 
    - final / override
    - 非静态数据成员的默认初始化：允许在类定义中直接为非静态成员赋予初始值。
    - 枚举类（enum class）

## 1.2. 标准库

+ 移动语义 / 完美转发
    - std::move / std::forward
+ 智能指针
    - std::shared_ptr std::unique_ptr std::weak_ptr
    - std::make_unique / std::make_shared
+ 原子 <atomic>
+ 并发 / 多线程
    - std::thread
    - std::mutex、std::condition_variable
    - std::async
+ 函数 std::function / std::bind
+ 类型特征 <type_traits>
    - std::enable_if 根据编译时布尔表达式 启用/禁用 模板，C++ 20后 被 Concepts 代替。
    - 类型关系：std::is_same，用于检查两个类型是否相同。
    - 类型属性：std::is_const，std::is_volatile，std::is_integral，std::is_floating_point
    - 类型修改：提供转换类型的方式，std::remove_const，std::add_pointer
    - 类型分类：判断类型的类别，std::is_pointer，std::is_array，std::is_enum
    - 支持的操作 std::is_constructible，std::is_assignable
    - 属性查询：std::alignment_of，std::rank
+ 正则 <regex> 
+ 时间 std::chrono
+ 随机 <random> 
+ 容器
    - std::ref
    - std::array
    - std::unordered_map、std::unordered_set
    - std::tuple / std::tie
+ 算法
    - std::begin/end
    - std::all_of std::any_of std::none_of
    - std::copy_if / std::exchange

# 2. C++17

## 2.1. 语言特性

+ 结构化绑定：允许从tuple或结构体一次性解构多个变量。
+ constexpr

## 2.2. 标准库

+ std::optional
+ std::variant / std::any
+ std::filesystem
+ std::invoke / std::apply
+ std::sample / std::clamp / std::reduce
+ std::not_fn

# 3. C++20

## 3.1. 语言特性

+ `Concept`：引入了对模板类型约束的支持。
+ `Ranges`：提供了更现代化的方法来处理序列。
+ `协程`：引入了原生协程支持。
+ （不成熟）`模块`：引入模块的概念来替代传统的头文件和源文件模型，改善编译时间和代码组织。

## 3.2. 标准库

+ std::span
+ std::is_constant_evaluated

# 4. 第三方库

## 4.1. 特性

+ [C++ 中文 周刊](https://github.com/wanghenshui/cppweeklynews) 
+ [CppCoreGuidelines](https://github.com/lynnboy/CppCoreGuidelines-zh-CN/blob/master/CppCoreGuidelines-zh-CN.md) 规范指南
+ [modern-cpp-features](https://github.com/AnthonyCalandra/modern-cpp-features) C++ 11/14/17/20 特性

## 4.2. 数据结构

+ [Facebook Folly](https://github.com/facebook/folly)，基于C++ 17
+ [Google Abseil](https://abseil.io/docs/cpp/)

## 4.3. 实用库

+ [google test](https://github.com/google/googletest) 测试框架
+ [Catch2](https://github.com/catchorg/Catch2)测试框架
+ [spdlog](https://github.com/gabime/spdlog)快速、异步 日志库

## 4.4. 存储

+ [leveldb](https://github.com/google/leveldb)和 [rocksdb ](https://github.com/facebook/rocksdb)
    - lsm / sst
    - Skiplist / Arena

## 4.5. 异步

+ [Asio](https://think-async.com/Asio/)
+ [阿里 async_simple](https://github.com/alibaba/async_simple)  C++20

## 4.6. 网络

+ [chenshuo/muduo](https://github.com/chenshuo/muduo) 事件驱动的Linux 网络库
+ [30天自制C++服务器](https://github.com/yuesong-feng/30dayMakeCppServer)
+ [搜狗 Workflow](https://github.com/sogou/workflow)，并行计算，异步网络框架
+ [百度 bRPC 框架](https://github.com/apache/brpc)

## 4.7. 渲染

+ [NV 数学库](https://github.com/NVIDIAGameWorks/MathLib)
+ [NV Donut: DX11 / DX12 / Vulkan](https://github.com/NVIDIAGameWorks/donut)
+ [NV NRI: DX11 / DX12 / Vulkan](https://github.com/NVIDIAGameWorks/NRI)
+ [NVRhi: DX11 / DX12 / Vulkan](https://github.com/NVIDIAGameWorks/nvrhi)
+ [NVidia Sample](https://github.com/nvpro-samples)
+ [Falcor](https://github.com/NVIDIAGameWorks/Falcor)
+ [d3d12-book](https://github.com/d3dcoder/d3d12book.git)
+ [DirectX-Graphics-Samples](https://github.com/microsoft/DirectX-Graphics-Samples)
+ [DirectX12 Render](https://github.com/Bubble888/DirectX12Renderer.git)
+ [GRS D3D-12 Sample](https://github.com/GamebabyRockSun/GRSD3D12Sample.git)
+ [Learning DirectX 12](https://github.com/jpvanoosten/LearningDirectX12.git)
+ [Anvil](https://github.com/GPUOpen-Archive/Anvil) Vulkan 
+ [Vulkan-Sampler](https://github.com/SaschaWillems/Vulkan.git) 
+ [Google filament](https://github.com/google/filament)
+ [腾讯 tgfx](https://github.com/Tencent/tgfx) 2D-图形框架
+ [The-Forge](https://github.com/ConfettiFX/The-Forge.git) 跨平台渲染框架

## 4.8. 游戏

+ [box2d ](https://github.com/erincatto/box2d)物理引擎
+ [flecs: ecs框架](https://github.com/SanderMertens/flecs)
+ [entt: ecs框架](https://github.com/skypjack/entt)
+ [acl 动画压缩](https://github.com/nfrechette/acl) 
+ [godot ](https://github.com/godotengine/godot)游戏引擎
+ [Unreal ](https://github.com/EpicGames/UnrealEngine)游戏引擎
+ [recast-navigation](https://github.com/recastnavigation) 网格导航