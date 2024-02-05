target("utils")
    set_kind("static") -- 设置为静态库
    add_files("src/*.cpp") -- 添加源文件
    add_includedirs("include", {public = true}) -- 添加头文件目录