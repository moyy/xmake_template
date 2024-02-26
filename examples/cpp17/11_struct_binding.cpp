// 通过结构化绑定，对于tuple、map等类型，获取相应值会方便很多

#include <map>
#include <string>
#include <tuple>
#include <iostream>

std::tuple<int, double> func() {
    return std::tuple(1, 2.2);
}

void print_map() {
    std::map<int, std::string> m = {
      {0, "a"},
      {1, "b"},  
    };

    for (const auto &[i, s]: m) {
        std::cout << i << " " << s << std::endl;
    }
}

int main() {
    print_map(); 
    
    auto[i1, d1] = func();
    std::cout << i1 << " " << d1 << std::endl; // 1 2.2
    
    std::pair a(1, 2.3f);
    auto[i2, f] = a;
    std::cout << i2 << " " << f << std::endl; // 1 2.3
    return 0;
}
