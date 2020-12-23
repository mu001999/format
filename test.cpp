#include "format.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(Format, Direct)
{
    ASSERT_EQ(fmt::format<"Hello, {}!">("world"), "Hello, world!");
    ASSERT_EQ(fmt::format<"The number is {}">(42), "The number is 42");
}

TEST(Format, Positional)
{
    ASSERT_EQ(fmt::format<"{1} to see you, {0}">("Mu00", "Nice"), "Nice to see you, Mu00");
    ASSERT_EQ(fmt::format<"{1} {} {0} {}">(1, 2), "2 1 1 2");
    ASSERT_EQ(fmt::format<"{2} {1} {0}">(0, 1, 2), "2 1 0");
}

TEST(Format, Escaping)
{
    ASSERT_EQ(fmt::format<"Hello {{}">(), "Hello {}");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
