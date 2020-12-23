#ifndef __FORMAT_HPP__
#define __FORMAT_HPP__

#include <tuple>
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
struct FixedString
{
    static constexpr std::size_t size = N - 1;
    char data[size] {};

    constexpr FixedString(const char (&str)[N])
    {
        for (std::size_t i = 0; i < size; ++i)
        {
            data[i] = str[i];
        }
    }

    constexpr char operator[](std::size_t i) const
    {
        return data[i];
    }

    constexpr std::size_t find(char chr, std::size_t start = 0, std::size_t end = size) const
    {
        for (; start < end; ++start)
        {
            if (data[start] == chr)
            {
                return start;
            }
        }
        return size;
    }
};

struct Argument
{
    Argument() = default;
};

template<typename T>
inline std::string as_string(T &&arg, Argument argument = Argument())
{
    if constexpr (std::is_convertible_v<std::decay_t<T>, const char *>)
    {
        return arg;
    }
    else if constexpr (std::is_convertible_v<std::decay_t<T>, std::string>)
    {
        return arg;
    }
    else
    {
        return std::to_string(arg);
    }
}

template<FixedString pattern>
constexpr std::size_t as_size_t(std::size_t begin, std::size_t end)
{
    std::size_t res = 0;
    for (; begin < end; ++begin)
    {
        if (pattern[begin] < '0' || pattern[begin] > '9')
        {
            return -1;
        }
        res += res * 10 + pattern[begin] - '0';
    }
    return res;
}

template<FixedString pattern>
constexpr Argument as_argument(std::size_t begin, std::size_t end)
{
    
}

template<FixedString pattern, std::size_t i, std::size_t arg_ind, typename ...Args>
inline std::string format_impl(const std::tuple<Args...> &args)
{
    using Tuple = std::tuple<Args...>;

    static_assert(i <= pattern.size, "Out of range");

    if constexpr (i == pattern.size)
    {
        return std::string();
    }
    else if constexpr (pattern[i] == '{')
    {
        static_assert(i + 1 < pattern.size, "Use {{ or {...} but not only {");

        // escaping {
        if constexpr (pattern[i + 1] == '{')
        {
            return '{' + format_impl<pattern, i + 2, arg_ind>(args);
        }
        // simplest placeholder {}
        else if constexpr (pattern[i + 1] == '}')
        {
            static_assert(arg_ind < std::tuple_size_v<Tuple>, "Too few arguments");
            return as_string(std::get<arg_ind>(args)) + format_impl<pattern, i + 2, arg_ind + 1>(args);
        }
        else
        {
            constexpr auto end_pos = pattern.find('}', i + 1);
            static_assert(end_pos != pattern.size, "Use {{ or {...} but not only {");

            constexpr auto colon_pos = pattern.find(':', i + 1, end_pos);
            if constexpr (colon_pos < end_pos)
            {
                constexpr auto position = as_size_t<pattern>(i + 1, colon_pos);
                static_assert(position != -1, "Invalid number in {:}");
                static_assert(position < std::tuple_size_v<Tuple>, "Positional parameters not match");
                return as_string(std::get<position>(args), as_argument<pattern>(colon_pos + 1, end_pos))
                    + format_impl<pattern, end_pos + 1, arg_ind>(args)
                ;
            }
            else
            {
                constexpr auto position = as_size_t<pattern>(i + 1, end_pos);
                static_assert(position != -1, "Invalid number in {}");
                static_assert(position < std::tuple_size_v<Tuple>, "Positional parameters not match");
                return as_string(std::get<position>(args)) + format_impl<pattern, end_pos + 1, arg_ind>(args);
            }
        }
    }
    else
    {
        constexpr auto pos = pattern.find('{', i + 1);
        if constexpr (pos == pattern.size)
        {
            return std::string(pattern.data + i, pos - i);
        }
        else
        {
            return std::string(pattern.data + i, pos - i) + format_impl<pattern, pos, arg_ind>(args);
        }
    }
}
} // namespace details

template<details::FixedString pattern, typename ...Args>
inline std::string format(Args &&...args)
{
    return details::format_impl<pattern, 0, 0>(std::make_tuple(std::forward<Args>(args)...));
}
} // namespace fmt

#endif
