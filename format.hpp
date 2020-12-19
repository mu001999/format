#ifndef __FORMAT_HPP__
#define __FORMAT_HPP__

#include <string>
#include <cstddef>

#if __cplusplus <= 201703L
#error only support in C++20
#endif

namespace fmt
{
namespace details
{
template<std::size_t N>
class fixed_string
{
  public:
    constexpr fixed_string(const char (&str)[N])
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            data_[i] = str[i];
        }
    }

    constexpr char operator[](std::size_t i) const
    {
        return data_[i];
    }

    constexpr std::size_t size() const
    {
        return N;
    }

    constexpr const char *data() const
    {
        return data_;
    }

  private:
    char data_[N] {};
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
        return pattern.data() + index;
    }

    template<std::size_t index, typename Arg, typename ...Args>
    static std::string fmt_impl(Arg &&arg, Args &&...args)
    {
        static_assert(index < pattern.size(), "Too much arguments");

        if constexpr (pattern[index] == '%')
        {
            constexpr char mode = pattern[index + 1];
            if constexpr (mode == 's')
            {
                static_assert(std::is_convertible_v<std::decay_t<Arg>, std::string> || std::is_convertible_v<std::decay_t<Arg>, const char *>,
                    "%s needs type could be convertible to const char * or std::string"
                );
                return arg + fmt_impl<index + 2>(std::forward<Args>(args)...);
            }
            else if constexpr (mode == 'd')
            {
                static_assert(std::is_convertible_v<std::decay_t<Arg>, int>,
                    "$d needs type could be convertible to int"
                );
                return std::to_string(arg) + fmt_impl<index + 2>(std::forward<Args>(args)...);
            }
        }
        else
        {
            return pattern[index] + fmt_impl<index + 1>(std::forward<Arg>(arg), std::forward<Args>(args)...);
        }
    }
};
} // namespace fmt

#endif
