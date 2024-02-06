-- xmake build example_math 
-- xmake run example_math 
target("example_math")
    set_kind("binary") -- 设置为可执行文件
    add_deps("math")  -- 添加依赖
    set_default(false)
    add_files("math/*.cpp") -- 添加源文件

-- 定义一个函数来为指定目录下的每个.cpp文件创建target
function add_example_targets(dir)
    for _, cppfile in ipairs(os.files(dir .. "/*.cpp")) do
        -- 获取不带扩展名的文件名
        local basename = path.basename(cppfile)
        
        -- 创建target名，添加前缀 "example_"
        local targetname = "example_" .. basename

        -- 定义target
        target(targetname)
            set_kind("binary") -- 设置为可执行文件
            set_default(false) -- 设置为非默认构建目标
            add_files(cppfile) -- 添加当前遍历到的cpp文件
    end
end

-- 调用函数为指定目录创建targets
-- 构建 xmake build example_`CPP文件名`
-- 运行 xmake run example_`CPP文件名`
add_example_targets("cpp11_14")
add_example_targets("cpp17")
add_example_targets("cpp20")