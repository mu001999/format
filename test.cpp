#include "format.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(Format, Direct)
{
    ASSERT_EQ(fmt::format<"Hello, {}!">("world"), "Hello, world!");
    ASSERT_EQ(fmt::format<"The number is {}">(42), "The number is 42");
    ASSERT_EQ(fmt::format<"{1} to see you, {0}">("Mu00", "Nice"), "Nice to see you, Mu00");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
