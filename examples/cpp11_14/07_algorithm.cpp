// 新增算法
// 
// + std::all_of, std::any_of, std::none_of
// + std::find_if_not
// + std::copy_if
// + std::minmax_element
// + std::is_sorted, std::is_sorted_until

#include <vector>
#include <algorithm>
#include <iostream>

void test_all_any_none() {
    std::vector<int> v {1, 3, 5, 7, 9}; 
    // 全部满足条件,返回 true
    if (std::all_of(v.cbegin(), v.cend(), [](auto &elem) { return elem % 2 == 1; })) {
        std::cout << "all number are odd" << std::endl;
    }

    // 有一个 满足条件,返回 true
    if (std::any_of(v.cbegin(), v.cend(), [](auto &elem) { return elem > 5; })) {
        std::cout << "has number > 5" << std::endl;
    }

    // 没有一个 满足条件,返回 true
    if (std::none_of(v.cbegin(), v.cend(), [](auto &elem) { return elem > 9; })) {
        std::cout << "none of number > 9" << std::endl;
    }
}

// min element at: 1
// max element at: 9
void test_minmax() {
    std::vector<int> v {3, 5, 1, 9, 7}; 

    auto result = std::minmax_element(v.begin(), v.end());
    
    std::cout << "min element at: " << *(result.first) << '\n';
    std::cout << "max element at: " << *(result.second) << '\n';
}

void test_is_sorted() {
    std::vector<int> v1 {3, 5, 1, 9, 7}; 
    if (std::is_sorted(v1.begin(), v1.end())) {
        std::cout << "v1 is sorted\n";
    }

    std::vector<int> v2 {1, 3, 5, 7, 9}; 
    // 打印, 默认 升序
    if (std::is_sorted(v2.begin(), v2.end())) {
        std::cout << "v2 is sorted\n";
    }

    std::vector<int> v3 {9, 7, 5, 3, 1}; 
    if (std::is_sorted(v3.begin(), v3.end())) {
        std::cout << "v3 is sorted\n";
    }
}

int main() {
    test_all_any_none();
    test_minmax();
    test_is_sorted();
    return 0;
}