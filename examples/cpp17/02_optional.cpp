#include <string>
#include <iostream>
#include <optional> // C++ 17

std::optional<int> s_to_i(const std::string &s) {
    try {
        return std::stoi(s);
    } catch(...) {
        return std::nullopt;
    }
}

int main() {
    std::string s{"123"};
    std::optional<int> o = s_to_i(s);

    if (o) {
        std::cout << *o << std::endl;
    } else {
        std::cout << "error" << std::endl;
    }

    return 0;
}