#include "format.hpp"
#include <iostream>

int main()
{
    std::cout << fmt::format<"%d %s">::fmt(42, "Mu00") << std::endl;
    std::cout << fmt::format<"%d">::fmt("test") << std::endl;
}
