#ifndef __FORMAT_HPP__
#define __FORMAT_HPP__

#include <tuple>
#include <string>
#include <cstddef>
#include <stdexcept>

#if __cplusplus <= 201703L
#error only support in C++20
#endif

namespace fmt
{
namespace details
{
constexpr bool is_align(char chr)
{
    return chr == '<' || chr == '^' || chr == '>';
}

constexpr bool is_sign(char chr)
{
    return chr == '+' || chr == '-';
}

constexpr bool is_digit(char chr)
{
    return '0' <= chr && chr <= '9';
}

constexpr bool is_type(char chr)
{
    return chr == 'x' || chr == 'X' || chr == 'b' || chr == 'o';
}

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
        return end;
    }

    constexpr std::size_t find_consequent_digit(std::size_t start = 0, std::size_t end = size) const
    {
        for (; start < end; ++start)
        {
            if (!is_digit(data[start]))
            {
                return start;
            }
        }
        return end;
    }
};

struct Spec
{
    enum Mode : std::uint8_t
    {
        Default     =   0,
        Align       =   1 << 0,
        Sign        =   1 << 1,
        Type        =   1 << 2,
        Padding     =   1 << 3,
        Width       =   1 << 4,
        Precision   =   1 << 5,
        WidthArg    =  (1 << 4) + (1 << 6),
        PreciArg    =  (1 << 5) + (1 << 7),
    };

    std::uint8_t mode;
    char         fill       = 0;
    char         align      = 0;
    char         sign       = 0;
    char         type       = 0;
    std::size_t  width      = 0;
    std::size_t  precision  = 0;

    // for Default
    constexpr Spec() : mode(Default)
    {
        // ...
    }

    // for Align
    constexpr Spec(const Spec &spec, char fill, char align)
      : mode(spec.mode | Align)
      , fill(fill), align(align)
      , sign(spec.sign), type(spec.type)
      , width(spec.width), precision(spec.precision)
    {
        // ...
    }

    // for Sign
    constexpr Spec(const Spec &spec, char sign)
      : mode(spec.mode | Sign)
      , fill(spec.fill), align(spec.align)
      , sign(sign), type(spec.type)
      , width(spec.width), precision(spec.precision)
    {
        // ...
    }

    // for Type
    constexpr Spec(char type) : mode(Type), type(type) {}

    // for Padding
    constexpr Spec(const Spec &spec, Mode m)
      : mode(spec.mode | Padding)
      , fill(spec.fill), align(spec.align)
      , sign(spec.sign), type(spec.type)
      , width(spec.width), precision(spec.precision)
    {
        if (m != Padding)
        {
            throw std::invalid_argument("Error! Please report!");
        }
    }

    // for Width/Precision
    constexpr Spec(const Spec &spec, Mode m, std::size_t value)
      : mode(spec.mode | m)
      , fill(spec.fill), align(spec.align)
      , sign(spec.sign), type(spec.type)
      , width(spec.width), precision(spec.precision)
    {
        if (m & Width)
        {
            width = value;
        }
        else if (m & Precision)
        {
            precision = value;
        }
        else
        {
            throw std::invalid_argument("Error! Please report!");
        }
    }

    constexpr bool is_default() const
    {
        return mode == Default;
    }

    constexpr bool has_set(Mode m) const
    {
        return (mode & m) == m;
    }
};

