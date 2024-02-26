add_rules("mode.debug", "mode.release")

-- c++ 20 标准
set_languages("c++20")

-- 仅对 MSVC 编译器添加 /Zc:__cplusplus 选项
if is_plat("windows") then
    add_cxflags("/Zc:__cplusplus", {force = true})
end

-- 引入 模块
includes("modules/utils")
includes("modules/math")
includes("modules/main")

includes("tests")

includes("examples")


-- 注：这里每个模块 的 写法 注意 先后顺序，必须是：set_kind, add_deps, 后面就是添加 源文件，头文件搜索
-- target("xmake_template")
--     set_kind("binary") -- 设置为可执行文件
--     add_deps("utils", "math")  -- 添加依赖
--     add_files("src/*.cpp") -- 添加源文件
--     add_includedirs("include") -- 添加头文件目录

-- 注：如果要在DLL导出符号，放到最后；
-- target("math")
--     set_kind("shared") -- 设置为动态库
--     add_deps("utils")  -- 添加依赖
--     add_files("src/*.cpp") -- 添加源文件
--     add_includedirs("include", {public = true}) -- 添加头文件目录
--     if is_plat("windows") then
--         add_rules("utils.symbols.export_all", {export_classes = true})
--     end
