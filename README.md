# format

A single-header format library for C++20 with the syntax like std::fmt of Rust

## Usage

```cpp
std::cout << fmt::format<"Hello, {}!">("world");                    // => "Hello, world!"
std::cout << fmt::format<"The number is {}">(42);                   // => "The number is 42"
std::cout << fmt::format<"{1} to see you, {0}">("Mu00", "Nice");    // => "Nice to see you, Mu00"
```

### Positional parameters

```cpp
std::cout << fmt::format<"{1} {} {0} {}">(1, 2); // => "2 1 1 2"
std::cout << fmt::format<"{2} {1} {0}">(0, 1, 2) // => "2 1 0"
```

## Formatting Parameters

### Width

```cpp
std::cout << fmt::format<"{:5}!">("x");         // => "x    !"
std::cout << fmt::format<"{:1$}!">("x", 5);     // => "x    !"
std::cout << fmt::format<"{1:0$}!">(5, "x");    // => "x    !"
```

### Fill/Alignment

```cpp
std::cout << fmt::format<"{:<5}!">("x");    // => "x    !"
std::cout << fmt::format<"{:-<5}!">("x");   // => "x----!"
std::cout << fmt::format<"{:^5}!">("x");    // => "  x  !"
std::cout << fmt::format<"{:>5}!">("x");    // => "    x!"
```

### Sign/#/0

```cpp
std::cout << fmt::format<"{:+}!">(5)        // => "+5!"
std::cout << fmt::format<"{:#x}!">(27)      // => "0x1b!"
std::cout << fmt::format<"{:05}!">(5)       // => "00005!"
std::cout << fmt::format<"{:05}!">(-5)      // => "-0005!"
std::cout << fmt::format<"{:#010x}!">(27)   // => "0x0000001b!"
```

### Precision

Not completed now

## Escaping

The literal character `{` may be included in a string by preceding them with the same character. And only occured pairs of `{}` will be treated as placeholders, single `}` will be treated as a literal character. For example:

```cpp
std::cout << fmt::format<"Hello {{}">();    // => "Hello {}"
```

## Syntax

```
format_string := <text> [ maybe-format <text> ] *
maybe-format := '{' '{' | <format>
format := '{' [ integer ] [ ':' format_spec ] '}'

format_spec := [[fill]align][sign]['#']['0'][width]['.' precision][type]
fill := character
align := '<' | '^' | '>'
sign := '+' | '-'
width := count
precision := count
type := 'x' | 'X' | 'b' | 'o'
count := parameter | integer
parameter := integer '$'
```