template<Spec spec = Spec(), typename T, typename ...Args>
inline std::string as_string(T &&arg, const std::tuple<Args...> &args)
{
    using RT = std::decay_t<T>;

    std::string result;
    if constexpr (std::is_convertible_v<RT, const char *>)
    {
        result = arg;
    }
    else if constexpr (std::is_convertible_v<RT, std::string>)
    {
        result = arg;
    }
    else if constexpr (!spec.has_set(Spec::Type))
    {
        result = std::to_string(arg);
    }

    if constexpr (spec.is_default())
    {
        return result;
    }

    std::size_t width;
    if constexpr (spec.has_set(Spec::WidthArg))
    {
        width = std::get<spec.width>(args);
    }
    else
    {
        width = spec.width;
    }

    std::size_t precision;
    if constexpr (spec.has_set(Spec::PreciArg))
    {
        precision = std::get<spec.precision>(args);
    }
    else
    {
        precision = spec.precision;
    }

    if constexpr (spec.has_set(Spec::Type))
    {
        static_assert(std::is_integral_v<RT>, "Invalid argument");

        result.clear();
        auto num = (arg < 0) ? -arg : arg;
        if constexpr (spec.type == 'x')
        {
            do
            {
                auto rem = num % 16;
                if (rem < 10)
                {
                    result = static_cast<char>(rem + '0') + result;
                }
                else
                {
                    result = static_cast<char>(rem - 10 + 'a') + result;
                }
                num /= 16;
            } while (num != 0);
        }
        else if constexpr (spec.type == 'X')
        {
            do
            {
                auto rem = num % 16;
                if (rem < 10)
                {
                    result = static_cast<char>(rem + '0') + result;
                }
                else
                {
                    result = static_cast<char>(rem - 10 + 'A') + result;
                }
                num /= 16;
            } while (num != 0);
        }
        else if constexpr (spec.type == 'b')
        {
            do
            {
                result = static_cast<char>((num & 1) + '0') + result;
                num >>= 1;
            } while (num != 0);
        }
        else
        {
            static_assert(spec.type == 'o', "Error! Please report!");

            do
            {
                result = static_cast<char>(num % 8 + '0') + result;
                num /= 8;
            } while (num != 0);
        }
        result = "0" + ((spec.type == 'X' ? 'x' : spec.type) + result);

        auto sign = (arg < 0) ? '-' : '+';
        bool has_sign = (spec.sign == '+' || arg < 0) ? true : false;

        if (has_sign)
        {
            result = sign + result;
        }

        if constexpr (spec.has_set(Spec::Padding))
        {
            static_assert(spec.has_set(Spec::Width), "Error! Please report!");

            if (result.size() < width)
            {
                auto pos = has_sign ? 3 : 2;
                result = result.substr(0, pos) + std::string(width - result.size(), '0') + result.substr(pos);
            }
        }
    }
    else if constexpr (spec.has_set(Spec::Padding))
    {
        static_assert(std::is_integral_v<RT> || std::is_floating_point_v<RT>, "Invalid argument");
        static_assert(spec.has_set(Spec::Width), "Error! Please report!");

        auto sign = (arg < 0) ? '-' : '+';
        bool has_sign = (spec.sign == '+' || arg < 0) ? true : false;

        if (has_sign && sign == '+')
        {
            result = '+' + result;
        }

        if (result.size() < width)
        {
            if (has_sign)
            {
                result = result[0] + std::string(width - result.size(), '0') + result.substr(1);
            }
            else
            {
                result = std::string(width - result.size(), '0') + result;
            }
        }
    }
    else if constexpr (spec.sign == '+')
    {
        static_assert(std::is_integral_v<RT> || std::is_floating_point_v<RT>, "Invalid argument");
        if (arg >= 0)
        {
            result = '+' + result;
        }
    }

    if constexpr (spec.has_set(Spec::Align))
    {
        if constexpr (spec.align == '<')
        {
            if (result.size() < width)
            {
                result = result + std::string(width - result.size(), spec.fill);
            }
        }
        else if constexpr (spec.align == '^')
        {
            if (result.size() < width)
            {
                auto rest = width - result.size();
                result = std::string(rest / 2, spec.fill) + result + std::string(rest - rest / 2, spec.fill);
            }
        }
        else
        {
            static_assert(spec.align == '>', "Error! Please report!");

            if (result.size() < width)
            {
                result = std::string(width - result.size(), spec.fill) + result;
            }
        }
    }
    else if constexpr (spec.has_set(Spec::Width))
    {
        if (result.size() < width)
        {
            result += std::string(width - result.size(), ' ');
        }
    }

    return result;
}

template<FixedString pattern>
constexpr std::size_t as_size_t(std::size_t begin, std::size_t end)
{
    std::size_t res = 0;
    for (; begin < end; ++begin)
    {
        if (!is_digit(pattern[begin]))
        {
            throw std::invalid_argument("Invalid integer");
        }
        res = res * 10 + pattern[begin] - '0';
    }
    return res;
}

