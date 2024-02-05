target("math")
    set_kind("shared") -- 设置为动态库
    add_deps("utils")  -- 添加依赖
    add_files("src/*.cpp") -- 添加源文件
    add_includedirs("include", {public = true}) -- 添加头文件目录
    if is_plat("windows") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end