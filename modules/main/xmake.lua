target("xmake_template")
    set_kind("binary") -- 设置为可执行文件
    add_deps("utils", "math")  -- 添加依赖
    add_files("src/*.cpp") -- 添加源文件
    add_includedirs("include") -- 添加头文件目录
    