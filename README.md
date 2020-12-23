# format

This can be used only in C++20

## Usage

```cpp
std::cout << fmt::format<"Hello, {}!">("world");                    // => "Hello, world!"
std::cout << fmt::format<"The number is {}">(42);                   // => "The number is 42"
std::cout << fmt::format<"{1} to see you, {0}">("Mu00", "Nice");    // => "Nice to see you, Mu00"
```

### Positional parameters

### Formatting Parameters

### Escaping

The literal character `{` may be included in a string by preceding them with the same character. And only occured pairs of `{}` will be treated as placeholders, single `}` will be treated as a literal character. For example:

```cpp
std::cout << fmt::format<"Hello {{}">();    // => "Hello {}"
```