template<FixedString pattern, std::size_t begin, std::size_t end, Spec::Mode anchor = Spec::Default>
constexpr Spec as_spec()
{
    static_assert(begin <= end, "Error! Please report!");

    if constexpr (begin == end)
    {
        return Spec();
    }
    else if constexpr (begin + 1 < end && is_align(pattern[begin + 1]))
    {
        static_assert(anchor < Spec::Align, "Invalid format spec");
        constexpr auto spec = Spec(as_spec<pattern, begin + 2, end, Spec::Align>(), pattern[begin], pattern[begin + 1]);
        static_assert(spec.has_set(Spec::Width), "Invalid format spec");
        static_assert(!spec.has_set(Spec::Padding), "Invalid format spec");
        return spec;
    }
    else if constexpr (is_align(pattern[begin]))
    {
        static_assert(anchor < Spec::Align, "Invalid format spec");
        constexpr auto spec = Spec(as_spec<pattern, begin + 1, end, Spec::Align>(), ' ', pattern[begin]);
        static_assert(spec.has_set(Spec::Width), "Invalid format spec");
        static_assert(!spec.has_set(Spec::Padding), "Invalid format spec");
        return spec;
    }
    else if constexpr (is_sign(pattern[begin]))
    {
        static_assert(anchor < Spec::Sign, "Invalid format spec");
        return Spec(as_spec<pattern, begin + 1, end, Spec::Sign>(), pattern[begin]);
    }
    else if constexpr (pattern[begin] == '#')
    {
        static_assert(anchor < Spec::Type, "Invalid format spec");
        constexpr auto spec = as_spec<pattern, begin + 1, end, Spec::Type>();
        static_assert(spec.has_set(Spec::Type), "Invalid format spec");
        return spec;
    }
    else if constexpr (pattern[begin] == '0')
    {
        if constexpr (pattern[begin + 1] == '$')
        {
            static_assert(anchor < Spec::Width, "Invalid format spec");
            return Spec(as_spec<pattern, begin + 2, end, Spec::Width>(), Spec::WidthArg, 0);
        }
        else
        {
            static_assert(anchor < Spec::Padding, "Invalid format spec");
            constexpr auto spec = Spec(as_spec<pattern, begin + 1, end, Spec::Padding>(), Spec::Padding);
            static_assert(spec.has_set(Spec::Width), "Invalid format spec");
            return spec;
        }
    }
    else if constexpr (is_digit(pattern[begin]))
    {
        static_assert(anchor < Spec::Width, "Invalid format spec");

        constexpr auto pos = pattern.find_consequent_digit(begin + 1, end);
        if constexpr (pattern[pos] == '$')
        {
            return Spec(as_spec<pattern, pos + 1, end, Spec::Width>(), Spec::WidthArg, as_size_t<pattern>(begin, pos));
        }
        else
        {
            return Spec(as_spec<pattern, pos, end, Spec::Width>(), Spec::Width, as_size_t<pattern>(begin, pos));
        }
    }
    else if constexpr (pattern[begin] == '.')
    {
        static_assert(anchor < Spec::Precision, "Invalid format spec");
        static_assert(begin + 1 < end, "Invalid format spec");

        constexpr auto pos = pattern.find_consequent_digit(begin + 1, end);
        if constexpr (pattern[pos] == '$')
        {
            return Spec(as_spec<pattern, pos + 1, end, Spec::Precision>(), Spec::PreciArg, as_size_t<pattern>(begin + 1, end - 1));
        }
        else
        {
            return Spec(as_spec<pattern, pos, end, Spec::Precision>(), Spec::Precision, as_size_t<pattern>(begin + 1, end - 1));
        }
    }
    else if constexpr (is_type(pattern[begin]))
    {
        static_assert(anchor <= Spec::Precision && begin + 1 == end, "Invalid format spec");
        return Spec(pattern[begin]);
    }
    else
    {
        throw std::invalid_argument("Invalid format spec");
    }
}

template<FixedString pattern, std::size_t i, std::size_t arg_ind, typename ...Args>
inline std::string format_impl(const std::tuple<Args...> &args)
{
    using Tuple = std::tuple<Args...>;

    static_assert(i <= pattern.size, "Error! Please report!");

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
            return as_string(std::get<arg_ind>(args), args) + format_impl<pattern, i + 2, arg_ind + 1>(args);
        }
        else
        {
            constexpr auto end_pos = pattern.find('}', i + 1);
            static_assert(end_pos != pattern.size, "Use {{ or {...} but not only {");

            constexpr auto colon_pos = pattern.find(':', i + 1, end_pos);
            if constexpr (colon_pos < end_pos)
            {
                constexpr auto position = as_size_t<pattern>(i + 1, colon_pos);
                static_assert(position < std::tuple_size_v<Tuple>, "Positional parameters not match");
                return as_string<as_spec<pattern, colon_pos + 1, end_pos>()>(std::get<position>(args), args)
                    + format_impl<pattern, end_pos + 1, arg_ind>(args)
                ;
            }
            else
            {
                constexpr auto position = as_size_t<pattern>(i + 1, end_pos);
                static_assert(position < std::tuple_size_v<Tuple>, "Positional parameters not match");
                return as_string(std::get<position>(args), args) + format_impl<pattern, end_pos + 1, arg_ind>(args);
            }
        }
    }
    else
    {
        constexpr auto pos = pattern.find('{', i + 1);
        return std::string(pattern.data + i, pos - i) + format_impl<pattern, pos, arg_ind>(args);
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
