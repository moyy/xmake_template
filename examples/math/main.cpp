#include "e_math.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    int r = e_math::add(1, 2);
    std::cout << "example_math, add(1, 2) = " << r << std::endl;
    
    return 0;
}