#include <gtest/gtest.h>

#include "e_math.hpp"

TEST(E_Math, Add) {
    EXPECT_EQ(e_math::add(1, 2), 3); 
    EXPECT_EQ(e_math::add(-1, 1), 0);
}