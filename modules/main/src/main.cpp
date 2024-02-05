#include "e_hello.h"
#include "e_math.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    hello();

    int r = e_math::add(1, 2);
    std::cout << "add(1, 2) = " << r << std::endl;
    
    return 0;
}