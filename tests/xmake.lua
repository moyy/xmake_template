add_requires("gtest")

-- xmake build test_utils
-- xmake run test_utils

target("test_utils")
    set_kind("binary")
    add_deps("math")
    add_packages("gtest")
    set_default(false)
    add_files("**.cpp")