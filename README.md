# STL Preview
![logo.png](logo.png)

## Description
**Use the latest STL implementation regardless of your C++ version (C++14 ~)**

```c++
// All implementations are written under C++14 standard semantics
#include "preview/ranges.h"

auto map = preview::views::iota('A', 'E') | 
           preview::views::enumerate | 
           preview::ranges::to<std::map>(); // See notes about CTAD below

std::cout << map[0] << ' '
          << map[1] << ' '
          << map[2] << '\n';
// A B C
```

**`preview` is standard-conforming, and is compatible with existing STL**
```c++
// Pre-C++20 iterators are fully compatible
auto floats = std::istringstream{"1.1  2.2\t3.3\v4.4\f55\n66\r7.7  8.8"};
ranges::copy(views::istream<float>(floats), std::ostream_iterator<float>{std::cout, ", "});

// Complex ranges/iterators are also compatible with both pre C++20 and 
// post C++20 std::iterator_traits
auto r = preview::views::iota(0) |
      preview::views::take(10) |
      preview::views::filter([](auto x) { return x % 2 == 0; });

static_assert(std::is_same<
    std::iterator_traits<decltype(r.begin())>::reference,
    int
>::value, "");

```

**Up to C++26 STL are available**
```c++
void foo(preview::span<int>) {}

int main() {
  // span with C++26 standard
  foo({1, 2, 3});
  
  preview::variant<int, float, double> v{1.0f};
  v.visit([](auto x) {
    // member visit(): C++26 standard
  });
  
  // views::concat
  std::vector<char> v = {'h', 'e', 'l'};
  std::string s = "lo, ";
  preview::string_view sv = "world";
  std::list<char> l = {'!'};

  // hello, world!
  for (auto c : preview::views::concat(v, s, sv, l)) {
    std::cout << c;
  }
  
  // expected
  auto process = [](preview::string_view str) -> preview::expected<int, std::string> {
    return parse_number(str)
      .transform([](double v) { return static_cast<int>(v); })
      .transform_error([](parse_error e) {
        return e == parse_error::invalid_input ? "invalid input" : "overflow";
      });
  };
  std::cout << process("42").value_or(-1) << '\n;'; // 42
  std::cout << process("inf").value_or(-1) << '\n'; // -1
}
```

