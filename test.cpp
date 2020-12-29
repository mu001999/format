#include "format.hpp"
#include <set>
#include <list>
#include <vector>
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
    ASSERT_EQ(fmt::format<"{:#X}!">(27),    "0x1B!");
    ASSERT_EQ(fmt::format<"{:#b}!">(27),    "0b11011!");
    ASSERT_EQ(fmt::format<"{:#o}!">(27),    "0o33!");
    ASSERT_EQ(fmt::format<"{:#o}!">(-27),   "-0o33!");
    ASSERT_EQ(fmt::format<"{:05}!">(5),     "00005!");
    ASSERT_EQ(fmt::format<"{:05}!">(-5),    "-0005!");
    ASSERT_EQ(fmt::format<"{:#010x}!">(27), "0x0000001b!");
    ASSERT_EQ(fmt::format<"{:.5}!">(5.0),   "5.00000!");

    int x = -27;
    ASSERT_EQ(fmt::format<"{:#x}!">(static_cast<unsigned int>(x)), "0xffffffe5!");
}

TEST(StaticFormat, Escaping)
{
    ASSERT_EQ(fmt::format<"{{}">(), "{}");
}

TEST(StaticFormat, Pair)
{
    ASSERT_EQ(fmt::format<"Hello, {}!">(make_pair(1, 2)), "Hello, (1, 2)!");
}

TEST(StaticFormat, Tuple)
{
    ASSERT_EQ(fmt::format<"Hello, {}!">(make_tuple(1, 2, 3)), "Hello, (1, 2, 3)!");
}

TEST(StaticFormat, Container)
{
    ASSERT_EQ(fmt::format<"Hello, {}!">(std::set{1, 2, 3}), "Hello, {1, 2, 3}!");
    ASSERT_EQ(fmt::format<"Hello, {}!">(std::list{1, 2, 3}), "Hello, {1, 2, 3}!");
    ASSERT_EQ(fmt::format<"Hello, {}!">(std::vector{1, 2, 3}), "Hello, {1, 2, 3}!");

    std::vector vec
    {
        std::vector{1, 2},
        std::vector{2, 3},
        std::vector{3, 4}
    };
    ASSERT_EQ(fmt::format<"Hello, {}!">(vec), "Hello, {{1, 2}, {2, 3}, {3, 4}}!");
}

template<fmt::details::FixedString pattern, typename ...Args>
string print_v(Args &&...args)
{
    testing::internal::CaptureStdout();
    fmt::print<pattern>(forward<Args>(args)...);
    return testing::internal::GetCapturedStdout();
}

TEST(StaticPrint, All)
{
    ASSERT_EQ(print_v<"Hello, {}!">("world"), "Hello, world!");
    ASSERT_EQ(print_v<"The number is {}">(42), "The number is 42");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
