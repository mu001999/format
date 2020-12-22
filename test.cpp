#include "format.hpp"
#include <iostream>

int main()
{
    std::cout << fmt::format<"Hello, {}!">("world") << std::endl;
    std::cout << fmt::format<"{1} to see you, {0}">("Mu00", "Nice") << std::endl;
}
