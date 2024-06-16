# STL Preview
![logo.png](logo.png)

## Description
**Use the latest STL implementation in C++14**

```c++
// All implementations are written under C++14 standard semantics
#include "preview/ranges.h"

auto map = preview::views::iota('A', 'E') | 
           preview::views::enumerate | 
           preview::ranges::to<std::map>();

std::cout << map[0] << ' '
          << map[1] << ' '
          << map[2] << '\n';
// A B C
```

**`preview` is standard-conforming, and is compatible with existing STL**
```c++
auto r = preview::views::iota(0) |
      preview::views::take(10) |
      preview::views::filter([](auto x) { return x % 2 == 0; });

// Compatible with both pre C++20 and post C++20 std::iterator_traits
static_assert(std::is_same<
    std::iterator_traits<decltype(r.begin())>::reference,
    int
>::value, "");

// Others such as std::get, std::tuple_size are also compatible 
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

  for (auto c : preview::views::concat(v, s, sv, l)) {
    std::cout << c;
  }
  // hello, world!
}
```

## Compiler Support
[![Build Status](https://github.com/lackhole/stl-preview/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/lackhole/stl-preview/actions/workflows/cmake-multi-platform.yml)

| Compiler    | Minimum version tested                | Maximum version tested                |
|-------------|---------------------------------------|---------------------------------------|
| MSVC        | 19.29.30154.0 (Visual Studio 16 2019) | 19.40.33811.0 (Visual Studio 17 2022) |
| gcc         | 9.5.0                                 | 11.4.0                                |
| clang       | 13.0.1                                | 15.0.7                                |
| Apple clang | 14.0.0.14000029                       | 15.0.0.15000040 (Silicon)             |
| Android NDK | r18 (clang 7.0)                       | r26 (clang 17.0.2)                    |
| Emscripten  | 3.1.20 (clang 16.0.0)                 | latest(3.1.61) (clang 19.0.0)         |


## Features
Description 
* ![](https://img.shields.io/badge/C++XX-C++YY-CCEEFF): C++YY standard implemented in C++XX
* ![](https://img.shields.io/badge/C++YY-red): C++YY standard, not implemented yet
* If the implementation is impossible(i.e., needs compiler support / hardware info) it is marked as **N/A**
* Some features are working in progress
* **Introduced**: First introduced version 
* **Revision**: Behavior changed/updated version

-----

* `<algorithm>` (61/114)

  |                                     | Introduced | Revision   |
  |-------------------------------------|------------|------------|
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
  | `ranges::count`                     | ![][c20ok] |            |
  | `ranges::count_if`                  | ![][c20ok] |            |
  | `ranges::mismatch`                  | ![][c20ok] |            |
  | `ranges::find`                      | ![][c20ok] |            |
  | `ranges::find_if`                   | ![][c20ok] |            |
  | `ranges::find_if_not`               | ![][c20ok] |            |
  | `ranges::find_last`                 | ![][c23ok] |            |
  | `ranges::find_last_if` `            | ![][c23ok] |            |
  | `ranges::find_last_if_not`          | ![][c23ok] |            |
  | `ranges::find_end`                  | ![][c20ok] |            |
  | `ranges::find_first_of`             | ![][c20ok] |            |
  | `ranges::adjacent_find`             | ![][c20no] |            |
  | `ranges::search`                    | ![][c20ok] |            |
  | `ranges::search_n`                  | ![][c20no] |            |
  | `ranges::contains`                  | ![][c23ok] |            |
  | `ranges::contains_subrange`         | ![][c23ok] |            |
  | `ranges::starts_with`               | ![][c23no] |            |
  | `ranges::ends_with`                 | ![][c23no] |            |
  | `ranges::fold_left`                 | ![][c23ok] |            |
  | `ranges::fold_left_first`           | ![][c23no] |            |
  | `ranges::fold_right`                | ![][c23no] |            |
  | `ranges::fold_right_last`           | ![][c23no] |            |
  | `ranges::fold_left_with_iter`       | ![][c23no] |            |
  | `ranges::fold_left_first_with_iter` | ![][c23no] |            |
  | `ranges::copy`                      | ![][c20ok] |            |
  | `ranges::copy_if`                   | ![][c20no] |            |
  | `ranges::copy_n`                    | ![][c20no] |            |
  | `ranges::copy_backward`             | ![][c20no] |            |
  | `ranges::move`                      | ![][c20no] |            |
  | `ranges::move_backward`             | ![][c20no] |            |
  | `ranges::fill`                      | ![][c20no] |            |
  | `ranges::fill_n`                    | ![][c20no] |            |
  | `ranges::transform`                 | ![][c20no] |            |
  | `ranges::generate`                  | ![][c20no] |            |
  | `ranges::generate_n`                | ![][c20no] |            |
  | `ranges::remove`                    | ![][c20no] |            |
  | `ranges::remove_if`                 | ![][c20no] |            |
  | `ranges::remove_copy`               | ![][c20no] |            |
  | `ranges::remove_copy_if`            | ![][c20no] |            |
  | `ranges::replace`                   | ![][c20no] |            |
  | `ranges::replace_if`                | ![][c20no] |            |
  | `ranges::replace_copy`              | ![][c20no] |            |
  | `ranges::replace_copy_if`           | ![][c20no] |            |
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
  | `ranges::stable_partition`          | ![][c20no] |            |
  | `ranges::partition_point`           | ![][c20no] |            |
  | `ranges::is_sorted`                 | ![][c20no] |            |
  | `ranges::is_sorted_until`           | ![][c20no] |            |
  | `ranges::sort`                      | ![][c20ok] |            |
  | `ranges::partial_sort`              | ![][c20no] |            |
  | `ranges::partial_sort_copy`         | ![][c20no] |            |
  | `ranges::stable_sort`               | ![][c20no] |            |
  | `ranges::nth_element`               | ![][c20no] |            |
  | `ranges::lower_bound`               | ![][c20no] |            |
  | `ranges::upper_bound`               | ![][c20no] |            |
  | `ranges::binary_search`             | ![][c20no] |            |
  | `ranges::equal_range`               | ![][c20no] |            |
  | `ranges::merge`                     | ![][c20no] |            |
  | `ranges::inplace_merge`             | ![][c20no] |            |
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

* `<any>` (0/5)

  |                | Introduced | Revision |
  |----------------|------------|----------|
  | `any`          | ![][c17no] |          |
  | `bad_any_cast` | ![][c17no] |          |
  | `swap(any)`    | ![][c17no] |          |
  | `make_any`     | ![][c17no] |          |
  | `any_cast`     | ![][c17no] |          |

* `<array>` (1/1)

  |            | Introduced | Revision |
  |------------|------------|----------|
  | `to_array` | ![][c20ok] |          | 

* `<atomic>` (0/17)

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

* `<barrier>` (0/1)

  |           | Introduced | Revision |
  |-----------|------------|----------|
  | `barrier` | ![][c20no] |          |

* `<bit>` (0/14)

  |                  | Introduced | Revision |
  |------------------|------------|----------|
  | `endian`         | ![][c20no] |          |
  | `bit_cast`       | ![][c20no] |          |
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

* `<charconv>` (0/5)

  |                     | Introduced | Revision   |
  |---------------------|------------|------------|
  | `chars_format`      | ![][c17no] |            |
  | `from_chars_result` | ![][c17no] |            |
  | `to_chars_result`   | ![][c17no] |            |
  | `from_chars`        | ![][c17no] | ![][c23no] |
  | `to_chars`          | ![][c17no] | ![][c23no] |

* `<chrono>`

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

* `<cmath>`

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

* `<compare>` N/A

* `<concepts>`

  |                         | Introduced | Revision |
  |-------------------------|------------|----------|
  | `same_as`               | ![][c20ok] |          |
  | `derived_from`          | ![][c20ok] |          |
  | `convertible_to`        | ![][c20ok] |          |
  | `common_reference_with` | ![][c20ok] |          |
  | `common_with`           | ![][c20ok] |          |
  | `integral`              | ![][c20ok] |          |
  | `signed_integral`       | ![][c20ok] |          |
  | `unsigned_integral`     | ![][c20ok] |          |
  | `floating_point`        | ![][c20ok] |          |
  | `assignable_from`       | ![][c20ok] |          |
  | `swappable`             | ![][c20ok] |          |
  | `destructible`          | ![][c20ok] |          |
  | `constructible_from`    | ![][c20ok] |          |
  | `default_initializable` | ![][c20ok] |          |
  | `move_constructible`    | ![][c20ok] |          |
  | `copy_constructible`    | ![][c20ok] |          |
  | `equality_comparable`   | ![][c20ok] |          |
  | `totally_ordered`       | ![][c20ok] |          |
  | `movable`               | ![][c20ok] |          |
  | `copyable`              | ![][c20ok] |          |
  | `semiregular`           | ![][c20ok] |          |
  | `regular`               | ![][c20ok] |          |
  | `invocable`             | ![][c20ok] |          |
  | `regular_invocable`     | ![][c20ok] |          |
  | `predicate`             | ![][c20ok] |          |
  | `relation`              | ![][c20ok] |          |
  | `equivalence_relation`  | ![][c20ok] |          |
  | `strict_weak_order`     | ![][c20ok] |          |
  | `ranges::swap`          | ![][c20ok] |          |

* `<coroutine>` N/A

* `<cstddef>` (2/2)

  |              | Introduced | Revision |
  |--------------|------------|----------|
  | `byte`       | ![][c17ok] |          |
  | `to_integer` | ![][c17ok] |          |

* `<exception>`

  |                       | Introduced       | Revision |
  |-----------------------|------------------|----------|
  | `uncaught_exceptions` | ![][c17no] (N/A) |          |

* `<expected>` (4/4)

  |                             | Introduced | Revision |
  |-----------------------------|------------|----------|
  | `expected`                  | ![][c23ok] |          |
  | `unexpected`                | ![][c23ok] |          |
  | `bad_expected_access`       | ![][c23ok] |          |
  | `unexpect`<br/>`unexpect_t` | ![][c23ok] |          |

* `<execution>` N/A

* `<flat_map>` (0/4)

  |                                               | Introduced | Revision |
  |-----------------------------------------------|------------|----------|
  | `flat_map`                                    | ![][c23no] |          |
  | `flat_multimap`                               | ![][c23no] |          |
  | `sorted_unique`<br/>`sorted_unique_t`         | ![][c23no] |          |
  | `sorted_equivalent`<br/>`sorted_equivalent_t` | ![][c23no] |          |

* `<flat_set>` (0/2)

  |                 | Introduced | Revision |
  |-----------------|---------------|-----------------|
  | `flat_set`      | ![][c23no]      |                 |
  | `flat_multiset` | ![][c23no]      |                 |

* `<format>`

  |                      | Introduced | Revision |
  |----------------------|------------|----------|
  |                      |            |          |

* `<filesystem>` N/A

* `<functional>` (6/16)

  |                                 | Introduced | Revision   |
  |---------------------------------|------------|------------|
  | `default_searcher`              | ![][c17no] | ![][c20no] |
  | `boyer_moore_searcher`          | ![][c17no] |            |
  | `boyer_moore_horspool_searcher` | ![][c17no] |            |
  | `invoke`                        | ![][c17ok] |            |
  | `invoke_r`                      | ![][c23ok] |            |
  | `not_fn`                        | ![][c17no] | ![][c26no] |
  | `bind_front`                    | ![][c20ok] |            |
  | `bind_back`                     | ![][c23ok] |            |
  | `identity`                      | ![][c20ok] |            |
  | `unwrap_reference`              | ![][c20no] |            |
  | `unwrap_ref_decay`              | ![][c20no] |            |
  | `ranges::equal_to`...           | ![][c20ok] |            |
  | `ranges::three_way`             | ![][c20no] |            |
  | `move_only_function`            | ![][c23no] |            |
  | `copyable_function`             | ![][c26no] |            |
  | `function_ref`                  | ![][c26no] |            |

* `<hazard_pointer>`

  |                           | Introduced | Revision |
  |---------------------------|------------|----------|
  | `hazard_pointer_obj_base` | ![][c26no] |          |
  | `hazard_pointer`          | ![][c26no] |          |
  | `make_hazard_pointer`     | ![][c26no] |          |

* `<latch>`

  |         | Introduced | Revision |
  |---------|------------|----------|
  | `latch` | ![][c20no] |          |

* `<linalg>`

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
  | ...                                                     | ![][c26no] |          |

* `<mdspan>`

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

* `<memory>`

  |                                                   | Introduced | Revision   |
  |---------------------------------------------------|------------|------------|
  | `aligned_alloc`                                   | ![][c17no] |            |
  | `assume_aligned`                                  | ![][c20no] |            |
  | `construct_at`                                    | ![][c20ok] |            |
  | `destroy_at`                                      | ![][c17ok] | ![][c20ok] |
  | `destroy`                                         | ![][c17ok] | ![][c20ok] |
  | `destroy_n`                                       | ![][c17no] | ![][c20no] |
  | `reinterpret_pointer_cast`                        | ![][c17no] | ![][c20no] |
  | `uninitialized_default_construct`                 | ![][c17no] | ![][c20no] |
  | `uninitialized_default_construct_n`               | ![][c17no] | ![][c20no] |
  | `uninitialized_move`                              | ![][c17no] | ![][c20no] |
  | `uninitialized_move_n`                            | ![][c17no] | ![][c20no] |
  | `uninitialized_value_construct`                   | ![][c17no] |            |
  | `uninitialized_value_construct_n`                 | ![][c17no] |            |
  | `make_obj_using_allocator`                        | ![][c20no] |            |
  | `make_shared_for_overwrite`                       | ![][c20no] |            |
  | `make_unique_for_overwrite`                       | ![][c20no] |            |
  | `to_address`                                      | ![][c20ok] |            |
  | `uninitialized_construct_using_allocator`         | ![][c20no] |            |
  | `uses_allocator_construction_args`                | ![][c20no] |            |
  | `operator<<(std::unique_ptr)`                     | ![][c20no] |            |
  | `pointer_traits`                                  | ![][c11]   | ![][c20ok] |
  | `ranges::construct_at`                            | ![][c20no] |            |
  | `ranges::destroy`                                 | ![][c20no] |            |
  | `ranges::destroy_n`                               | ![][c20no] |            |
  | `ranges::destroy_at`                              | ![][c20no] |            |
  | `ranges::uninitialized_copy`                      | ![][c20no] |            |
  | `ranges::uninitialized_copy_n`                    | ![][c20no] |            |
  | `ranges::uninitialized_fill`                      | ![][c20no] |            |
  | `ranges::uninitialized_fill_n`                    | ![][c20no] |            |
  | `ranges::uninitialized_move`                      | ![][c20no] |            |
  | `ranges::uninitialized_move_n`                    | ![][c20no] |            |
  | `ranges::uninitialized_default_construct`         | ![][c20no] |            |
  | `ranges::uninitialized_default_construct_n`       | ![][c20no] |            |
  | `ranges::uninitialized_value_construct`           | ![][c20no] |            |
  | `ranges::uninitialized_value_construct_n`         | ![][c20no] |            |
  | `inout_ptr`<br/>`inout_ptr_t`                     | ![][c23no] |            |
  | `out_ptr`<br/>`out_ptr_t`                         | ![][c23no] |            |
  | `start_lifetime_as`<br/>`start_lifetime_as_array` | ![][c23no] |            |
  | `owner_hash`                                      | ![][c26no] |            |
  | `owner_equal`                                     | ![][c26no] |            |

* `<memory_resource>` N/A

* `<mutex>`

  |               | Introduced | Revision |
  |---------------|------------|----------|
  | `scoped_lock` | ![][c17no] |          |

* `<new>`

  |                                           | Introduced | Revision |
  |-------------------------------------------|------------|----------|
  | `hardware_destructive_interference_size`  | ![][c17no] |          |
  | `hardware_constructive_interference_size` | ![][c17no] |          |
  | `launder`                                 | ![][c17no] |          |

* `<numbers>`

  |              | Introduced | Revision |
  |--------------|------------|----------|
  | `e`          | ![][c20no] |          |
  | `log2e`      | ![][c20no] |          |
  | `log10e`     | ![][c20no] |          |
  | `pi`         | ![][c20no] |          |
  | `inv_pi`     | ![][c20no] |          |
  | `inv_sqrtpi` | ![][c20no] |          |
  | `ln2`        | ![][c20no] |          |
  | `ln10`       | ![][c20no] |          |
  | `sqrt2`      | ![][c20no] |          |
  | `sqrt3`      | ![][c20no] |          |
  | `inv_sqrt3`  | ![][c20no] |          |
  | `egamma`     | ![][c20no] |          |
  | `phi`        | ![][c20no] |          |

* `<numeric>`

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
  | `ranges::iota`             | ![][c23no] |            |
  | `add_sat`                  | ![][c26no] |            |
  | `sub_sat`                  | ![][c26no] |            |
  | `mul_sat`                  | ![][c26no] |            |
  | `div_sat`                  | ![][c26no] |            |
  | `saturate_cast`            | ![][c26no] |            |

* `<optional>`

  |                       | Introduced | Revision   |
  |-----------------------|------------|------------|
  | `optional`            | ![][c17ok] | ![][c23ok] |
  | `bad_optional_access` | ![][c17ok] |            |
  | `std::hash<optional>` | ![][c17ok] |            |
  | `nullopt`             | ![][c17ok] |            |
  | `nullopt_t`           | ![][c17ok] |            |
  | `swap(optional)`      | ![][c17ok] |            |
  | `make_optional`       | ![][c17ok] |            |

* `<random>`

  |                                | Introduced | Revision |
  |--------------------------------|------------|----------|
  | `uniform_random_bit_generator` | ![][c20no] |          |

* `<ranges>`

  |                                                                   | Introduced           | Revision   |
  |-------------------------------------------------------------------|----------------------|------------|
  | `ranges::range`                                                   | ![][c20ok]           |            |
  | `ranges::borrowed_range`                                          | ![][c20ok]           |            |
  | `ranges::sized_range`                                             | ![][c20ok]           |            |
  | `ranges::view`                                                    | ![][c20ok]           |            |
  | `ranges::input_range`                                             | ![][c20ok]           |            |
  | `ranges::output_range`                                            | ![][c20ok]           |            |
  | `ranges::forward_range`                                           | ![][c20ok]           |            |
  | `ranges::bidirectional_range`                                     | ![][c20ok]           |            |
  | `ranges::random_access_range`                                     | ![][c20ok]           |            |
  | `ranges::contiguous_range`                                        | ![][c20ok]           |            |
  | `ranges::common_range`                                            | ![][c20ok]           |            |
  | `ranges::viewable_range`                                          | ![][c20ok]           |            |
  | `ranges::constant_range`                                          | ![][c20ok]           |            |
  | `ranges::to`                                                      | ![][c23ok] (Partial) |            |
  | `ranges::iterator_t`                                              | ![][c20ok]           |            |
  | `ranges::const_iterator_t`                                        | ![][c23ok]           |            |
  | `ranges::sentinel_t`                                              | ![][c20ok]           |            |
  | `ranges::const_sentinel_t`                                        | ![][c23ok]           |            |
  | `ranges::range_difference_t`                                      | ![][c20ok]           |            |
  | `ranges::range_size_t`                                            | ![][c20ok]           |            |
  | `ranges::range_value_t`                                           | ![][c20ok]           |            |
  | `ranges::range_refernce_t`                                        | ![][c20ok]           |            |
  | `ranges::range_const_reference_t`                                 | ![][c20ok]           |            |
  | `ranges::range_rvalue_reference_t`                                | ![][c20ok]           |            |
  | `ranges::range_common_reference_t`                                | ![][c20ok]           |            |
  | `ranges::view_interface`                                          | ![][c20ok]           |            |
  | `ranges::subrange`                                                | ![][c20ok]           |            |
  | `ranges::dangling`                                                | ![][c20ok]           |            |
  | `ranges::borrowed_iterator_t`                                     | ![][c20ok]           |            |
  | `ranges::borrowed_subrange_t`                                     | ![][c20ok]           |            |
  | `ranges::range_adaptor_closure`                                   | ![][c23ok]           |            |
  | `ranges::empty_view`<br/>`views::empty`                           | ![][c20ok]           |            |
  | `ranges::single_view`<br/>`views::single`                         | ![][c20ok]           |            |
  | `ranges::iota_view`<br/>`views::iota`                             | ![][c20ok]           |            |
  | `ranges::iota_view`<br/>`views::iota`                             | ![][c20ok]           |            |
  | `ranges::basic_istream_view`<br/>`views::istream`                 | ![][c20ok]           |            |
  | `ranges::repeat_view`<br/>`views::repeat`                         | ![][c23ok]           |            |
  | `ranges::cartesian_product_view`<br/>`views::cartesian_product`   | ![][c23ok]           |            |
  | `views::all_t`<br/>`views::all`                                   | ![][c20ok]           |            |
  | `ranges::ref_view`                                                | ![][c20ok]           |            |
  | `ranges::owning_view`                                             | ![][c20ok]           |            |
  | `ranges::filter_view`<br/>`views::filter`                         | ![][c20ok]           |            |
  | `ranges::transform_view`<br/>`views::transform`                   | ![][c20ok]           |            |
  | `ranges::take_view`<br/>`views::take`                             | ![][c20ok]           |            |
  | `ranges::take_while_view`<br/>`views::take_while`                 | ![][c20ok]           |            |
  | `ranges::drop_view`<br/>`views::drop`                             | ![][c20ok]           |            |
  | `ranges::drop_while_view`<br/>`views::drop_while`                 | ![][c20ok]           |            |
  | `ranges::join_view`<br/>`views::join`                             | ![][c20ok]           |            |
  | `ranges::lazy_split_view`<br/>`views::lazy_split`                 | ![][c20ok]           |            |
  | `ranges::split_view`<br/>`views::split`                           | ![][c20ok]           |            |
  | `views::counted`                                                  | ![][c20ok]           |            |
  | `ranges::common_view`<br/>`views::common`                         | ![][c20ok]           |            |
  | `ranges::reverse_view`<br/>`views::reverse`                       | ![][c20ok]           |            |
  | `ranges::as_const_view`<br/>`views::as_const`                     | ![][c23ok]           |            |
  | `ranges::as_rvalue_view`<br/>`views::as_rvalue`                   | ![][c23no]           |            |
  | `ranges::elements_view`<br/>`views::elements`                     | ![][c20ok]           |            |
  | `ranges::keys_view`<br/>`views::keys`                             | ![][c20ok]           |            |
  | `ranges::values_view`<br/>`views::values`                         | ![][c20ok]           |            |
  | `ranges::enumerate_view`<br/>`views::enumerate`                   | ![][c23ok]           |            |
  | `ranges::zip_view`<br/>`views::zip`                               | ![][c23ok]           |            |
  | `ranges::zip_transform_view`<br/>`views::zip_transform`           | ![][c23ok]           |            |
  | `ranges::adjacent_view`<br/>`views::adjacent`                     | ![][c23no]           |            |
  | `ranges::adjacent_transform_view`<br/>`views::adjacent_transform` | ![][c23no]           |            |
  | `ranges::join_with_view`<br/>`views::join_with`                   | ![][c23ok]           |            |
  | `ranges::stride_view`<br/>`views::stride`                         | ![][c23no]           |            |
  | `ranges::slide_view`<br/>`views::slide`                           | ![][c23no]           |            |
  | `ranges::chunk_view`<br/>`views::chunk`                           | ![][c23no]           |            |
  | `ranges::chunk_by_view`<br/>`views::chunk_by`                     | ![][c23no]           |            |
  | `ranges::concat_view`<br/>`views::concat`                         | ![][c26ok]           |            |
  | `ranges::begin`                                                   | ![][c20ok]           |            |
  | `ranges::end`                                                     | ![][c20ok]           |            |
  | `ranges::cbegin`                                                  | ![][c20ok]           |            |
  | `ranges::cend`                                                    | ![][c20ok]           |            |
  | `ranges::crbegin`                                                 | ![][c20ok]           |            |
  | `ranges::crend`                                                   | ![][c20ok]           |            |
  | `ranges::size`                                                    | ![][c20ok]           |            |
  | `ranges::ssize`                                                   | ![][c20ok]           |            |
  | `ranges::empty`                                                   | ![][c20ok]           |            |
  | `ranges::data`                                                    | ![][c20ok]           |            |
  | `ranges::cdata`                                                   | ![][c20ok]           | ![][c23ok] |
  | `ranges::subrange_kind`                                           | ![][c20ok]           |            |
  | `ranges::from_range_t`<br/>`ranges::from_range`                   | ![][c23ok]           |            |

* `<semaphore>`

  |                      | Introduced | Revision |
  |----------------------|------------|----------|
  | `counting_semaphore` | ![][c20no] |          |
  | `binary_semaphore`   | ![][c20no] |          |

* `<stop_token>`

  |                 | Introduced | Revision |
  |-----------------|------------|----------|
  | `stop_token`    | ![][c20no] |          |
  | `stop_source`   | ![][c20no] |          |
  | `stop_callback` | ![][c20no] |          |
  | `nostopstate_t` | ![][c20no] |          |
  | `nostopstate`   | ![][c20no] |          |

* `<string>`

  |                               | Introduced | Revision   |
  |-------------------------------|------------|------------|
  | `erase(std::basic_string)`    | ![][c20no] | ![][c26no] |
  | `erase_if(std::basic_string)` | ![][c20no] | ![][c26no] |

* `<string_view>`

  |                                | Introduced | Revision   |
  |--------------------------------|------------|------------|
  | `basic_string_view`            | ![][c17ok] | ![][c20ok] |
  | `std::hash<basic_string_view>` | ![][c17ok] |            |
  | `swap(optional)`               | ![][c17ok] |            |
  | `operator""_sv`                | ![][c17ok] |            |

* `<source_location>` N/A

* `<syncstream>`

  |                            | Introduced | Revision |
  |----------------------------|------------|----------|
  | `basic_syncbuf`            | ![][c20no] |          |
  | `basic_osyncstream`        | ![][c20no] |          |
  | `std::swap(basic_syncbuf)` | ![][c20no] |          |

* `<span>`

  |                     | Introduced | Revision   |
  |---------------------|------------|------------|
  | `span`              | ![][c20ok] | ![][c26ok] |
  | `dynamic_extent`    | ![][c20ok] |            |
  | `as_bytes`          | ![][c20ok] |            |
  | `as_writable_bytes` | ![][c20ok] |            |

* `<spanstream>`

  |                     | Introduced | Revision |
  |---------------------|------------|----------|
  | `basic_spanbuf`     | ![][c23no] |          |
  | `basic_ispanstream` | ![][c23no] |          |
  | `basic_ospanstream` | ![][c23no] |          |
  | `basic_spanstream`  | ![][c23no] |          |

* `<thread>`

  |           | Introduced | Revision |
  |-----------|------------|----------|
  | `jthread` | ![][c20no] |          |

* `<tuple>`

  |                   | Introduced | Revision   |
  |-------------------|------------|------------|
  | `apply`           | ![][c17ok] | ![][c23ok] |
  | `make_from_tuple` | ![][c17ok] | ![][c23ok] |

* `<type_traits>`

  |                                                 | Introduced     | Revision              |
  |-------------------------------------------------|----------------|-----------------------|
  | `common_type`                                   | ![][c14]       | ![][c20ok] ![][c23ok] |
  | `conjunction`                                   | ![][c17ok]     |                       |
  | `bool_constant`                                 | ![][c17ok]     |                       |
  | `disjunction`                                   | ![][c17ok]     |                       |
  | `has_unique_object_representations`             | ![][c17no] N/A |                       |
  | `is_aggregate`                                  | ![][c17no] N/A |                       |
  | `is_swappable`                                  | ![][c17ok]     |                       |
  | `is_invocable`                                  | ![][c17ok]     |                       |
  | `invoke_result`                                 | ![][c17ok]     |                       |
  | `negation`                                      | ![][c17ok]     |                       |
  | `void_t`                                        | ![][c17ok]     |                       |
  | `common_reference`<br/>`basic_common_reference` | ![][c20ok]     |                       |
  | `is_bounded_array`                              | ![][c20ok]     |                       |
  | `is_constant_evaluated`                         | ![][c20no] N/A |                       |
  | `is_corresponding_member`                       | ![][c20no] N/A |                       |
  | `is_layout_compatible`                          | ![][c20no] N/A |                       |
  | `is_nothrow_convertible`                        | ![][c20ok]     |                       |
  | `is_pointer_interconvertible_base_of`           | ![][c20no] N/A |                       |
  | `is_pointer_interconvertible_with_class`        | ![][c20no] N/A |                       |
  | `is_scoped_enum`                                | ![][c20no] N/A |                       |
  | `is_unbounded_array`                            | ![][c20ok]     |                       |
  | `remove_cvref`                                  | ![][c20ok]     |                       |
  | `type_identity`                                 | ![][c20ok]     |                       |
  | `is_implicit_lifetime`                          | ![][c23no] N/A |                       |
  | `is_within_lifetime`                            | ![][c26no] N/A |                       |
  | `arity`                                         | Extended       |                       |
  | `conditional`                                   | Extended       |                       |
  | `copy_cvref`                                    | Extended       |                       |
  | `copy_template`                                 | Extended       |                       |
  | `has_typename_xxx`                              | Extended       |                       |
  | `is_complete`                                   | Extended       |                       |
  | `is_explicitly_constructible`                   | Extended       |                       |
  | `is_explicitly_convertible`                     | Extended       |                       |
  | `is_implicitly_constructible`                   | Extended       |                       |
  | `is_initializer_list`                           | Extended       |                       |
  | `is_integer_like`                               | Extended       |                       |
  | `is_list_initializable`                         | Extended       |                       |
  | `is_referenceable`                              | Extended       |                       |
  | `is_specialization`                             | Extended       |                       |
  | `maybe_const`                                   | Extended       |                       |
  | `template_arity`                                | Extended       |                       |

* `<utility>`

  |                             | Introduced     | Revision |
  |-----------------------------|----------------|----------|
  | `as_const`                  | ![][c17ok]     |          |
  | `in_place`<br/>`in_place_t` | ![][c17ok]     |          |
  | `cmp_equal`                 | ![][c20ok]     |          |
  | `in_range`                  | ![][c20ok]     |          |
  | `forward_like`              | ![][c23no]     |          |
  | `to_underlying`             | ![][c23ok]     |          |
  | `nontype`<br/>`nontype_t`   | ![][c23ok]     |          |
  | `unreachable`               | ![][c23no] N/A |          |

* `<variant>`

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

* `<version>`

  |                      | Introduced | Revision |
  |----------------------|------------|----------|
  |                      |            |          |

* Others

  |            | Introduced | Revision   |
  |------------|------------|------------|
  | `size(C)`  | ![][c17ok] | ![][c20ok] |
  | `empty(C)` | ![][c17no] | ![][c20no] |
  | `data(C)`  | ![][c17no] |            |


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
