#include <iostream>

enum class AColor {
    kRed,
    kGreen,
    kBlue
};

enum class BColor {
    kWhite,
    kBlack,
    kYellow
};

int main() {
    // if (AColor::kRed == BColor::kWhite) { } // 编译失败
    if (AColor::kRed == AColor::kGreen) {
        std::cout << "red == white" << std::endl;
    }
    return 0;
}