# xmake

# 1. 构建需求

+ C/C++
+ 跨平台：Windows / Linux / MacOS / Android / iOS / WASM
+ 监听 添加 / 删除 / 重命名 文件和目录
+ 管理 第三方库
+ 进阶：构建性能 / 云构建 等

# 2. 目录结构

+ README.md
+ modules/ 一个子目录 代表 库 / 可执行项目
    - math/ 静态库
        * src/
        * include/
        * README.md
+ tests/     针对 modules/ 的测试，使用 GoogleTest 框架
+ examples/  例子，每个子目录一个 可执行 项目
+ documents/ 文档
+ third_party/ 第三方库，比如 GoogleTest，FreeType 等

# 3. 头文件规范

+ module名_MSVC_DLL 只有 生成/使用 MSVC DLL
+ module名_EXPORT_MSVC_DLL 只有 生成 MSVC DLL

``` cpp
#ifndef MATH_INTERFACE_H // module名_头文件名_H
#define MATH_INTERFACE_H

#ifdef MATH_MSVC_DLL 
    #ifdef MATH_EXPORT_MSVC_DLL
        #define MATH_API __declspec(dllexport)
    #else
        #define MATH_API __declspec(dllimport)
    #endif // MATH_EXPORT_MSVC_DLL
#else
    #define MATH_API
#endif // MATH_MSVC_DLL

extern "C" {
    MATH_API int add(int a, int b);

    MATH_API int sub(int a, int b);
}

#endif // MATH_INTERFACE_H
```

# 3. 基本命令

+ xmake create 项目名
+ xmake clean 清理构建目录
+ 配置 xmake f / xmake config
    - “-m debug” 指示 xmake 构建时 用 debug
+ 构建 xmake 
+ 运行 xmake run
    - “-d“ 开启调试器

# 4. 生成项目文件 xmake project

+ “-k vsxmake / xcode / make / cmake / ninja"
+ "-m debug,release"
+ "-a x64,x86,x86_64,arm64"
    - windows: x64
    - macos x86_64
+ "-o 输出目录"

