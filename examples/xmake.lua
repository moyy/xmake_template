-- xmake build example_math 
-- xmake run example_math 

target("example_math")
    set_kind("binary") -- 设置为可执行文件
    add_deps("math")  -- 添加依赖
    set_default(false)
    add_files("math/*.cpp") -- 添加源文件