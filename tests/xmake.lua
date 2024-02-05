add_requires("gtest")

-- xmake build tests
-- xmake run tests

target("tests")
    set_kind("binary")
    add_deps("math")
    add_packages("gtest")
    set_default(false)
    add_files("**.cpp")