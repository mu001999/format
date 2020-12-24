#include "format.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(StaticFormat, Direct)
{
    ASSERT_EQ(fmt::format<"Hello, {}!">("world"), "Hello, world!");
    ASSERT_EQ(fmt::format<"The number is {}">(42), "The number is 42");
}

TEST(StaticFormat, Positional)
{
    ASSERT_EQ(fmt::format<"{1} to see you, {0}">("Mu00", "Nice"), "Nice to see you, Mu00");
    ASSERT_EQ(fmt::format<"{1} {} {0} {}">(1, 2), "2 1 1 2");
    ASSERT_EQ(fmt::format<"{2} {1} {0}">(0, 1, 2), "2 1 0");
}

TEST(StaticFormat, Width)
{
    ASSERT_EQ(fmt::format<"{:5}!">("x"),      "x    !");
    ASSERT_EQ(fmt::format<"{:1$}!">("x", 5),  "x    !");
    ASSERT_EQ(fmt::format<"{1:0$}!">(5, "x"), "x    !");

    int width = 5;
    ASSERT_EQ(fmt::format<"{1:0$}!">(width, "x"), "x    !");
}

TEST(StaticFormat, Fill_Alignment)
{
    ASSERT_EQ(fmt::format<"{:<5}!">("x"),  "x    !");
    ASSERT_EQ(fmt::format<"{:-<5}!">("x"), "x----!");
    ASSERT_EQ(fmt::format<"{:^5}!">("x"),  "  x  !");
    ASSERT_EQ(fmt::format<"{:>5}!">("x"),  "    x!");
}

TEST(StaticFormat, Sign_Type_Padding)
{
    ASSERT_EQ(fmt::format<"{:+}!">(5),      "+5!");
    ASSERT_EQ(fmt::format<"{:#x}!">(27),    "0x1b!");
    ASSERT_EQ(fmt::format<"{:05}!">(5),     "00005!");
    ASSERT_EQ(fmt::format<"{:05}!">(-5),    "-0005!");
    ASSERT_EQ(fmt::format<"{:#010x}!">(27), "0x0000001b!");
}

TEST(StaticFormat, Escaping)
{
    ASSERT_EQ(fmt::format<"{{}">(), "{}");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
