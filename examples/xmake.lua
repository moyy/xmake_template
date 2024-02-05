-- xmake build math_example 
-- xmake run math_example 

target("math_example")
    set_kind("binary") -- 设置为可执行文件
    add_deps("math")  -- 添加依赖
    set_default(false)
    add_files("math/*.cpp") -- 添加源文件