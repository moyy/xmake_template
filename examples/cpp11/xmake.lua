-- 遍历 所有.cpp文件
for _, cppfile in ipairs(os.files("*.cpp")) do
    -- 获取不带扩展名的文件名作为target名
    local targetname = path.basename(cppfile)
    
    -- 定义target
    target(targetname)
        set_kind("binary") -- 设置为可执行文件
        add_files(cppfile) -- 添加当前遍历到的cpp文件
end