## Compiler Support
[![Build Status](https://github.com/lackhole/stl-preview/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/lackhole/stl-preview/actions/workflows/cmake-multi-platform.yml)

| Compiler    | Minimum version tested             | Maximum version tested             |
|-------------|------------------------------------|------------------------------------|
| MSVC        | 19.16.27051.0 (Visual Studio 2017) | 19.40.33811.0 (Visual Studio 2022) |
| gcc         | 9.5.0                              | 13.1.0                             |
| clang       | 11.1.0                             | 15.0.7                             |
| Apple clang | 14.0.0.14000029                    | 15.0.0.15000040 (Silicon)          |
| Android NDK | r18 (clang 7.0)                    | r26 (clang 17.0.2)                 |
| Emscripten  | 3.1.20 (clang 16.0.0)              | latest(3.1.61) (clang 19.0.0)      |
| MinGW       | 13.1.0                             | 14.2.0                             |
| Intel C++   | ?                                  | icx 2024.2                         |

## Build & Install
### CMake
Copy this repository under your project and use `add_subdirectory`
```cmake
add_subdirectory(path/to/preview)
target_link_libraries(your-project INTERFACE preview)
```

### Non-CMake
Although it is not recommended, you can just copy & paste files under `include/` into your project.  
* Note: Without CMake configuration, heuristic compiler/STL checks will be performed. 

### Installation
With the reason stated in [Non-CMake](#non-cmake), installation is possible but not recommended.

## Implementation Status
Implementation available in C++14 ~ :

| Header                              | Total                                                   |   | C++17                                                  | C++20                                                   | C++23                                                  | C++26                                                  |
|-------------------------------------|---------------------------------------------------------|---|--------------------------------------------------------|---------------------------------------------------------|--------------------------------------------------------|--------------------------------------------------------|
|                                     | ![](https://img.shields.io/badge/312/694-grey)![][p045] |   | ![](https://img.shields.io/badge/44/113-grey)![][p039] | ![](https://img.shields.io/badge/231/429-grey)![][p054] | ![](https://img.shields.io/badge/59/144-grey)![][p041] | ![](https://img.shields.io/badge/18/135-grey)![][p013] |
|                                     |                                                         |   |                                                        |                                                         |                                                        |                                                        |
| [algorithm](#algorithm)             | ![](https://img.shields.io/badge/53/115-grey)![][p046]  |   | ![](https://img.shields.io/badge/2/4-grey)![][p050]    | ![](https://img.shields.io/badge/45/96-grey)![][p047]   | ![](https://img.shields.io/badge/8/18-grey)![][p044]   | ![](https://img.shields.io/badge/7/23-grey)![][p030]   |
| [array](#array)                     | ![](https://img.shields.io/badge/1/1-grey)![][p100]     |   |                                                        | ![](https://img.shields.io/badge/1/1-grey)![][p100]     |                                                        |                                                        |
| [bit](#bit)                         | ![](https://img.shields.io/badge/1/14-grey)![][p007]    |   |                                                        | ![](https://img.shields.io/badge/1/13-grey)![][p008]    | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |                                                        |
| [concepts](#concepts)               | ![](https://img.shields.io/badge/30/30-grey)![][p100]   |   |                                                        | ![](https://img.shields.io/badge/30/30-grey)![][p100]   | ![](https://img.shields.io/badge/1/1-grey)![][p100]    |                                                        |
| [cstddef](#cstddef)                 | ![](https://img.shields.io/badge/2/2-grey)![][p100]     |   | ![](https://img.shields.io/badge/2/2-grey)![][p100]    |                                                         |                                                        |                                                        |
| [expected](#expected)               | ![](https://img.shields.io/badge/4/4-grey)![][p100]     |   |                                                        |                                                         | ![](https://img.shields.io/badge/4/4-grey)![][p100]    |                                                        |
| [functional](#functional)           | ![](https://img.shields.io/badge/10/16-grey)![][p063]   |   | ![](https://img.shields.io/badge/3/5-grey)![][p060]    | ![](https://img.shields.io/badge/6/7-grey)![][p086]     | ![](https://img.shields.io/badge/2/3-grey)![][p067]    | ![](https://img.shields.io/badge/3/5-grey)![][p060]    |
| [iterator](#iterator)               | ![](https://img.shields.io/badge/57/59-grey)![][p097]   |   | ![](https://img.shields.io/badge/1/3-grey)![][p033]    | ![](https://img.shields.io/badge/51/52-grey)![][p098]   | ![](https://img.shields.io/badge/5/5-grey)![][p100]    | ![](https://img.shields.io/badge/2/2-grey)![][p100]    |
| [memory](#memory)                   | ![](https://img.shields.io/badge/9/44-grey)![][p020]    |   | ![](https://img.shields.io/badge/4/11-grey)![][p036]   | ![](https://img.shields.io/badge/9/36-grey)![][p025]    | ![](https://img.shields.io/badge/0/3-grey)![][p000]    | ![](https://img.shields.io/badge/0/2-grey)![][p000]    |
| [numbers](#numbers)                 | ![](https://img.shields.io/badge/13/13-grey)![][p100]   |   |                                                        | ![](https://img.shields.io/badge/13/13-grey)![][p100]   |                                                        |                                                        |
| [numeric](#numeric)                 | ![](https://img.shields.io/badge/1/15-grey)![][p007]    |   | ![](https://img.shields.io/badge/0/9-grey)![][p000]    | ![](https://img.shields.io/badge/0/4-grey)![][p000]     | ![](https://img.shields.io/badge/1/1-grey)![][p100]    | ![](https://img.shields.io/badge/0/5-grey)![][p000]    |
| [optional](#optional)               | ![](https://img.shields.io/badge/7/7-grey)![][p100]     |   | ![](https://img.shields.io/badge/7/7-grey)![][p100]    |                                                         | ![](https://img.shields.io/badge/1/1-grey)![][p100]    | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |
| [random](#random)                   | ![](https://img.shields.io/badge/1/1-grey)![][p100]     |   |                                                        | ![](https://img.shields.io/badge/1/1-grey)![][p100]     |                                                        |                                                        |
| [ranges](#ranges)                   | ![](https://img.shields.io/badge/76/82-grey)![][p093]   |   |                                                        | ![](https://img.shields.io/badge/60/60-grey)![][p100]   | ![](https://img.shields.io/badge/29/36-grey)![][p081]  | ![](https://img.shields.io/badge/1/1-grey)![][p100]    |
| [span](#span)                       | ![](https://img.shields.io/badge/4/4-grey)![][p100]     |   |                                                        | ![](https://img.shields.io/badge/4/4-grey)![][p100]     | ![](https://img.shields.io/badge/1/1-grey)![][p100]    | ![](https://img.shields.io/badge/1/1-grey)![][p100]    |
| [string_view](#string_view)         | ![](https://img.shields.io/badge/7/7-grey)![][p100]     |   | ![](https://img.shields.io/badge/4/4-grey)![][p100]    | ![](https://img.shields.io/badge/1/1-grey)![][p100]     | ![](https://img.shields.io/badge/1/1-grey)![][p100]    | ![](https://img.shields.io/badge/3/3-grey)![][p100] *  |
| [tuple](#tuple)                     | ![](https://img.shields.io/badge/2/2-grey)![][p100]     |   | ![](https://img.shields.io/badge/2/2-grey)![][p100]    |                                                         | ![](https://img.shields.io/badge/2/2-grey)![][p100]    |                                                        |
| [type_traits](#type_traits)         | ![](https://img.shields.io/badge/17/26-grey)![][p065]   |   | ![](https://img.shields.io/badge/8/10-grey)![][p080]   | ![](https://img.shields.io/badge/7/13-grey)![][p054]    | ![](https://img.shields.io/badge/2/3-grey)![][p067]    | ![](https://img.shields.io/badge/0/2-grey)![][p000]    |
| [utility](#utility)                 | ![](https://img.shields.io/badge/7/8-grey)![][p088]     |   | ![](https://img.shields.io/badge/2/2-grey)![][p100]    | ![](https://img.shields.io/badge/2/2-grey)![][p100]     | ![](https://img.shields.io/badge/2/3-grey)![][p067]    | ![](https://img.shields.io/badge/1/1-grey)![][p100]    |
| [variant](#variant)                 | ![](https://img.shields.io/badge/9/9-grey)![][p100]     |   | ![](https://img.shields.io/badge/9/9-grey)![][p100]    |                                                         |                                                        | ![](https://img.shields.io/badge/1/1-grey)![][p100]    |
|                                     |                                                         |   |                                                        |                                                         |                                                        |                                                        |
| [any](#any)                         | ![](https://img.shields.io/badge/5/5-grey)![][p100]     |   | ![](https://img.shields.io/badge/5/5-grey)![][p100]    |                                                         |                                                        |                                                        |
| [atomic](#atomic)                   | ![](https://img.shields.io/badge/0/17-grey)![][p000]    |   |                                                        | ![](https://img.shields.io/badge/0/13-grey)![][p000]    |                                                        | ![](https://img.shields.io/badge/0/4-grey)![][p000]    |
| [barrier](#barrier)                 | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/5-grey)![][p000]     |                                                        |                                                        |
| [charconv](#charconv)               | ![](https://img.shields.io/badge/0/5-grey)![][p000]     |   | ![](https://img.shields.io/badge/0/14-grey)![][p000]   |                                                         | ![](https://img.shields.io/badge/0/2-grey)![][p000]    |                                                        |
| [chrono](#chrono)                   | ![](https://img.shields.io/badge/0/47-grey)![][p000]    |   | ![](https://img.shields.io/badge/0/2-grey)![][p000]    | ![](https://img.shields.io/badge/0/47-grey)![][p000]    |                                                        | ![](https://img.shields.io/badge/0/2-grey)![][p000]    |
| [cmath](#cmath)                     | ![](https://img.shields.io/badge/0/22-grey)![][p000]    |   | ![](https://img.shields.io/badge/0/22-grey)![][p000]   |                                                         | ![](https://img.shields.io/badge/0/22-grey)![][p000]   |                                                        |
| [compare](#compare)                 |                                                         |   |                                                        |                                                         |                                                        |                                                        |
| [coroutine](#coroutine)             | N/A                                                     |   |                                                        |                                                         |                                                        |                                                        |
| [debugging](#debugging)             | ![](https://img.shields.io/badge/0/3-grey)![][p000]     |   |                                                        |                                                         |                                                        | ![](https://img.shields.io/badge/0/3-grey)![][p000]    |
| [exception](#exception)             | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |                                                         |                                                        |                                                        |
| [execution](#execution)             |                                                         |   |                                                        |                                                         |                                                        |                                                        |
| [flat_map](#flat_map)               | ![](https://img.shields.io/badge/0/4-grey)![][p000]     |   |                                                        |                                                         | ![](https://img.shields.io/badge/0/4-grey)![][p000]    |                                                        |
| [flat_set](#flat_set)               | ![](https://img.shields.io/badge/0/2-grey)![][p000]     |   |                                                        |                                                         | ![](https://img.shields.io/badge/0/2-grey)![][p000]    |                                                        |
| [format](#format)                   | ![](https://img.shields.io/badge/0/24-grey)![][p000]    |   |                                                        | ![](https://img.shields.io/badge/0/16-grey)![][p000]    | ![](https://img.shields.io/badge/0/15-grey)![][p000]   | ![](https://img.shields.io/badge/0/10-grey)![][p000]   |
| [filesystem](#filesystem)           | N/A                                                     |   |                                                        |                                                         |                                                        |                                                        |
| [hazard_pointer](#hazard_pointer)   | ![](https://img.shields.io/badge/0/3-grey)![][p000]     |   |                                                        |                                                         |                                                        | ![](https://img.shields.io/badge/0/3-grey)![][p000]    |
| [inplace_vector](#inplace_vector)   | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   |                                                        |                                                         |                                                        | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |
| [latch](#latch)                     | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |                                                        |                                                        |
| [linalg](#linalg)                   | ![](https://img.shields.io/badge/0/51-grey)![][p000]    |   |                                                        |                                                         |                                                        | ![](https://img.shields.io/badge/0/51-grey)![][p000]   |
| [mdspan](#mdspan)                   | ![](https://img.shields.io/badge/0/11-grey)![][p000]    |   |                                                        |                                                         | ![](https://img.shields.io/badge/0/6-grey)![][p000]    | ![](https://img.shields.io/badge/0/5-grey)![][p000]    |
| [memory_resource](#memory_resource) |                                                         |   |                                                        |                                                         |                                                        |                                                        |
| [mutex](#mutex)                     | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |                                                         |                                                        |                                                        |
| [new](#new)                         | ![](https://img.shields.io/badge/0/5-grey)![][p000]     |   | ![](https://img.shields.io/badge/0/4-grey)![][p000]    | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |                                                        |                                                        |
| [print](#print)                     | ![](https://img.shields.io/badge/0/6-grey)![][p000]     |   |                                                        |                                                         | ![](https://img.shields.io/badge/0/6-grey)![][p000]    |                                                        |
| [rcu](#rcu)                         | ![](https://img.shields.io/badge/0/6-grey)![][p000]     |   |                                                        |                                                         |                                                        | ![](https://img.shields.io/badge/0/6-grey)![][p000]    |
| [semaphore](#semaphore)             | ![](https://img.shields.io/badge/0/2-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/2-grey)![][p000]     |                                                        |                                                        |
| [shared_mutex](#shared_mutex)       | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |                                                         |                                                        |                                                        |
| [stop_token](#stop_token)           | ![](https://img.shields.io/badge/0/5-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/5-grey)![][p000]     |                                                        |                                                        |
| [string](#string)                   | ![](https://img.shields.io/badge/0/2-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/2-grey)![][p000]     |                                                        | ![](https://img.shields.io/badge/0/2-grey)![][p000]    |
| [source_location](#source_location) | N/A                                                     |   |                                                        |                                                         |                                                        |                                                        |
| [syncstream](#syncstream)           | ![](https://img.shields.io/badge/0/3-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/3-grey)![][p000]     |                                                        |                                                        |
| [spanstream](#spanstream)           | ![](https://img.shields.io/badge/0/4-grey)![][p000]     |   |                                                        |                                                         | ![](https://img.shields.io/badge/0/4-grey)![][p000]    |                                                        |
| [stacktrace](#stacktrace)           | N/A                                                     |   |                                                        |                                                         |                                                        |                                                        |
| [text_encoding](#text_encoding)     | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   |                                                        |                                                         |                                                        | ![](https://img.shields.io/badge/0/1-grey)![][p000]    |
| [thread](#thread)                   | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |   |                                                        | ![](https://img.shields.io/badge/0/1-grey)![][p000]     |                                                        |                                                        |
| [version](#version)                 |                                                         |   |                                                        |                                                         |                                                        |                                                        |

-----

## Detailed status

### Features
Description
* ![](https://img.shields.io/badge/C++XX-C++YY-CCEEFF): C++YY standard implemented in C++XX
* ![](https://img.shields.io/badge/C++YY-red): C++YY standard, not implemented yet
* If the implementation is impossible(i.e., needs compiler support / hardware info) it is marked as **N/A**
* Some features are working in progress
* **Introduced**: First introduced version
* **Revision**: Behavior changed/updated version

### Headers

#### `<algorithm>`

  |                                     | Introduced | Revision   |
  |-------------------------------------|------------|------------|
  | `search`                            | ![][c98]   | ![][c17no] |
  | `clamp`                             | ![][c17ok] | ![][c20ok] |
  | `for_each_n`                        | ![][c17ok] | ![][c20ok] |
  | `sample`                            | ![][c17no] | ![][c23no] |
  | `shift_left`                        | ![][c20ok] |            |
  | `shift_right`                       | ![][c20ok] |            |
  | `ranges::in_fun_result`             | ![][c20ok] |            |
  | `ranges::in_in_result`              | ![][c20ok] |            |
  | `ranges::in_out_result`             | ![][c20ok] |            |
  | `ranges::in_in_out_result`          | ![][c20ok] |            |
  | `ranges::in_out_out_result`         | ![][c20ok] |            |
  | `ranges::min_max_result`            | ![][c20ok] |            |
  | `ranges::in_found_result`           | ![][c20ok] |            |
  | `ranges::in_value_result`           | ![][c23ok] |            |
  | `ranges::out_value_result`          | ![][c23ok] |            |
  | `ranges::all_of`                    | ![][c20ok] |            |
  | `ranges::any_of`                    | ![][c20ok] |            |
  | `ranges::none_of`                   | ![][c20ok] |            |
  | `ranges::for_each`                  | ![][c20ok] |            |
  | `ranges::for_each_n`                | ![][c20ok] |            |
  | `ranges::count`                     | ![][c20ok] | ![][c26ok] |
  | `ranges::count_if`                  | ![][c20ok] |            |
  | `ranges::mismatch`                  | ![][c20ok] |            |
  | `ranges::find`                      | ![][c20ok] | ![][c26ok] |
  | `ranges::find_if`                   | ![][c20ok] |            |
  | `ranges::find_if_not`               | ![][c20ok] |            |
  | `ranges::find_last`                 | ![][c23ok] | ![][c26ok] |
  | `ranges::find_last_if`              | ![][c23ok] |            |
  | `ranges::find_last_if_not`          | ![][c23ok] |            |
  | `ranges::find_end`                  | ![][c20ok] |            |
  | `ranges::find_first_of`             | ![][c20ok] |            |
  | `ranges::adjacent_find`             | ![][c20ok] |            |
  | `ranges::search`                    | ![][c20ok] |            |
  | `ranges::search_n`                  | ![][c20ok] | ![][c26ok] |
  | `ranges::contains`                  | ![][c23ok] | ![][c26ok] |
  | `ranges::contains_subrange`         | ![][c23ok] |            |
  | `ranges::starts_with`               | ![][c23no] |            |
  | `ranges::ends_with`                 | ![][c23no] |            |
  | `ranges::fold_left`                 | ![][c23ok] | ![][c26no] |
  | `ranges::fold_left_first`           | ![][c23no] |            |
  | `ranges::fold_right`                | ![][c23no] | ![][c26no] |
  | `ranges::fold_right_last`           | ![][c23no] |            |
  | `ranges::fold_left_with_iter`       | ![][c23no] | ![][c26no] |
  | `ranges::fold_left_first_with_iter` | ![][c23no] |            |
  | `ranges::copy`                      | ![][c20ok] |            |
  | `ranges::copy_if`                   | ![][c20ok] |            |
  | `ranges::copy_n`                    | ![][c20ok] |            |
  | `ranges::copy_backward`             | ![][c20ok] |            |
  | `ranges::move`                      | ![][c20no] |            |
  | `ranges::move_backward`             | ![][c20no] |            |
  | `ranges::fill`                      | ![][c20ok] | ![][c26ok] |
  | `ranges::fill_n`                    | ![][c20ok] | ![][c26ok] |
  | `ranges::transform`                 | ![][c20no] |            |
  | `ranges::generate`                  | ![][c20no] |            |
  | `ranges::generate_n`                | ![][c20no] |            |
  | `ranges::remove`                    | ![][c20no] | ![][c26no] |
  | `ranges::remove_if`                 | ![][c20no] |            |
  | `ranges::remove_copy`               | ![][c20no] | ![][c26no] |
  | `ranges::remove_copy_if`            | ![][c20no] |            |
  | `ranges::replace`                   | ![][c20no] | ![][c26no] |
  | `ranges::replace_if`                | ![][c20no] | ![][c26no] |
  | `ranges::replace_copy`              | ![][c20no] | ![][c26no] |
  | `ranges::replace_copy_if`           | ![][c20no] | ![][c26no] |
  | `ranges::swap_ranges`               | ![][c20ok] |            |
  | `ranges::reverse`                   | ![][c20no] |            |
  | `ranges::reverse_copy`              | ![][c20no] |            |
  | `ranges::rotate`                    | ![][c20no] |            |
  | `ranges::rotate_copy`               | ![][c20no] |            |
  | `ranges::shift_left`                | ![][c23no] |            |
  | `ranges::shift_right`               | ![][c23no] |            |
  | `ranges::sample`                    | ![][c20no] |            |
  | `ranges::shuffle`                   | ![][c20no] |            |
  | `ranges::unique`                    | ![][c20no] |            |
  | `ranges::unique_copy`               | ![][c20no] |            |
  | `ranges::is_partitioned`            | ![][c20no] |            |
  | `ranges::partition`                 | ![][c20no] |            |
  | `ranges::partition_copy`            | ![][c20no] |            |
  | `ranges::stable_partition`          | ![][c20no] | ![][c26no] |
  | `ranges::partition_point`           | ![][c20no] |            |
  | `ranges::is_sorted`                 | ![][c20no] |            |
  | `ranges::is_sorted_until`           | ![][c20no] |            |
  | `ranges::sort`                      | ![][c20ok] |            |
  | `ranges::partial_sort`              | ![][c20no] |            |
  | `ranges::partial_sort_copy`         | ![][c20no] |            |
  | `ranges::stable_sort`               | ![][c20no] | ![][c26no] |
  | `ranges::nth_element`               | ![][c20no] |            |
  | `ranges::lower_bound`               | ![][c20no] | ![][c26no] |
  | `ranges::upper_bound`               | ![][c20no] | ![][c26no] |
  | `ranges::binary_search`             | ![][c20no] | ![][c26no] |
  | `ranges::equal_range`               | ![][c20no] | ![][c26no] |
  | `ranges::merge`                     | ![][c20no] |            |
  | `ranges::inplace_merge`             | ![][c20no] | ![][c26no] |
  | `ranges::includes`                  | ![][c20no] |            |
  | `ranges::set_difference`            | ![][c20no] |            |
  | `ranges::set_intersection`          | ![][c20ok] |            |
  | `ranges::set_symmetric_difference`  | ![][c20no] |            |
  | `ranges::set_union`                 | ![][c20no] |            |
  | `ranges::is_heap`                   | ![][c20no] |            |
  | `ranges::is_heap_until`             | ![][c20no] |            |
  | `ranges::make_heap`                 | ![][c20ok] |            |
  | `ranges::push_heap`                 | ![][c20no] |            |
  | `ranges::pop_heap`                  | ![][c20ok] |            |
  | `ranges::sort_heap`                 | ![][c20ok] |            |
  | `ranges::max`                       | ![][c20ok] |            |
  | `ranges::max_element`               | ![][c20ok] |            |
  | `ranges::min`                       | ![][c20ok] |            |
  | `ranges::min_element`               | ![][c20ok] |            |
  | `ranges::minmax`                    | ![][c20no] |            |
  | `ranges::minmax_element`            | ![][c20no] |            |
  | `ranges::clamp`                     | ![][c20no] |            |
  | `ranges::equal`                     | ![][c20ok] |            |
  | `ranges::lexicographical_compare`   | ![][c20ok] |            |
  | `ranges::is_permutation`            | ![][c20no] |            |
  | `ranges::next_permutation`          | ![][c20no] |            |
  | `ranges::prev_permutation`          | ![][c20no] |            |

#### `<any>`

  |                | Introduced  | Revision |
  |----------------|-------------|----------|
  | `any`          | ![][c17ok]* |          |
  | `bad_any_cast` | ![][c17ok]  |          |
  | `swap(any)`    | ![][c17ok]  |          |
  | `make_any`     | ![][c17ok]  |          |
  | `any_cast`     | ![][c17ok]  |          |

* Notes
  * `preview::any`
    * `preview::any` is an alias of `std::any` if using C++17 or later to prevent
      implicit construction of `std::any` from `preview::any` and vice-versa. 
      See [any.h](include/preview/__any/any.h) for more detail.

#### `<array>`

  |            | Introduced | Revision |
  |------------|------------|----------|
  | `to_array` | ![][c20ok] |          | 

#### `<atomic>`

  |                             | Introduced | Revision |
  |-----------------------------|------------|----------|
  | `atomic_ref`                | ![][c20no] |          | 
  | `atomic_signed_lock_free`   | ![][c20no] |          | 
  | `atomic_unsigned_lock_free` | ![][c20no] |          | 
  | `atomic_wait`               | ![][c20no] |          | 
  | `atomic_wait_explicit`      | ![][c20no] |          | 
  | `atomic_notify_one`         | ![][c20no] |          | 
  | `atomic_notify_all`         | ![][c20no] |          | 
  | `atomic_flag_test`          | ![][c20no] |          | 
  | `atomic_flag_test_explicit` | ![][c20no] |          | 
  | `atomic_flag_wait`          | ![][c20no] |          | 
  | `atomic_flag_wait_explicit` | ![][c20no] |          | 
  | `atomic_flag_notify_one`    | ![][c20no] |          | 
  | `atomic_flag_notifly_all`   | ![][c20no] |          | 
  | `atomic_fetch_max`          | ![][c26no] |          | 
  | `atomic_fetch_max_explicit` | ![][c26no] |          | 
  | `atomic_fetch_min`          | ![][c26no] |          | 
  | `atomic_fetch_min_explicit` | ![][c26no] |          | 

#### `<barrier>`

  |           | Introduced | Revision |
  |-----------|------------|----------|
  | `barrier` | ![][c20no] |          |

#### `<bit>`

  |                  | Introduced | Revision |
  |------------------|------------|----------|
  | `endian`         | ![][c20no] |          |
  | `bit_cast`       | ![][c20ok] |          |
  | `byteswap`       | ![][c23no] |          |
  | `has_single_bit` | ![][c20no] |          |
  | `bit_ceil`       | ![][c20no] |          |
  | `bit_floor`      | ![][c20no] |          |
  | `bit_width`      | ![][c20no] |          |
  | `rotl`           | ![][c20no] |          |
  | `rotr`           | ![][c20no] |          |
  | `countl_zero`    | ![][c20no] |          |
  | `countl_one`     | ![][c20no] |          |
  | `countr_zero`    | ![][c20no] |          |
  | `countr_one`     | ![][c20no] |          |
  | `popcount`       | ![][c20no] |          |

#### `<charconv>`

  |                     | Introduced | Revision   |
  |---------------------|------------|------------|
  | `chars_format`      | ![][c17no] |            |
  | `from_chars_result` | ![][c17no] |            |
  | `to_chars_result`   | ![][c17no] |            |
  | `from_chars`        | ![][c17no] | ![][c23no] |
  | `to_chars`          | ![][c17no] | ![][c23no] |

#### `<chrono>`

  |                           | Introduced | Revision                         |
  |---------------------------|------------|----------------------------------|
  | `std::chrono::duration`   | ![][c11]   | ![][c17no] ![][c20no] ![][c26no] |
  | `std::chrono::time_point` | ![][c11]   | ![][c17no] ![][c20no] ![][c26no] |
  | `clock_time_conversion`   | ![][c20no] |                                  |
  | `is_clock`                | ![][c20no] |                                  |
  | `utc_clock`               | ![][c20no] |                                  |
  | `tai_clock`               | ![][c20no] |                                  |
  | `gps_clock`               | ![][c20no] |                                  |
  | `file_clock`              | ![][c20no] |                                  |
  | `local_t`                 | ![][c20no] |                                  |
  | `last_spec`               | ![][c20no] |                                  |
  | `day`                     | ![][c20no] |                                  |
  | `month`                   | ![][c20no] |                                  |
  | `year`                    | ![][c20no] |                                  |
  | `weekday`                 | ![][c20no] |                                  |
  | `weekday_indexed`         | ![][c20no] |                                  |
  | `weekday_last`            | ![][c20no] |                                  |
  | `month_day`               | ![][c20no] |                                  |
  | `month_day_last`          | ![][c20no] |                                  |
  | `month_weekday`           | ![][c20no] |                                  |
  | `month_weekday_last`      | ![][c20no] |                                  |
  | `year_month`              | ![][c20no] |                                  |
  | `year_month_day`          | ![][c20no] |                                  |
  | `year_month_day_last`     | ![][c20no] |                                  |
  | `year_month_weekday`      | ![][c20no] |                                  |
  | `year_month_weekday_last` | ![][c20no] |                                  |
  | `hh_mm_ss`                | ![][c20no] |                                  |
  | `tzdb`                    | ![][c20no] |                                  |
  | `tzdb_list`               | ![][c20no] |                                  |
  | `tzdb_zone`               | ![][c20no] |                                  |
  | `get_tzdb`                | ![][c20no] |                                  |
  | `get_tzdb_list`           | ![][c20no] |                                  |
  | `reload_tzdb`             | ![][c20no] |                                  |
  | `remote_version`          | ![][c20no] |                                  |
  | `locate_zone`             | ![][c20no] |                                  |
  | `sys_info`                | ![][c20no] |                                  |
  | `local_info`              | ![][c20no] |                                  |
  | `choose`                  | ![][c20no] |                                  |
  | `zoned_traits`            | ![][c20no] |                                  |
  | `zoned_time`              | ![][c20no] |                                  |
  | `time_zone_link`          | ![][c20no] |                                  |
  | `nonexistent_local_time`  | ![][c20no] |                                  |
  | `ambiguous_local_time`    | ![][c20no] |                                  |
  | `leap_second`             | ![][c20no] |                                  |
  | `leap_second_info`        | ![][c20no] |                                  |
  | `get_leap_second_info`    | ![][c20no] |                                  |
  | `is_am`<br/>`is_pm`       | ![][c20no] |                                  |
  | `make_12`<br/>`make_24`   | ![][c20no] |                                  |

#### `<cmath>`

  |                  | Introduced | Revision   |
  |------------------|------------|------------|
  | `assoc_laguerre` | ![][c17no] | ![][c23no] |
  | `assoc_legendre` | ![][c17no] | ![][c23no] |
  | `beta`           | ![][c17no] | ![][c23no] |
  | `comp_ellint_1`  | ![][c17no] | ![][c23no] |
  | `comp_ellint_2`  | ![][c17no] | ![][c23no] |
  | `comp_ellint_3`  | ![][c17no] | ![][c23no] |
  | `cyl_bessel_i`   | ![][c17no] | ![][c23no] |
  | `cyl_bessel_j`   | ![][c17no] | ![][c23no] |
  | `cyl_bessel_k`   | ![][c17no] | ![][c23no] |
  | `cyl_neumann`    | ![][c17no] | ![][c23no] |
  | `ellint_1`       | ![][c17no] | ![][c23no] |
  | `ellint_2`       | ![][c17no] | ![][c23no] |
  | `ellint_3`       | ![][c17no] | ![][c23no] |
  | `expint`         | ![][c17no] | ![][c23no] |
  | `hermite`        | ![][c17no] | ![][c23no] |
  | `legendre`       | ![][c17no] | ![][c23no] |
  | `laguerre`       | ![][c17no] | ![][c23no] |
  | `riemann_zeta`   | ![][c17no] | ![][c23no] |
  | `sph_bessel`     | ![][c17no] | ![][c23no] |
  | `sph_legendre`   | ![][c17no] | ![][c23no] |
  | `sph_neumann`    | ![][c17no] | ![][c23no] |
  | `hypot(x, y, z)` | ![][c17no] | ![][c26no] |

#### `<compare>`
  N/A

#### `<concepts>`

  |                            | Introduced | Revision   |
  |----------------------------|------------|------------|
  | `same_as`                  | ![][c20ok] |            |
  | `derived_from`             | ![][c20ok] |            |
  | `convertible_to`           | ![][c20ok] |            |
  | `common_reference_with`    | ![][c20ok] |            |
  | `common_with`              | ![][c20ok] |            |
  | `integral`                 | ![][c20ok] |            |
  | `signed_integral`          | ![][c20ok] |            |
  | `unsigned_integral`        | ![][c20ok] |            |
  | `floating_point`           | ![][c20ok] |            |
  | `assignable_from`          | ![][c20ok] |            |
  | `swappable`                | ![][c20ok] |            |
  | `destructible`             | ![][c20ok] |            |
  | `constructible_from`       | ![][c20ok] |            |
  | `default_initializable`    | ![][c20ok] |            |
  | `move_constructible`       | ![][c20ok] |            |
  | `copy_constructible`       | ![][c20ok] |            |
  | `equality_comparable`      | ![][c20ok] |            |
  | `equality_comparable_with` | ![][c20ok] | ![][c23ok] |
  | `totally_ordered`          | ![][c20ok] |            |
  | `movable`                  | ![][c20ok] |            |
  | `copyable`                 | ![][c20ok] |            |
  | `semiregular`              | ![][c20ok] |            |
  | `regular`                  | ![][c20ok] |            |
  | `invocable`                | ![][c20ok] |            |
  | `regular_invocable`        | ![][c20ok] |            |
  | `predicate`                | ![][c20ok] |            |
  | `relation`                 | ![][c20ok] |            |
  | `equivalence_relation`     | ![][c20ok] |            |
  | `strict_weak_order`        | ![][c20ok] |            |
  | `ranges::swap`             | ![][c20ok] |            |

#### `<coroutine>`
  N/A

#### `<cstddef>`

  |              | Introduced | Revision |
  |--------------|------------|----------|
  | `byte`       | ![][c17ok] |          |
  | `to_integer` | ![][c17ok] |          |

#### `<debugging>`

  |                           | Introduced | Revision |
  |---------------------------|------------|----------|
  | `breakpoint`              | ![][c26no] |          |
  | `breakpoint_if_debugging` | ![][c26no] |          |
  | `is_debugger_present`     | ![][c26no] |          |

#### `<exception>`

  |                       | Introduced       | Revision |
  |-----------------------|------------------|----------|
  | `uncaught_exceptions` | ![][c17no] (N/A) |          |

#### `<expected>`

  |                             | Introduced | Revision |
  |-----------------------------|------------|----------|
  | `expected`                  | ![][c23ok] |          |
  | `unexpected`                | ![][c23ok] |          |
  | `bad_expected_access`       | ![][c23ok] |          |
  | `unexpect`<br/>`unexpect_t` | ![][c23ok] |          |

#### `<execution>`
  N/A

#### `<flat_map>`

  |                                               | Introduced | Revision |
  |-----------------------------------------------|------------|----------|
  | `flat_map`                                    | ![][c23no] |          |
  | `flat_multimap`                               | ![][c23no] |          |
  | `sorted_unique`<br/>`sorted_unique_t`         | ![][c23no] |          |
  | `sorted_equivalent`<br/>`sorted_equivalent_t` | ![][c23no] |          |

#### `<flat_set>`

  |                 | Introduced | Revision |
  |-----------------|------------|----------|
  | `flat_set`      | ![][c23no] |          |
  | `flat_multiset` | ![][c23no] |          |
 
#### `<format>`

  |                                            | Introduced | Revision              |
  |--------------------------------------------|------------|-----------------------|
  | `formatter`                                | ![][c20no] | ![][c23no]            |
  | `basic_format_parse_context`               | ![][c20no] | ![][c26no]            |
  | `basic_format_context`                     | ![][c20no] |                       |
  | `basic_format_arg`                         | ![][c20no] | ![][c26no]            |
  | `basic_format_args`                        | ![][c20no] |                       |
  | `basic_format_string`                      | ![][c20no] | ![][c23no] ![][c26no] |
  | `format_error`                             | ![][c20no] |                       |
  | `format`                                   | ![][c20no] | ![][c23no] ![][c26no] |
  | `format_to`                                | ![][c20no] | ![][c23no] ![][c26no] |
  | `format_to_n`                              | ![][c20no] | ![][c23no] ![][c26no] |
  | `formatted_size`                           | ![][c20no] | ![][c23no] ![][c26no] |
  | `vformat`                                  | ![][c20no] | ![][c23no] ![][c26no] |
  | `vformat_to`                               | ![][c20no] | ![][c23no] ![][c26no] |
  | ~~`visit_format_arg`~~                     | ![][c20no] | deprecated in C++26   |
  | `make_format_args`                         | ![][c20no] |                       |
  | `make_wformat_args`                        | ![][c20no] |                       |
  | `formattable`                              | ![][c23no] |                       |
  | `range_format`                             | ![][c23no] |                       |
  | `range_formatter`                          | ![][c23no] |                       |
  | `formatter<pair-or-tuple>`                 | ![][c23no] |                       |
  | `formatter<range>`                         | ![][c23no] |                       |
  | `format_kind`                              | ![][c23no] |                       |
  | `enable_nonlocking_formatter_optimization` | ![][c23no] |                       |
  | `runtime_format`                           | ![][c26no] |                       |

#### `<filesystem>`
  N/A

#### `<functional>`

  |                                 | Introduced | Revision     |
  |---------------------------------|------------|--------------|
  | `default_searcher`              | ![][c17ok] | ![][c20ok]   |
  | `boyer_moore_searcher`          | ![][c17no] |              |
  | `boyer_moore_horspool_searcher` | ![][c17no] |              |
  | `invoke`                        | ![][c17ok] |              |
  | `invoke_r`                      | ![][c23ok] |              |
  | `not_fn`                        | ![][c17ok] | ![][c26ok]   |
  | `bind_front`                    | ![][c20ok] | ![][c26ok17] |
  | `bind_back`                     | ![][c23ok] | ![][c26ok17] |
  | `identity`                      | ![][c20ok] |              |
  | `unwrap_reference`              | ![][c20ok] |              |
  | `unwrap_ref_decay`              | ![][c20ok] |              |
  | `ranges::equal_to`...           | ![][c20ok] |              |
  | `ranges::three_way`             | ![][c20no] |              |
  | `move_only_function`            | ![][c23no] |              |
  | `copyable_function`             | ![][c26no] |              |
  | `function_ref`                  | ![][c26no] |              |

#### `<hazard_pointer>`

  |                           | Introduced | Revision |
  |---------------------------|------------|----------|
  | `hazard_pointer_obj_base` | ![][c26no] |          |
  | `hazard_pointer`          | ![][c26no] |          |
  | `make_hazard_pointer`     | ![][c26no] |          |

#### `<inplace_vector>`

  |                       | Introduced | Revision |
  |-----------------------|------------|----------|
  | `inplace_vector`      | ![][c26no] |          |

#### `<iterator>`

  |                                                     | Introduced   | Revision    |
  |-----------------------------------------------------|--------------|-------------|
  | `contiguous_iterator_tag`                           | ![][c20ok] * |             |
  | `indirectly_readable`                               | ![][c20ok]   |             |
  | `indirectly_writable`                               | ![][c20ok]   |             |
  | `weakly_incrementable`                              | ![][c20ok]   |             |
  | `incrementable`                                     | ![][c20ok]   |             |
  | `input_or_output_iterator`                          | ![][c20ok]   |             |
  | `sentinel_for`                                      | ![][c20ok]   |             |
  | `sized_sentinel_for`                                | ![][c20ok]   |             |
  | `input_iterator`                                    | ![][c20ok]   |             |
  | `output_iterator`                                   | ![][c20ok]   |             |
  | `forward_iterator`                                  | ![][c20ok]   |             |
  | `bidirectional_iterator`                            | ![][c20ok]   |             |
  | `random_access_iterator`                            | ![][c20ok]   |             |
  | `contiguous_iterator`                               | ![][c20ok] * |             |
  | `indirectly_unary_invocable`                        | ![][c20ok]   |             |
  | `indirectly_regular_unary_invocable`                | ![][c20ok]   |             |
  | `indirect_unary_predicate`                          | ![][c20ok]   |             |
  | `indirect_binary_predicate`                         | ![][c20ok]   |             |
  | `indirect_equivalence_relation`                     | ![][c20ok]   |             |
  | `indirect_strict_weak_order`                        | ![][c20ok]   |             |
  | `indirectly_movable`                                | ![][c20ok]   |             |
  | `indirectly_movable_storable`                       | ![][c20ok]   |             |
  | `indirectly_copyable`                               | ![][c20ok]   |             |
  | `indirectly_copyable_storable`                      | ![][c20ok]   |             |
  | `indirectly_swappable`                              | ![][c20ok]   |             |
  | `indirectly_comparable`                             | ![][c20ok]   |             |
  | `permutable`                                        | ![][c20ok]   |             |
  | `mergeable`                                         | ![][c20ok]   |             |
  | `sortable`                                          | ![][c20ok]   |             |
  | `indirect_result_t`                                 | ![][c20ok]   |             |
  | `projected`                                         | ![][c20ok]   | ![][c26ok]* |
  | `projected_value_t`                                 | ![][c26ok]   |             |
  | `incrementable_traits`                              | ![][c20ok]   |             |
  | `indirectly_readable_traits`                        | ![][c20ok]   |             |
  | `iter_value_t`<br/>`iter_reference_t`               | ![][c20ok]   |             |
  | `iter_const_reference_t`                            | ![][c23ok]   |             |
  | `iter_difference_t`                                 | ![][c20ok]   |             |
  | `iter_rvalue_reference_t`                           | ![][c20ok]   |             |
  | `iter_common_reference_t`                           | ![][c20ok]   |             |
  | `iterator_traits`                                   | ![][c98]     | ![][c20ok]  |
  | `move_sentinel`                                     | ![][c20ok]   |             |
  | `basic_const_iterator`                              | ![][c23ok]   |             |
  | `const_iterator`                                    | ![][c23ok]   |             |
  | `const_sentinel`                                    | ![][c23ok]   |             |
  | `common_iterator`                                   | ![][c20ok]   |             |
  | `default_sentinel_t`<br/>`default_sentinel`         | ![][c20ok]   |             |
  | `counted_iterator`                                  | ![][c20ok]   |             |
  | `unreachable_sentinel_t`<br/>`unreachable_sentinel` | ![][c20ok]   |             |
  | `iter_move`                                         | ![][c20ok]   |             |
  | `iter_swap`                                         | ![][c20ok]   |             |
  | `make_const_iterator`<br/>`make_const_sentinel`     | ![][c23ok]   |             |
  | `ranges::advance`                                   | ![][c20ok]   |             |
  | `ranges::distance`                                  | ![][c20ok]   |             |
  | `ranges::next`                                      | ![][c20ok]   |             |
  | `ranges::prev`                                      | ![][c20ok]   |             |
  | `size(C)`                                           | ![][c17ok]   | ![][c20ok]  |
  | `ssize(C)`                                          | ![][c20ok]   |             |
  | `empty(C)`                                          | ![][c17no]   | ![][c20no]  |
  | `data(C)`                                           | ![][c17no]   |             |

  * Notes
    * `contiguous_iterator_tag`
      * Alias to `std::contiguous_iterator_tag` if defined, `preview::detail::pseudo_contiguous_iterator_tag` otherwise.
    <br/><br/>
    * `contiguous_iterator<I>` 
      * May incorrectly evaluates to `true` for some `random_access_iterator` if 
        `I::iterator_category` does not satisfy `derived_from<contiguous_iterator_tag>`(typically before C++20)
      * Following pre-C++20 iterators explicitly evaluates to `false`
        * `std::vector<bool>::xxx_iterator`
        * `std::deque<T>::xxx_iterator`
      <br/><br/>
    * `projected`
      * Indirect layer doesn't work without using concepts. 
        Check `preview::projectable` before using `preview::projected` directly.


#### `<latch>`

  |         | Introduced | Revision |
  |---------|------------|----------|
  | `latch` | ![][c20no] |          |

#### `<linalg>`

  |                                                         | Introduced | Revision |
  |---------------------------------------------------------|------------|----------|
  | `layout_blas_packed`                                    | ![][c26no] |          |
  | `scaled_accessor`                                       | ![][c26no] |          |
  | `conjugated_accessor`                                   | ![][c26no] |          |
  | `layout_transpose`                                      | ![][c26no] |          |
  | `column_major`<br/>`column_major_t`                     | ![][c26no] |          |
  | `row_major`<br/>`row_major_t`                           | ![][c26no] |          |
  | `upper_triangle`<br/>`upper_triangle_t`                 | ![][c26no] |          |
  | `lower_triangle`<br/>`lower_triangle_t`                 | ![][c26no] |          |
  | `implicit_unit_diagonal`<br/>`implicit_unit_diagonal_t` | ![][c26no] |          |
  | `explicit_unit_diagonal`<br/>`explicit_unit_diagonal_t` | ![][c26no] |          |
  | `scaled`                                                | ![][c26no] |          |
  | `conjugated`                                            | ![][c26no] |          |
  | `transposed`                                            | ![][c26no] |          |
  | `conjugated_transposed`                                 | ![][c26no] |          |
  | `setup_givens_rotation`                                 | ![][c26no] |          |
  | `apply_givens_rotation`                                 | ![][c26no] |          |
  | `swap_elements`                                         | ![][c26no] |          |
  | `scale`                                                 | ![][c26no] |          |
  | `copy`                                                  | ![][c26no] |          |
  | `add`                                                   | ![][c26no] |          |
  | `dot`<br/>`dotc`                                        | ![][c26no] |          |
  | `vector_sum_of_squares`                                 | ![][c26no] |          |
  | `vector_two_norm`                                       | ![][c26no] |          |
  | `vector_abs_sum`                                        | ![][c26no] |          |
  | `vector_idx_abs_max`                                    | ![][c26no] |          |
  | `matrix_frob_norm`                                      | ![][c26no] |          |
  | `matrix_one_norm`                                       | ![][c26no] |          |
  | `matrix_inf_norm`                                       | ![][c26no] |          |
  | `matrix_vector_product`                                 | ![][c26no] |          |
  | `symmetric_matrix_vector_product`                       | ![][c26no] |          |
  | `hermitian_matrix_vector_product`                       | ![][c26no] |          |
  | `triangular_matrix_vector_product`                      | ![][c26no] |          |
  | `triangular_matrix_vector_solve`                        | ![][c26no] |          |
  | `matrix_rank_1_update`                                  | ![][c26no] |          |
  | `matrix_rank_1_update_c`                                | ![][c26no] |          |
  | `symmetric_matrix_rank_1_update`                        | ![][c26no] |          |
  | `hermitian_matrix_rank_1_update`                        | ![][c26no] |          |
  | `symmetric_matrix_rank_2_update`                        | ![][c26no] |          |
  | `hermitian_matrix_rank_2_update`                        | ![][c26no] |          |
  | `matrix_product`                                        | ![][c26no] |          |
  | `symmetric_matrix_product`                              | ![][c26no] |          |
  | `hermitian_matrix_product`                              | ![][c26no] |          |
  | `triangular_matrix_product`                             | ![][c26no] |          |
  | `triangular_matrix_left_product`                        | ![][c26no] |          |
  | `triangular_matrix_right_product`                       | ![][c26no] |          |
  | `symmetric_matrix_rank_k_update`                        | ![][c26no] |          |
  | `hermitian_matrix_rank_k_update`                        | ![][c26no] |          |
  | `symmetric_matrix_rank_2k_update`                       | ![][c26no] |          |
  | `hermitian_matrix_rank_2k_update`                       | ![][c26no] |          |
  | `triangular_matrix_matrix_left_solve`                   | ![][c26no] |          |
  | `triangular_matrix_matrix_right_solve`                  | ![][c26no] |          |

#### `<mdspan>`

  |                                   | Introduced | Revision |
  |-----------------------------------|------------|----------|
  | `mdspan`                          | ![][c23no] |          |
  | `extents`                         | ![][c23no] |          |
  | `layout_left`                     | ![][c23no] |          |
  | `layout_right`                    | ![][c23no] |          |
  | `layout_stride`                   | ![][c23no] |          |
  | `layout_stride`                   | ![][c23no] |          |
  | `submdspan`                       | ![][c26no] |          |
  | `submdspan_extents`               | ![][c26no] |          |
  | `strided_slice`                   | ![][c26no] |          |
  | `submdspan_mapping_result`        | ![][c26no] |          |
  | `full_extent`<br/>`full_extent_t` | ![][c26no] |          |

#### `<memory>`

  |                                                   | Introduced  | Revision   |
  |---------------------------------------------------|-------------|------------|
  | `pointer_traits`                                  | ![][c11]    | ![][c20ok] |
  | `aligned_alloc`                                   | ![][c17no]  |            |
  | `destroy_at`                                      | ![][c17ok]  | ![][c20ok] |
  | `destroy`                                         | ![][c17ok]  | ![][c20ok] |
  | `destroy_n`                                       | ![][c17ok]  | ![][c20ok] |
  | `xxx_pointer_cast`                                | ![][c17ok]  | ![][c20ok] |
  | `uninitialized_default_construct`                 | ![][c17no]  | ![][c20no] |
  | `uninitialized_default_construct_n`               | ![][c17no]  | ![][c20no] |
  | `uninitialized_move`                              | ![][c17no]  | ![][c20no] |
  | `uninitialized_move_n`                            | ![][c17no]  | ![][c20no] |
  | `uninitialized_value_construct`                   | ![][c17no]  |            |
  | `uninitialized_value_construct_n`                 | ![][c17no]  |            |
  | `atomic<shared_ptr>`                              | ![][c20no]  |            |
  | `atomic<unique_ptr>`                              | ![][c20no]  |            |
  | `assume_aligned`                                  | ![][c20no]  |            |
  | `construct_at`                                    | ![][c20ok]  |            |
  | `make_shared_for_overwrite`                       | ![][c20no]  |            |
  | `make_unique_for_overwrite`                       | ![][c20no]  |            |
  | `allocate_shared_for_overwrite`                   | ![][c20no]  |            |
  | `to_address`                                      | ![][c20ok]* |            |
  | `uses_allocator_construction_args`                | ![][c20ok]  |            |
  | `make_obj_using_allocator`                        | ![][c20ok]  |            |
  | `uninitialized_construct_using_allocator`         | ![][c20no]  |            |
  | `operator<<(std::unique_ptr)`                     | ![][c20no]  |            |
  | `ranges::construct_at`                            | ![][c20no]  |            |
  | `ranges::destroy`                                 | ![][c20no]  |            |
  | `ranges::destroy_n`                               | ![][c20no]  |            |
  | `ranges::destroy_at`                              | ![][c20no]  |            |
  | `ranges::uninitialized_copy`                      | ![][c20no]  |            |
  | `ranges::uninitialized_copy_n`                    | ![][c20no]  |            |
  | `ranges::uninitialized_fill`                      | ![][c20no]  |            |
  | `ranges::uninitialized_fill_n`                    | ![][c20no]  |            |
  | `ranges::uninitialized_move`                      | ![][c20no]  |            |
  | `ranges::uninitialized_move_n`                    | ![][c20no]  |            |
  | `ranges::uninitialized_default_construct`         | ![][c20no]  |            |
  | `ranges::uninitialized_default_construct_n`       | ![][c20no]  |            |
  | `ranges::uninitialized_value_construct`           | ![][c20no]  |            |
  | `ranges::uninitialized_value_construct_n`         | ![][c20no]  |            |
  | `allocation_result`                               | ![][c23no]  |            |
  | `inout_ptr`<br/>`inout_ptr_t`                     | ![][c23no]  |            |
  | `out_ptr`<br/>`out_ptr_t`                         | ![][c23no]  |            |
  | `start_lifetime_as`<br/>`start_lifetime_as_array` | ![][c23no]  |            |
  | `owner_hash`                                      | ![][c26no]  |            |
  | `owner_equal`                                     | ![][c26no]  |            |

* Notes
  * `to_address`
    * If `std::pointer_traits::to_address` is available, it is used before `preview::pointer_traits::to_address`.
      * ⚠️ `std::pointer_traits::to_address` is not sfinae-friendly until MSVC 2022, so not used.

#### `<memory_resource>`
  N/A

#### `<mutex>`

  |               | Introduced | Revision |
  |---------------|------------|----------|
  | `scoped_lock` | ![][c17no] |          |

#### `<new>`

  |                                              | Introduced | Revision |
  |----------------------------------------------|------------|----------|
  | `align_val_t`                                | ![][c17no] |          |
  | `hardware_destructive_interference_size`     | ![][c17no] |          |
  | `hardware_constructive_interference_size`    | ![][c17no] |          |
  | `launder`                                    | ![][c17no] |          |
  | `destroying_delete`<br>`destroying_delete_t` | ![][c20no] |          |

#### `<numbers>`

  |              | Introduced | Revision |
  |--------------|------------|----------|
  | `e`          | ![][c20ok] |          |
  | `log2e`      | ![][c20ok] |          |
  | `log10e`     | ![][c20ok] |          |
  | `pi`         | ![][c20ok] |          |
  | `inv_pi`     | ![][c20ok] |          |
  | `inv_sqrtpi` | ![][c20ok] |          |
  | `ln2`        | ![][c20ok] |          |
  | `ln10`       | ![][c20ok] |          |
  | `sqrt2`      | ![][c20ok] |          |
  | `sqrt3`      | ![][c20ok] |          |
  | `inv_sqrt3`  | ![][c20ok] |          |
  | `egamma`     | ![][c20ok] |          |
  | `phi`        | ![][c20ok] |          |

#### `<numeric>`

  |                            | Introduced | Revision   |
  |----------------------------|------------|------------|
  | `exclusive_scan`           | ![][c17no] | ![][c20no] |
  | `transform_exclusive_scan` | ![][c17no] | ![][c20no] |
  | `inclusive_scan`           | ![][c17no] | ![][c20no] |
  | `transform_inclusive_scan` | ![][c17no] | ![][c20no] |
  | `gcd`                      | ![][c17no] |            |
  | `lcm`                      | ![][c17no] |            |
  | `reduce`                   | ![][c17no] |            |
  | `transform_reduce`         | ![][c17no] |            |
  | `midpoint`                 | ![][c17no] |            |
  | `ranges::iota`             | ![][c23ok] |            |
  | `add_sat`                  | ![][c26no] |            |
  | `sub_sat`                  | ![][c26no] |            |
  | `mul_sat`                  | ![][c26no] |            |
  | `div_sat`                  | ![][c26no] |            |
  | `saturate_cast`            | ![][c26no] |            |

#### `<print>`

  |                              | Introduced | Revision |
  |------------------------------|------------|----------|
  | `print`                      | ![][c23no] |          |
  | `println`                    | ![][c23no] |          |
  | `vprint_unicode`             | ![][c23no] |          |
  | `vprint_unicode_buffered`    | ![][c23no] |          |
  | `vprint_nonunicode`          | ![][c23no] |          |
  | `vprint_nonunicode_buffered` | ![][c23no] |          |


#### `<optional>`

  |                       | Introduced  | Revision              |
  |-----------------------|-------------|-----------------------|
  | `optional`            | ![][c17ok]  | ![][c23ok] ![][c26no] |
  | `bad_optional_access` | ![][c17ok]  |                       |
  | `std::hash<optional>` | ![][c17ok]  |                       |
  | `nullopt`             | ![][c17ok]* |                       |
  | `nullopt_t`           | ![][c17ok]* |                       |
  | `swap(optional)`      | ![][c17ok]  |                       |
  | `make_optional`       | ![][c17ok]  |                       |

* Notes
  * `nullopt`, `nullopt_t`
    * `std::nullopt` is used if available, `preview::nullopt` otherwise.

#### `<random>`

  |                                | Introduced | Revision |
  |--------------------------------|------------|----------|
  | `uniform_random_bit_generator` | ![][c20ok] |          |

#### `<ranges>`

  |                                                                   | Introduced   | Revision   |
  |-------------------------------------------------------------------|--------------|------------|
  | `ranges::range`                                                   | ![][c20ok]   |            |
  | `ranges::borrowed_range`                                          | ![][c20ok]   |            |
  | `ranges::sized_range`                                             | ![][c20ok]   |            |
  | `ranges::view`                                                    | ![][c20ok]   |            |
  | `ranges::input_range`                                             | ![][c20ok]   |            |
  | `ranges::output_range`                                            | ![][c20ok]   |            |
  | `ranges::forward_range`                                           | ![][c20ok]   |            |
  | `ranges::bidirectional_range`                                     | ![][c20ok]   |            |
  | `ranges::random_access_range`                                     | ![][c20ok]   |            |
  | `ranges::contiguous_range`                                        | ![][c20ok]   |            |
  | `ranges::common_range`                                            | ![][c20ok]   |            |
  | `ranges::viewable_range`                                          | ![][c20ok]   |            |
  | `ranges::constant_range`                                          | ![][c23ok]   |            |
  | `ranges::to`                                                      | ![][c23ok] * |            |
  | `ranges::iterator_t`                                              | ![][c20ok]   |            |
  | `ranges::const_iterator_t`                                        | ![][c23ok]   |            |
  | `ranges::sentinel_t`                                              | ![][c20ok]   |            |
  | `ranges::const_sentinel_t`                                        | ![][c23ok]   |            |
  | `ranges::range_difference_t`                                      | ![][c20ok]   |            |
  | `ranges::range_size_t`                                            | ![][c20ok]   |            |
  | `ranges::range_value_t`                                           | ![][c20ok]   |            |
  | `ranges::range_refernce_t`                                        | ![][c20ok]   |            |
  | `ranges::range_const_reference_t`                                 | ![][c23ok]   |            |
  | `ranges::range_rvalue_reference_t`                                | ![][c20ok]   |            |
  | `ranges::range_common_reference_t`                                | ![][c20ok]   |            |
  | `ranges::view_interface`                                          | ![][c20ok]   | ![][c23ok] |
  | `ranges::subrange`                                                | ![][c20ok]   | ![][c23ok] |
  | `ranges::dangling`                                                | ![][c20ok]   |            |
  | `ranges::borrowed_iterator_t`                                     | ![][c20ok]   |            |
  | `ranges::borrowed_subrange_t`                                     | ![][c20ok]   |            |
  | `ranges::range_adaptor_closure`                                   | ![][c23ok]   |            |
  | `ranges::empty_view`<br/>`views::empty`                           | ![][c20ok]   |            |
  | `ranges::single_view`<br/>`views::single`                         | ![][c20ok]   |            |
  | `ranges::iota_view`<br/>`views::iota`                             | ![][c20ok]   |            |
  | `ranges::iota_view`<br/>`views::iota`                             | ![][c20ok]   |            |
  | `ranges::basic_istream_view`<br/>`views::istream`                 | ![][c20ok]   |            |
  | `ranges::repeat_view`<br/>`views::repeat`                         | ![][c23ok]   |            |
  | `ranges::cartesian_product_view`<br/>`views::cartesian_product`   | ![][c23ok]   |            |
  | `views::all_t`<br/>`views::all`                                   | ![][c20ok]   |            |
  | `ranges::ref_view`                                                | ![][c20ok]   |            |
  | `ranges::owning_view`                                             | ![][c20ok]   |            |
  | `ranges::filter_view`<br/>`views::filter`                         | ![][c20ok]   |            |
  | `ranges::transform_view`<br/>`views::transform`                   | ![][c20ok]   | ![][c23ok] |
  | `ranges::take_view`<br/>`views::take`                             | ![][c20ok]   |            |
  | `ranges::take_while_view`<br/>`views::take_while`                 | ![][c20ok]   |            |
  | `ranges::drop_view`<br/>`views::drop`                             | ![][c20ok]   | ![][c23ok] |
  | `ranges::drop_while_view`<br/>`views::drop_while`                 | ![][c20ok]   |            |
  | `ranges::join_view`<br/>`views::join`                             | ![][c20ok]   |            |
  | `ranges::lazy_split_view`<br/>`views::lazy_split`                 | ![][c20ok]   |            |
  | `ranges::split_view`<br/>`views::split`                           | ![][c20ok]   |            |
  | `views::counted`                                                  | ![][c20ok]   |            |
  | `ranges::common_view`<br/>`views::common`                         | ![][c20ok]   |            |
  | `ranges::reverse_view`<br/>`views::reverse`                       | ![][c20ok]   |            |
  | `ranges::as_const_view`<br/>`views::as_const`                     | ![][c23ok]   |            |
  | `ranges::as_rvalue_view`<br/>`views::as_rvalue`                   | ![][c23no]   |            |
  | `ranges::elements_view`<br/>`views::elements`                     | ![][c20ok]   |            |
  | `ranges::keys_view`<br/>`views::keys`                             | ![][c20ok]   |            |
  | `ranges::values_view`<br/>`views::values`                         | ![][c20ok]   |            |
  | `ranges::enumerate_view`<br/>`views::enumerate`                   | ![][c23ok]   |            |
  | `ranges::zip_view`<br/>`views::zip`                               | ![][c23ok]   |            |
  | `ranges::zip_transform_view`<br/>`views::zip_transform`           | ![][c23ok]   |            |
  | `ranges::adjacent_view`<br/>`views::adjacent`                     | ![][c23no]   |            |
  | `ranges::adjacent_transform_view`<br/>`views::adjacent_transform` | ![][c23no]   |            |
  | `ranges::join_with_view`<br/>`views::join_with`                   | ![][c23ok]   |            |
  | `ranges::stride_view`<br/>`views::stride`                         | ![][c23no]   |            |
  | `ranges::slide_view`<br/>`views::slide`                           | ![][c23no]   |            |
  | `ranges::chunk_view`<br/>`views::chunk`                           | ![][c23no]   |            |
  | `ranges::chunk_by_view`<br/>`views::chunk_by`                     | ![][c23no]   |            |
  | `ranges::concat_view`<br/>`views::concat`                         | ![][c26ok] * |            |
  | `ranges::begin`                                                   | ![][c20ok]   | ![][c23ok] |
  | `ranges::end`                                                     | ![][c20ok]   | ![][c23ok] |
  | `ranges::cbegin`                                                  | ![][c20ok]   | ![][c23ok] |
  | `ranges::cend`                                                    | ![][c20ok]   | ![][c23ok] |
  | `ranges::crbegin`                                                 | ![][c20ok]   | ![][c23ok] |
  | `ranges::crend`                                                   | ![][c20ok]   | ![][c23ok] |
  | `ranges::size`                                                    | ![][c20ok]   | ![][c23ok] |
  | `ranges::ssize`                                                   | ![][c20ok]   | ![][c23ok] |
  | `ranges::empty`                                                   | ![][c20ok]   | ![][c23ok] |
  | `ranges::data`                                                    | ![][c20ok]   | ![][c23ok] |
  | `ranges::cdata`                                                   | ![][c20ok]   | ![][c23ok] |
  | `ranges::subrange_kind`                                           | ![][c20ok]   |            |
  | `ranges::from_range_t`<br/>`ranges::from_range`                   | ![][c23ok]   |            |

  * Notes
    * `ranges::to`
      * CTAD for `Args...` may be incorrect before C++17
      * Equipped with C++23 conversions(e.g., `pair-like` -> `std::pair`)
    * `ranges::concat_view`
      * Implemented before standardization
      * Needs update


#### `<rcu>`

  |                      | Introduced | Revision |
  |----------------------|------------|----------|
  | `rcu_obj_base`       | ![][c26no] |          |
  | `rcu_domain`         | ![][c26no] |          |
  | `rcu_default_domain` | ![][c26no] |          |
  | `rcu_synchronize`    | ![][c26no] |          |
  | `rcu_barrier`        | ![][c26no] |          |
  | `rcu_retire`         | ![][c26no] |          |

#### `<semaphore>`

  |                      | Introduced | Revision |
  |----------------------|------------|----------|
  | `counting_semaphore` | ![][c20no] |          |
  | `binary_semaphore`   | ![][c20no] |          |

#### `<shared_mutex>`

  |                    | Introduced | Revision |
  |--------------------|------------|----------|
  | `shared_mutex`     | ![][c17no] |          |

#### `<stop_token>`

  |                 | Introduced | Revision |
  |-----------------|------------|----------|
  | `stop_token`    | ![][c20no] |          |
  | `stop_source`   | ![][c20no] |          |
  | `stop_callback` | ![][c20no] |          |
  | `nostopstate_t` | ![][c20no] |          |
  | `nostopstate`   | ![][c20no] |          |

#### `<string>`

  |                               | Introduced | Revision   |
  |-------------------------------|------------|------------|
  | `erase(std::basic_string)`    | ![][c20no] | ![][c26no] |
  | `erase_if(std::basic_string)` | ![][c20no] | ![][c26no] |

#### `<string_view>`

  |                                | Introduced | Revision                           |
  |--------------------------------|------------|------------------------------------|
  | `basic_string_view`            | ![][c17ok] | ![][c20ok] ![][c23ok] ![][c26ok] * |
  | `std::hash<basic_string_view>` | ![][c17ok] |                                    |
  | `swap(optional)`               | ![][c17ok] |                                    |
  | `operator""_sv`                | ![][c17ok] |                                    |

  * Notes
    * C++26 
      * [P2591 (R5)](https://wg21.link/P2591R5)
      * [P2697 (R1)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2697r1.pdf)
        * Implemented as `preview::basic_string_view::operator bitset`
      * [P2495 (R3)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2495r3.pdf)
        * Implemented as `preview::basic_string_view::operator string-streams`
        * Memory allocation is performed (conversion to `std::basic_string`)

#### `<source_location>`
  N/A

  |                   | Introduced | Revision |
  |-------------------|------------|----------|
  | `source_location` | ![][c20no] |          |

#### `<syncstream>`

  |                            | Introduced | Revision |
  |----------------------------|------------|----------|
  | `basic_syncbuf`            | ![][c20no] |          |
  | `basic_osyncstream`        | ![][c20no] |          |
  | `std::swap(basic_syncbuf)` | ![][c20no] |          |

#### `<span>`

  |                     | Introduced | Revision              |
  |---------------------|------------|-----------------------|
  | `span`              | ![][c20ok] | ![][c23ok] ![][c26ok] |
  | `dynamic_extent`    | ![][c20ok] |                       |
  | `as_bytes`          | ![][c20ok] |                       |
  | `as_writable_bytes` | ![][c20ok] |                       |

#### `<spanstream>`

  |                     | Introduced | Revision |
  |---------------------|------------|----------|
  | `basic_spanbuf`     | ![][c23no] |          |
  | `basic_ispanstream` | ![][c23no] |          |
  | `basic_ospanstream` | ![][c23no] |          |
  | `basic_spanstream`  | ![][c23no] |          |

#### `<stacktrace>`

  |                           | Introduced | Revision |
  |---------------------------|------------|----------|
  | `stacktrace_entry`        | ![][c23no] |          |
  | `basic_stacktrace`        | ![][c23no] |          |
  | `hash<stracktrace_entry>` | ![][c23no] |          |
  | `hash<basic_stacktrace>`  | ![][c23no] |          |

#### `<text_encoding>`

  |                 | Introduced | Revision |
  |-----------------|------------|----------|
  | `text_encoding` | ![][c26no] |          |

#### `<thread>`

  |           | Introduced | Revision |
  |-----------|------------|----------|
  | `jthread` | ![][c20no] |          |

#### `<tuple>`

  |                   | Introduced | Revision   |
  |-------------------|------------|------------|
  | `apply`           | ![][c17ok] | ![][c23ok] |
  | `make_from_tuple` | ![][c17ok] | ![][c23ok] |

#### `<type_traits>`

  |                                                 | Introduced     | Revision              |
  |-------------------------------------------------|----------------|-----------------------|
  | `common_type`                                   | ![][c14]       | ![][c20ok] ![][c23ok] |
  | `bool_constant`                                 | ![][c17ok]     |                       |
  | `has_unique_object_representations`             | ![][c17no] N/A |                       |
  | `is_aggregate`                                  | ![][c17no] N/A |                       |
  | `is_swappable_with`<br/>`is_swappable`...       | ![][c17ok]     |                       |
  | `is_invocable`                                  | ![][c17ok]     |                       |
  | `invoke_result`                                 | ![][c17ok]     |                       |
  | `void_t`                                        | ![][c17ok]     |                       |
  | `conjunction`                                   | ![][c17ok]     |                       |
  | `disjunction`                                   | ![][c17ok]     |                       |
  | `negation`                                      | ![][c17ok]     |                       |
  | `common_reference`<br/>`basic_common_reference` | ![][c20ok]     | ![][c23ok]            |
  | `is_bounded_array`                              | ![][c20ok]     |                       |
  | `is_unbounded_array`                            | ![][c20ok]     |                       |
  | `is_scoped_enum`                                | ![][c20no] N/A |                       |
  | `is_constant_evaluated`                         | ![][c20no] N/A |                       |
  | `is_corresponding_member`                       | ![][c20no] N/A |                       |
  | `is_layout_compatible`                          | ![][c20no] N/A |                       |
  | `is_nothrow_convertible`                        | ![][c20ok]     |                       |
  | `is_pointer_interconvertible_base_of`           | ![][c20no] N/A |                       |
  | `is_pointer_interconvertible_with_class`        | ![][c20no] N/A |                       |
  | `remove_cvref`                                  | ![][c20ok]     |                       |
  | `type_identity`                                 | ![][c20ok]     |                       |
  | `is_implicit_lifetime`                          | ![][c23no] N/A |                       |
  | `is_within_lifetime`                            | ![][c26no] N/A |                       |
  | `is_virtual_base_of`                            | ![][c26no]     |                       |

* Notes
  * `common_type`
    * If `common_type` fails to define member typedef `type`, it falls back to `std::common_type`
    ```c++
    struct common_a {};
    struct common_b {};
    struct common_c {};
    
    template<>
    struct std::common_type<common_a, common_b> {
      using type = common_c;
    };
    
    // fallback to std::common_type for user specialization types
    static_assert(std::is_same_v<preview::common_type_t<common_a, common_b>, common_c>);
    
    // MSVC and GCC (before C++23) does not satisfy C++23 standard for std::common_type<tuple-like>  
    static_assert(std::is_same_v<
        std::common_type_t<std::tuple<int, int>, std::pair<int, double>>, 
        std::pair<int, int>>);
    
    // Since std::common_type does not satisfy C++23, preview::common_type does not fallback to std::common_type
    static_assert(std::is_same_v<
        preview::common_type_t< std::tuple<int, int>, std::pair<int, double>>, 
        std::pair<int, double>>);
    ```


#### `<utility>`

  |                             | Introduced     | Revision |
  |-----------------------------|----------------|----------|
  | `as_const`                  | ![][c17ok]     |          |
  | `in_place`<br/>`in_place_t` | ![][c17ok]     |          |
  | `cmp_equal`...              | ![][c20ok]     |          |
  | `in_range`                  | ![][c20ok]     |          |
  | `forward_like`              | ![][c23ok]     |          |
  | `to_underlying`             | ![][c23ok]     |          |
  | `unreachable`               | ![][c23no] N/A |          |
  | `nontype`<br/>`nontype_t`   | ![][c26ok]     |          |

#### `<variant>`

  |                       | Introduced | Revision   |
  |-----------------------|------------|------------|
  | `variant`             | ![][c17ok] | ![][c26ok] |
  | `monostate`           | ![][c17ok] |            |
  | `bad_variant_access`  | ![][c17ok] |            |
  | `variant_size`        | ![][c17ok] |            |
  | `variant_alternative` | ![][c17ok] |            |
  | `std::hash<variant>`  | ![][c17ok] |            |
  | `variant_npos`        | ![][c17ok] |            |
  | `visit`               | ![][c17ok] |            |
  | `get_if`              | ![][c17ok] |            |

#### `<version>`

  |                      | Introduced | Revision |
  |----------------------|------------|----------|
  |                      |            |          |


[c98]: https://img.shields.io/badge/C++98-grey
[c11]: https://img.shields.io/badge/C++11-grey
[c14]: https://img.shields.io/badge/C++14-grey
[c17]: https://img.shields.io/badge/C++17-0055AA
[c20]: https://img.shields.io/badge/C++20-4477BB
[c23]: https://img.shields.io/badge/C++23-skyblue
[c26]: https://img.shields.io/badge/C++26-99EEFF

[c17no]: https://img.shields.io/badge/C++17-red
[c17ok]: https://img.shields.io/badge/C++14-C++17-0055AA

[c20no]: https://img.shields.io/badge/C++20-red
[c20ok]: https://img.shields.io/badge/C++14-C++20-4477BB
[c20ok17]: https://img.shields.io/badge/C++17-C++20-4477BB

[c23no]: https://img.shields.io/badge/C++23-red
[c23ok]: https://img.shields.io/badge/C++14-C++23-skyblue
[c23ok17]: https://img.shields.io/badge/C++17-C++23-skyblue
[c23ok20]: https://img.shields.io/badge/C++20-C++23-skyblue

[c26no]: https://img.shields.io/badge/C++26-red
[c26ok]: https://img.shields.io/badge/C++14-C++26-CCEEFF
[c26ok17]: https://img.shields.io/badge/C++17-C++26-77EEFF
[c26ok20]: https://img.shields.io/badge/C++20-C++26-77EEFF
[c26ok23]: https://img.shields.io/badge/C++23-C++26-77EEFF


[p000]: https://img.shields.io/badge/0%25-red
[p001]: https://img.shields.io/badge/1%25-orange
[p002]: https://img.shields.io/badge/2%25-orange
[p003]: https://img.shields.io/badge/3%25-orange
[p004]: https://img.shields.io/badge/4%25-orange
[p005]: https://img.shields.io/badge/5%25-orange
[p006]: https://img.shields.io/badge/6%25-orange
[p007]: https://img.shields.io/badge/7%25-orange
[p008]: https://img.shields.io/badge/8%25-orange
[p009]: https://img.shields.io/badge/9%25-orange
[p010]: https://img.shields.io/badge/10%25-orange
[p011]: https://img.shields.io/badge/11%25-orange
[p012]: https://img.shields.io/badge/12%25-orange
[p013]: https://img.shields.io/badge/13%25-orange
[p014]: https://img.shields.io/badge/14%25-orange
[p015]: https://img.shields.io/badge/15%25-orange
[p016]: https://img.shields.io/badge/16%25-orange
[p017]: https://img.shields.io/badge/17%25-orange
[p018]: https://img.shields.io/badge/18%25-orange
[p019]: https://img.shields.io/badge/19%25-orange
[p020]: https://img.shields.io/badge/20%25-orange
[p021]: https://img.shields.io/badge/21%25-orange
[p022]: https://img.shields.io/badge/22%25-orange
[p023]: https://img.shields.io/badge/23%25-orange
[p024]: https://img.shields.io/badge/24%25-orange
[p025]: https://img.shields.io/badge/25%25-yellow
[p026]: https://img.shields.io/badge/26%25-yellow
[p027]: https://img.shields.io/badge/27%25-yellow
[p028]: https://img.shields.io/badge/28%25-yellow
[p029]: https://img.shields.io/badge/29%25-yellow
[p030]: https://img.shields.io/badge/30%25-yellow
[p031]: https://img.shields.io/badge/31%25-yellow
[p032]: https://img.shields.io/badge/32%25-yellow
[p033]: https://img.shields.io/badge/33%25-yellow
[p034]: https://img.shields.io/badge/34%25-yellow
[p035]: https://img.shields.io/badge/35%25-yellow
[p036]: https://img.shields.io/badge/36%25-yellow
[p037]: https://img.shields.io/badge/37%25-yellow
[p038]: https://img.shields.io/badge/38%25-yellow
[p039]: https://img.shields.io/badge/39%25-yellow
[p040]: https://img.shields.io/badge/40%25-yellow
[p041]: https://img.shields.io/badge/41%25-yellow
[p042]: https://img.shields.io/badge/42%25-yellow
[p043]: https://img.shields.io/badge/43%25-yellow
[p044]: https://img.shields.io/badge/44%25-yellow
[p045]: https://img.shields.io/badge/45%25-yellow
[p046]: https://img.shields.io/badge/46%25-yellow
[p047]: https://img.shields.io/badge/47%25-yellow
[p048]: https://img.shields.io/badge/48%25-yellow
[p049]: https://img.shields.io/badge/49%25-yellow
[p050]: https://img.shields.io/badge/50%25-yellowgreen
[p051]: https://img.shields.io/badge/51%25-yellowgreen
[p052]: https://img.shields.io/badge/52%25-yellowgreen
[p053]: https://img.shields.io/badge/53%25-yellowgreen
[p054]: https://img.shields.io/badge/54%25-yellowgreen
[p055]: https://img.shields.io/badge/55%25-yellowgreen
[p056]: https://img.shields.io/badge/56%25-yellowgreen
[p057]: https://img.shields.io/badge/57%25-yellowgreen
[p058]: https://img.shields.io/badge/58%25-yellowgreen
[p059]: https://img.shields.io/badge/59%25-yellowgreen
[p060]: https://img.shields.io/badge/60%25-yellowgreen
[p061]: https://img.shields.io/badge/61%25-yellowgreen
[p062]: https://img.shields.io/badge/62%25-yellowgreen
[p063]: https://img.shields.io/badge/63%25-yellowgreen
[p064]: https://img.shields.io/badge/64%25-yellowgreen
[p065]: https://img.shields.io/badge/65%25-yellowgreen
[p066]: https://img.shields.io/badge/66%25-yellowgreen
[p067]: https://img.shields.io/badge/67%25-yellowgreen
[p068]: https://img.shields.io/badge/68%25-yellowgreen
[p069]: https://img.shields.io/badge/69%25-yellowgreen
[p070]: https://img.shields.io/badge/70%25-yellowgreen
[p071]: https://img.shields.io/badge/71%25-yellowgreen
[p072]: https://img.shields.io/badge/72%25-yellowgreen
[p073]: https://img.shields.io/badge/73%25-yellowgreen
[p074]: https://img.shields.io/badge/74%25-yellowgreen
[p075]: https://img.shields.io/badge/75%25-green
[p076]: https://img.shields.io/badge/76%25-green
[p077]: https://img.shields.io/badge/77%25-green
[p078]: https://img.shields.io/badge/78%25-green
[p079]: https://img.shields.io/badge/79%25-green
[p080]: https://img.shields.io/badge/80%25-green
[p081]: https://img.shields.io/badge/81%25-green
[p082]: https://img.shields.io/badge/82%25-green
[p083]: https://img.shields.io/badge/83%25-green
[p084]: https://img.shields.io/badge/84%25-green
[p085]: https://img.shields.io/badge/85%25-green
[p086]: https://img.shields.io/badge/86%25-green
[p087]: https://img.shields.io/badge/87%25-green
[p088]: https://img.shields.io/badge/88%25-green
[p089]: https://img.shields.io/badge/89%25-green
[p090]: https://img.shields.io/badge/90%25-green
[p091]: https://img.shields.io/badge/91%25-green
[p092]: https://img.shields.io/badge/92%25-green
[p093]: https://img.shields.io/badge/93%25-green
[p094]: https://img.shields.io/badge/94%25-green
[p095]: https://img.shields.io/badge/95%25-green
[p096]: https://img.shields.io/badge/96%25-green
[p097]: https://img.shields.io/badge/97%25-green
[p098]: https://img.shields.io/badge/98%25-green
[p099]: https://img.shields.io/badge/99%25-green
[p100]: https://img.shields.io/badge/100%25-brightgreen
