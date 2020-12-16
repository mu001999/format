#ifndef __FORMAT_HPP__
#define __FORMAT_HPP__

#include <string>
#include <cstddef>

namespace fmt
{
namespace details
{
template<std::size_t N>
struct fixed_string
{
    char buf[N] {};
    constexpr fixed_string(const char (&str)[N])
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            buf[i] = str[i];
        }
    }
};
} // namespace details

template<details::fixed_string pattern>
class format
{
  public:
    template<typename ...Args>
    static std::string fmt(Args &&...args)
    {
        return fmt_impl<0>(std::forward<Args>(args)...);
    }

  private:
    template<std::size_t index>
    static std::string fmt_impl()
    {
        return pattern.buf + index;
    }

    template<std::size_t index, typename Arg, typename ...Args>
    static std::string fmt_impl(Arg &&arg, Args &&...args)
    {
        if constexpr (pattern.buf[index] == '%')
        {
            constexpr char mode = pattern.buf[index + 1];
            if constexpr (mode == 's')
            {
                static_assert(std::is_same_v<std::decay_t<Arg>, std::string> || std::is_convertible_v<std::decay_t<Arg>, const char *>);
                return arg + fmt_impl<index + 2>(std::forward<Args>(args)...);
            }
            else if constexpr (mode == 'd')
            {
                static_assert(std::is_convertible_v<std::decay_t<Arg>, int>);
                return std::to_string(arg) + fmt_impl<index + 2>(std::forward<Args>(args)...);
            }
        }
        else
        {
            return pattern.buf[index] + fmt_impl<index + 1>(std::forward<Arg>(arg), std::forward<Args>(args)...);
        }
    }
};
} // namespace fmt

#endif
