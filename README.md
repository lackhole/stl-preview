# STL Preview
![logo.png](logo.png)

## Description
Use the latest STL implementation in C++14

(Code migration in progress)

## Compiler Support

| Compiler    | Minimum version tested                | Maximum version tested                |
|-------------|---------------------------------------|---------------------------------------|
| MSVC        | 19.29.30154.0 (Visual Studio 16 2019) | 19.40.33811.0 (Visual Studio 17 2022) |
| gcc         | 9.5.0                                 | 11.4.0                                |
| clang       | 13.0.1                                | 15.0.7                                |
| Apple clang | 14.0.0.14000029                       | 15.0.0.15000040 (Silicon)             |
| Android NDK | r18 (clang 7.0)                       | r26 (clang 17.0.2)                    |
| Emscripten  | 3.1.20 (clang 16.0.0)                 | latest(3.1.61) (clang 19.0.0)         |


## Features

* `<algorithm>`

  |                                                                               | Introduced in | Latest standard | Implementation status |
  |-------------------------------------------------------------------------------|---------------|-----------------|-----------------------|
  | `clamp`                                                                       | ![][c17]      | ![][c20]        |                       |
  | `for_each_n`                                                                  | ![][c17]      | ![][c20]        |                       |
  | `sample`                                                                      | ![][c17]      | ![][c23]        |                       |
  | `shift_left`<br/>`shift_right`                                                | ![][c20]      |                 |                       |
  | `ranges::in_fun_result`                                                       | ![][c20]      |                 |                       |
  | `ranges::in_in_result`                                                        | ![][c20]      |                 |                       |
  | `ranges::in_out_result`                                                       | ![][c20]      |                 |                       |
  | `ranges::in_in_out_result`                                                    | ![][c20]      |                 |                       |
  | `ranges::in_out_out_result`                                                   | ![][c20]      |                 |                       |
  | `ranges::min_max_result`                                                      | ![][c20]      |                 |                       |
  | `ranges::in_found_result`                                                     | ![][c20]      |                 |                       |
  | `ranges::in_value_result`                                                     | ![][c23]      |                 |                       |
  | `ranges::out_value_result`                                                    | ![][c23]      |                 |                       |
  | `ranges::all_of`<br/>`ranges::any_of`<br/>`ranges::none_of`                   | ![][c20]      |                 |                       |
  | `ranges::for_each`<br/>`ranges::for_each_n`                                   | ![][c20]      |                 |                       |
  | `ranges::count`<br/>`ranges::count_if`                                        | ![][c20]      |                 |                       |
  | `ranges::mismatch`                                                            | ![][c20]      |                 |                       |
  | `ranges::find`<br/>`ranges::find_if`<br/>`ranges::find_if_not`                | ![][c20]      |                 |                       |
  | `ranges::find_last`<br/>`ranges::find_last_if`<br/>`ranges::find_last_if_not` | ![][c23]      |                 |                       |
  | `ranges::find_end`                                                            | ![][c20]      |                 |                       |
  | `ranges::find_first_of`                                                       | ![][c20]      |                 |                       |
  | `ranges::adjacent_find`                                                       | ![][c20]      |                 |                       |
  | `ranges::search`<br/>`ranges::search_n`                                       | ![][c20]      |                 |                       |
  | `ranges::contains`<br/>`ranges::contains_subrange`                            | ![][c23]      |                 |                       |
  | `ranges::starts_with`<br/>`ranges::ends_with`                                 | ![][c23]      |                 |                       |
  | `ranges::fold_left`<br/>`ranges::fold_left_first`                             | ![][c23]      |                 |                       |
  | `ranges::fold_right`<br/>`ranges::fold_right_last`                            | ![][c23]      |                 |                       |
  | `ranges::fold_left_with_iter`<br/>`ranges::fold_left_first_with_iter`         | ![][c23]      |                 |                       |
  | `ranges::copy`<br/>`ranges::copy_if`<br/>`ranges::copy_n`                     | ![][c20]      |                 |                       |
  | `ranges::copy_backward`                                                       | ![][c20]      |                 |                       |
  | `ranges::move`<br/>`ranges::move_backward`                                    | ![][c20]      |                 |                       |
  | `ranges::fill`<br/>`ranges::fill_n`                                           | ![][c20]      |                 |                       |
  | `ranges::transform`                                                           | ![][c20]      |                 |                       |
  | `ranges::generate`<br/>`ranges::generate_n`                                   | ![][c20]      |                 |                       |
  | `ranges::remove`<br/>`ranges::remove_if`                                      | ![][c20]      |                 |                       |
  | `ranges::remove_copy`<br/>`ranges::remove_copy_if`                            | ![][c20]      |                 |                       |
  | `ranges::replace`<br/>`ranges::replace_if`                                    | ![][c20]      |                 |                       |
  | `ranges::replace_copy`<br/>`ranges::replace_copy_if`                          | ![][c20]      |                 |                       |
  | `ranges::swap_ranges`                                                         | ![][c20]      |                 |                       |
  | `ranges::reverse`<br/>`ranges::reverse_copy`                                  | ![][c20]      |                 |                       |
  | `ranges::rotate`<br/>`ranges::rotate_copy`                                    | ![][c20]      |                 |                       |
  | `ranges::shift_left`<br/>`ranges::shift_right`                                | ![][c23]      |                 |                       |
  | `ranges::sample`                                                              | ![][c20]      |                 |                       |
  | `ranges::shuffle`                                                             | ![][c20]      |                 |                       |
  | `ranges::unique`<br/>`ranges::unique_copy`                                    | ![][c20]      |                 |                       |
  | `ranges::is_partitioned`                                                      | ![][c20]      |                 |                       |
  | `ranges::partition`<br/>`ranges::partition_copy`                              | ![][c20]      |                 |                       |
  | `ranges::stable_partition`<br/>`ranges::partition_point`                      | ![][c20]      |                 |                       |
  | `ranges::is_sorted`<br/>`ranges::is_sorted_until`                             | ![][c20]      |                 |                       |
  | `ranges::sort`                                                                | ![][c20]      |                 |                       |
  | `ranges::partial_sort`<br/>`ranges::partial_sort_copy`                        | ![][c20]      |                 |                       |
  | `ranges::stable_sort`                                                         | ![][c20]      |                 |                       |
  | `ranges::nth_element`                                                         | ![][c20]      |                 |                       |
  | `ranges::lower_bound`<br/>`ranges::upper_bound`                               | ![][c20]      |                 |                       |
  | `ranges::binary_search`                                                       | ![][c20]      |                 |                       |
  | `ranges::equal_range`                                                         | ![][c20]      |                 |                       |
  | `ranges::merge`<br/>`ranges::inplace_merge`                                   | ![][c20]      |                 |                       |
  | `ranges::includes`                                                            | ![][c20]      |                 |                       |
  | `ranges::set_difference`                                                      | ![][c20]      |                 |                       |
  | `ranges::set_intersection`                                                    | ![][c20]      |                 |                       |
  | `ranges::set_symmetric_difference`                                            | ![][c20]      |                 |                       |
  | `ranges::set_union`                                                           | ![][c20]      |                 |                       |
  | `ranges::is_heap`<br/>`ranges::is_heap_until`                                 | ![][c20]      |                 |                       |
  | `ranges::make_heap`                                                           | ![][c20]      |                 |                       |
  | `ranges::push_heap`<br/>`ranges::pop_heap`                                    | ![][c20]      |                 |                       |
  | `ranges::sort_heap`                                                           | ![][c20]      |                 |                       |
  | `ranges::max`<br/>`ranges::max_element`                                       | ![][c20]      |                 |                       |
  | `ranges::min`<br/>`ranges::min_element`                                       | ![][c20]      |                 |                       |
  | `ranges::minmax`<br/>`ranges::minmax_element`                                 | ![][c20]      |                 |                       |
  | `ranges::clamp`                                                               | ![][c20]      |                 |                       |
  | `ranges::equal`                                                               | ![][c20]      |                 |                       |
  | `ranges::lexicographical_compare`                                             | ![][c20]      |                 |                       |
  | `ranges::is_permutation`                                                      | ![][c20]      |                 |                       |
  | `ranges::next_permutation`                                                    | ![][c20]      |                 |                       |
  | `ranges::prev_permutation`                                                    | ![][c20]      |                 |                       |

* `<any>`

  |                | Introduced in | Latest standard | Implementation status |
  |----------------|---------------|-----------------|-----------------------|
  | `any`          | ![][c17]      |                 |                       |
  | `bad_any_cast` | ![][c17]      |                 |                       |
  | `swap(any)`    | ![][c17]      |                 |                       |
  | `make_any`     | ![][c17]      |                 |                       |
  | `any_cast`     | ![][c17]      |                 |                       |

* `<array>`

  |            | Introduced in | Latest standard | Implementation status |
  |------------|---------------|-----------------|-----------------------|
  | `to_array` | ![][c20]      |                 |                       | 

* `<atomic>`

  |                                                        | Introduced in   | Latest standard | Implementation status |
  |--------------------------------------------------------|-----------------|-----------------|-----------------------|
  | `atomic_ref`                                           | ![][c20]        |                 |                       | 
  | `atomic_signed_lock_free`                              | ![][c20]        |                 |                       | 
  | `atomic_unsigned_lock_free`                            | ![][c20]        |                 |                       | 
  | `atomic_wait`<br/>`atomic_wait_explicit`               | ![][c20]        |                 |                       | 
  | `atomic_notify_one`<br/>`atomic_notify_all`            | ![][c20]        |                 |                       | 
  | `atomic_flag_test`<br/>`atomic_flag_test_explicit`     | ![][c20]        |                 |                       | 
  | `atomic_flag_wait`<br/>`atomic_flag_wait_explicit`     | ![][c20]        |                 |                       | 
  | `atomic_flag_notify_one`<br/>`atomic_flag_notifly_all` | ![][c20]        |                 |                       | 
  | `atomic_fetch_max`<br/>`atomic_fetch_max_explicit`     | ![][c26]        |                 |                       | 
  | `atomic_fetch_min`<br/>`atomic_fetch_min_explicit`     | ![][c26]        |                 |                       | 

* `<barrier>`

  |           | Introduced in | Latest standard | Implementation status |
  |-----------|---------------|-----------------|-----------------------|
  | `barrier` | ![][c20]      |                 |                       |

* `<bit>`

  |                  | Introduced in  | Latest standard | Implementation status |
  |------------------|----------------|-----------------|-----------------------|
  | `endian`         | ![][c20]       |                 |                       |
  | `bit_cast`       | ![][c20]       |                 |                       |
  | `byteswap`       | ![][c23]       |                 |                       |
  | `has_single_bit` | ![][c20]       |                 |                       |
  | `bit_ceil`       | ![][c20]       |                 |                       |
  | `bit_floor`      | ![][c20]       |                 |                       |
  | `bit_width`      | ![][c20]       |                 |                       |
  | `rotl`           | ![][c20]       |                 |                       |
  | `rotr`           | ![][c20]       |                 |                       |
  | `countl_zero`    | ![][c20]       |                 |                       |
  | `countl_one`     | ![][c20]       |                 |                       |
  | `countr_zero`    | ![][c20]       |                 |                       |
  | `countr_one`     | ![][c20]       |                 |                       |
  | `popcount`       | ![][c20]       |                 |                       |

* `<charconv>`

  |                     | Introduced in | Latest standard | Implementation status |
  |---------------------|---------------|-----------------|-----------------------|
  | `chars_format`      | ![][c17]      |                 |                       |
  | `from_chars_result` | ![][c17]      |                 |                       |
  | `to_chars_result`   | ![][c17]      |                 |                       |
  | `from_chars`        | ![][c17]      | ![][c23]        |                       |
  | `to_chars`          | ![][c17]      | ![][c23]        |                       |

* `<chrono>`

  |                                          | Introduced in | Latest standard | Implementation status |
  |------------------------------------------|---------------|-----------------|-----------------------|
  | `chrono::abs(std::chrono::duration)`     | ![][c17]      |                 |                       |
  | `chrono::ceil(std::chrono::duration)`    | ![][c17]      |                 |                       |
  | `chrono::floor(std::chrono::duration)`   | ![][c17]      |                 |                       |
  | `chrono::round(std::chrono::duration)`   | ![][c17]      |                 |                       |
  | `chrono::ceil(std::chrono::time_point)`  | ![][c17]      |                 |                       |
  | `chrono::floor(std::chrono::time_point)` | ![][c17]      |                 |                       |
  | `chrono::round(std::chrono::time_point)` | ![][c17]      |                 |                       |

* `<cmath>`

  |                  | Introduced in | Latest standard | Implementation status |
  |------------------|---------------|-----------------|-----------------------|
  | `assoc_laguerre` | ![][c17]      | ![][c23]        |                       |
  | `assoc_legendre` | ![][c17]      | ![][c23]        |                       |
  | `beta`           | ![][c17]      | ![][c23]        |                       |
  | `comp_ellint_1`  | ![][c17]      | ![][c23]        |                       |
  | `comp_ellint_2`  | ![][c17]      | ![][c23]        |                       |
  | `comp_ellint_3`  | ![][c17]      | ![][c23]        |                       |
  | `cyl_bessel_i`   | ![][c17]      | ![][c23]        |                       |
  | `cyl_bessel_j`   | ![][c17]      | ![][c23]        |                       |
  | `cyl_bessel_k`   | ![][c17]      | ![][c23]        |                       |
  | `cyl_neumann`    | ![][c17]      | ![][c23]        |                       |
  | `ellint_1`       | ![][c17]      | ![][c23]        |                       |
  | `ellint_2`       | ![][c17]      | ![][c23]        |                       |
  | `ellint_3`       | ![][c17]      | ![][c23]        |                       |
  | `expint`         | ![][c17]      | ![][c23]        |                       |
  | `hermite`        | ![][c17]      | ![][c23]        |                       |
  | `legendre`       | ![][c17]      | ![][c23]        |                       |
  | `laguerre`       | ![][c17]      | ![][c23]        |                       |
  | `riemann_zeta`   | ![][c17]      | ![][c23]        |                       |
  | `sph_bessel`     | ![][c17]      | ![][c23]        |                       |
  | `sph_legendre`   | ![][c17]      | ![][c23]        |                       |
  | `sph_neumann`    | ![][c17]      | ![][c23]        |                       |
  | `hypot(x, y, z)` | ![][c17]      | ![][c26]        |                       |

* `<compare>` N/A

* `<concepts>`

  |                         | Introduced in | Latest standard | Implementation status |
  |-------------------------|---------------|-----------------|-----------------------|
  | `same_as`               | ![][c20]      |                 |                       |
  | `derived_from`          | ![][c20]      |                 |                       |
  | `convertible_to`        | ![][c20]      |                 |                       |
  | `common_reference_with` | ![][c20]      |                 |                       |
  | `common_with`           | ![][c20]      |                 |                       |
  | `integral`              | ![][c20]      |                 |                       |
  | `signed_integral`       | ![][c20]      |                 |                       |
  | `unsigned_integral`     | ![][c20]      |                 |                       |
  | `floating_point`        | ![][c20]      |                 |                       |
  | `assignable_from`       | ![][c20]      |                 |                       |
  | `swappable`             | ![][c20]      |                 |                       |
  | `destructible`          | ![][c20]      |                 |                       |
  | `constructible_from`    | ![][c20]      |                 |                       |
  | `default_initializable` | ![][c20]      |                 |                       |
  | `move_constructible`    | ![][c20]      |                 |                       |
  | `copy_constructible`    | ![][c20]      |                 |                       |
  | `equality_comparable`   | ![][c20]      |                 |                       |
  | `totally_ordered`       | ![][c20]      |                 |                       |
  | `movable`               | ![][c20]      |                 |                       |
  | `copyable`              | ![][c20]      |                 |                       |
  | `semiregular`           | ![][c20]      |                 |                       |
  | `regular`               | ![][c20]      |                 |                       |
  | `invocable`             | ![][c20]      |                 |                       |
  | `regular_invocable`     | ![][c20]      |                 |                       |
  | `predicate`             | ![][c20]      |                 |                       |
  | `relation`              | ![][c20]      |                 |                       |
  | `equivalence_relation`  | ![][c20]      |                 |                       |
  | `strict_weak_order`     | ![][c20]      |                 |                       |
  | `ranges::swap`          | ![][c20]      |                 |                       |

* `<coroutine>` N/A

* `<cstddef>`

  |              | Introduced in  | Latest standard | Implementation status |
  |--------------|----------------|-----------------|-----------------------|
  | `byte`       | ![][c17]       |                 | ![][c17]              |
  | `to_integer` | ![][c17]       |                 |                       |

* `<exception>`

  |                       | Introduced in | Latest standard | Implementation status |
  |-----------------------|---------------|-----------------|-----------------------|
  | `uncaught_exceptions` | ![][c17]      |                 | N/A                   |

* `<expected>`

  |                             | Introduced in | Latest standard | Implementation status |
  |-----------------------------|---------------|-----------------|-----------------------|
  | `expected`                  | ![][c23]      |                 |                       |
  | `unexpected`                | ![][c23]      |                 |                       |
  | `bad_expected_access`       | ![][c23]      |                 |                       |
  | `unexpect`<br/>`unexpect_t` | ![][c23]      |                 |                       |

* `<execution>` N/A

* `<flat_map>`

  |                                               | Introduced in | Latest standard | Implementation status |
  |-----------------------------------------------|---------------|-----------------|-----------------------|
  | `flat_map`                                    | ![][c23]      |                 |                       |
  | `flat_multimap`                               | ![][c23]      |                 |                       |
  | `sorted_unique`<br/>`sorted_unique_t`         | ![][c23]      |                 |                       |
  | `sorted_equivalent`<br/>`sorted_equivalent_t` | ![][c23]      |                 |                       |

* `<flat_set>`

  |                                               | Introduced in | Latest standard | Implementation status |
  |-----------------------------------------------|---------------|-----------------|-----------------------|
  | `flat_set`                                    | ![][c23]      |                 |                       |
  | `flat_multiset`                               | ![][c23]      |                 |                       |

* `<format>`

  |                      | Introduced in | Latest standard | Implementation status |
  |----------------------|---------------|-----------------|-----------------------|
  |                      |               |                 |                       |

* `<filesystem>` N/A

* `<functional>`

  |                                 | Introduced in  | Latest standard | Implementation status |
  |---------------------------------|----------------|-----------------|-----------------------|
  | `default_searcher`              | ![][c17]       | ![][c20]        |                       |
  | `boyer_moore_searcher`          | ![][c17]       |                 |                       |
  | `boyer_moore_horspool_searcher` | ![][c17]       |                 |                       |
  | `invoke`                        | ![][c17]       |                 |                       |
  | `invoke_r`                      | ![][c23]       |                 |                       |
  | `not_fn`                        | ![][c17]       | ![][c26]        |                       |
  | `bind_front`                    | ![][c20]       |                 |                       |
  | `bind_back`                     | ![][c23]       |                 |                       |
  | `identity`                      | ![][c20]       |                 |                       |
  | `unwrap_reference`              | ![][c20]       |                 |                       |
  | `unwrap_ref_decay`              | ![][c20]       |                 |                       |
  | `ranges::equal_to`...           | ![][c20]       |                 |                       |
  | `ranges::three_way`             | ![][c20]       |                 | N/A                   |
  | `move_only_function`            | ![][c23]       |                 |                       |
  | `copyable_function`             | ![][c26]       |                 |                       |
  | `function_ref`                  | ![][c26]       |                 |                       |

* `<hazard_pointer>`

  |                           | Introduced in | Latest standard | Implementation status |
  |---------------------------|---------------|-----------------|-----------------------|
  | `hazard_pointer_obj_base` | ![][c26]      |                 |                       |
  | `hazard_pointer`          | ![][c26]      |                 |                       |
  | `make_hazard_pointer`     | ![][c26]      |                 |                       |

* `<latch>`

  |         | Introduced in | Latest standard | Implementation status |
  |---------|---------------|-----------------|-----------------------|
  | `latch` | ![][c20]      |                 |                       |

* `<linalg>`

  |                                                         | Introduced in | Latest standard | Implementation status |
  |---------------------------------------------------------|---------------|-----------------|-----------------------|
  | `layout_blas_packed`                                    | ![][c26]      |                 |                       |
  | `scaled_accessor`                                       | ![][c26]      |                 |                       |
  | `conjugated_accessor`                                   | ![][c26]      |                 |                       |
  | `layout_transpose`                                      | ![][c26]      |                 |                       |
  | `column_major`<br/>`column_major_t`                     | ![][c26]      |                 |                       |
  | `row_major`<br/>`row_major_t`                           | ![][c26]      |                 |                       |
  | `upper_triangle`<br/>`upper_triangle_t`                 | ![][c26]      |                 |                       |
  | `lower_triangle`<br/>`lower_triangle_t`                 | ![][c26]      |                 |                       |
  | `implicit_unit_diagonal`<br/>`implicit_unit_diagonal_t` | ![][c26]      |                 |                       |
  | `explicit_unit_diagonal`<br/>`explicit_unit_diagonal_t` | ![][c26]      |                 |                       |
  | `scaled`                                                | ![][c26]      |                 |                       |
  | `conjugated`                                            | ![][c26]      |                 |                       |
  | `transposed`                                            | ![][c26]      |                 |                       |
  | `conjugated_transposed`                                 | ![][c26]      |                 |                       |
  | `setup_givens_rotation`                                 | ![][c26]      |                 |                       |
  | `apply_givens_rotation`                                 | ![][c26]      |                 |                       |
  | `swap_elements`                                         | ![][c26]      |                 |                       |
  | `scale`                                                 | ![][c26]      |                 |                       |
  | `copy`                                                  | ![][c26]      |                 |                       |
  | `add`                                                   | ![][c26]      |                 |                       |
  | `dot`<br/>`dotc`                                        | ![][c26]      |                 |                       |
  | `vector_sum_of_squares`                                 | ![][c26]      |                 |                       |
  | `vector_two_norm`                                       | ![][c26]      |                 |                       |
  | `vector_abs_sum`                                        | ![][c26]      |                 |                       |
  | `vector_idx_abs_max`                                    | ![][c26]      |                 |                       |
  | `matrix_frob_norm`                                      | ![][c26]      |                 |                       |
  | `matrix_one_norm`                                       | ![][c26]      |                 |                       |
  | `matrix_inf_norm`                                       | ![][c26]      |                 |                       |
  | `matrix_vector_product`                                 | ![][c26]      |                 |                       |
  | `symmetric_matrix_vector_product`                       | ![][c26]      |                 |                       |
  | ...                                                     | ![][c26]      |                 |                       |

* `<mdspan>`

  |                                   | Introduced in | Latest standard | Implementation status |
  |-----------------------------------|---------------|-----------------|-----------------------|
  | `mdspan`                          | ![][c23]      |                 |                       |
  | `extents`                         | ![][c23]      |                 |                       |
  | `layout_left`                     | ![][c23]      |                 |                       |
  | `layout_right`                    | ![][c23]      |                 |                       |
  | `layout_stride`                   | ![][c23]      |                 |                       |
  | `layout_stride`                   | ![][c23]      |                 |                       |
  | `submdspan`                       | ![][c26]      |                 |                       |
  | `submdspan_extents`               | ![][c26]      |                 |                       |
  | `strided_slice`                   | ![][c26]      |                 |                       |
  | `submdspan_mapping_result`        | ![][c26]      |                 |                       |
  | `full_extent`<br/>`full_extent_t` | ![][c26]      |                 |                       |

* `<memory>`

  |                                                   | Introduced in | Latest standard | Implementation status |
  |---------------------------------------------------|---------------|-----------------|-----------------------|
  | `aligned_alloc`                                   | ![][c17]      |                 | N/A                   |
  | `destroy_at`                                      | ![][c17]      | ![][c20]        |                       |
  | `destroy`                                         | ![][c17]      | ![][c20]        |                       |
  | `destroy_n`                                       | ![][c17]      | ![][c20]        |                       |
  | `reinterpret_pointer_cast`                        | ![][c17]      | ![][c20]        |                       |
  | `uninitialized_default_construct`                 | ![][c17]      | ![][c20]        |                       |
  | `uninitialized_default_construct_n`               | ![][c17]      | ![][c20]        |                       |
  | `uninitialized_move`                              | ![][c17]      | ![][c20]        |                       |
  | `uninitialized_move_n`                            | ![][c17]      | ![][c20]        |                       |
  | `uninitialized_value_construct`                   | ![][c17]      |                 |                       |
  | `uninitialized_value_construct_n`                 | ![][c17]      |                 |                       |
  | `assume_aligned`                                  | ![][c20]      |                 |                       |
  | `construct_at`                                    | ![][c20]      |                 |                       |
  | `destroy_at`                                      | ![][c20]      |                 |                       |
  | `make_obj_using_allocator`                        | ![][c20]      |                 |                       |
  | `make_shared_for_overwrite`                       | ![][c20]      |                 |                       |
  | `make_unique_for_overwrite`                       | ![][c20]      |                 |                       |
  | `to_address`                                      | ![][c20]      |                 |                       |
  | `uninitialized_construct_using_allocator`         | ![][c20]      |                 |                       |
  | `uses_allocator_construction_args`                | ![][c20]      |                 |                       |
  | `operator<<(std::unique_ptr)`                     | ![][c20]      |                 |                       |
  | `ranges::construct_at`                            | ![][c20]      |                 |                       |
  | `ranges::destroy`                                 | ![][c20]      |                 |                       |
  | `ranges::destroy_n`                               | ![][c20]      |                 |                       |
  | `ranges::destroy_at`                              | ![][c20]      |                 |                       |
  | `ranges::uninitialized_copy`                      | ![][c20]      |                 |                       |
  | `ranges::uninitialized_copy_n`                    | ![][c20]      |                 |                       |
  | `ranges::uninitialized_fill`                      | ![][c20]      |                 |                       |
  | `ranges::uninitialized_fill_n`                    | ![][c20]      |                 |                       |
  | `ranges::uninitialized_move`                      | ![][c20]      |                 |                       |
  | `ranges::uninitialized_move_n`                    | ![][c20]      |                 |                       |
  | `ranges::uninitialized_default_construct`         | ![][c20]      |                 |                       |
  | `ranges::uninitialized_default_construct_n`       | ![][c20]      |                 |                       |
  | `ranges::uninitialized_value_construct`           | ![][c20]      |                 |                       |
  | `ranges::uninitialized_value_construct_n`         | ![][c20]      |                 |                       |
  | `inout_ptr`<br/>`inout_ptr_t`                     | ![][c23]      |                 |                       |
  | `out_ptr`<br/>`out_ptr_t`                         | ![][c23]      |                 |                       |
  | `start_lifetime_as`<br/>`start_lifetime_as_array` | ![][c23]      |                 |                       |
  | `owner_hash`                                      | ![][c26]      |                 |                       |
  | `owner_equal`                                     | ![][c26]      |                 |                       |

* `<memory_resource>` N/A

* `<mutex>`

  |                                           | Introduced in | Latest standard | Implementation status |
  |-------------------------------------------|---------------|-----------------|-----------------------|
  | `scoped_lock`                             | ![][c17]      |                 |                       |

* `<new>`

  |                                           | Introduced in | Latest standard | Implementation status |
  |-------------------------------------------|---------------|-----------------|-----------------------|
  | `hardware_destructive_interference_size`  | ![][c17]      |                 | N/A                   |
  | `hardware_constructive_interference_size` | ![][c17]      |                 | N/A                   |
  | `launder`                                 | ![][c17]      |                 | N/A                   |

* `<numbers>`

  |              | Introduced in | Latest standard | Implementation status |
  |--------------|---------------|-----------------|-----------------------|
  | `e`          | ![][c20]      |                 |                       |
  | `log2e`      | ![][c20]      |                 |                       |
  | `log10e`     | ![][c20]      |                 |                       |
  | `pi`         | ![][c20]      |                 |                       |
  | `inv_pi`     | ![][c20]      |                 |                       |
  | `inv_sqrtpi` | ![][c20]      |                 |                       |
  | `ln2`        | ![][c20]      |                 |                       |
  | `ln10`       | ![][c20]      |                 |                       |
  | `sqrt2`      | ![][c20]      |                 |                       |
  | `sqrt3`      | ![][c20]      |                 |                       |
  | `inv_sqrt3`  | ![][c20]      |                 |                       |
  | `egamma`     | ![][c20]      |                 |                       |
  | `phi`        | ![][c20]      |                 |                       |

* `<numeric>`

  |                            | Introduced in  | Latest standard | Implementation status |
  |----------------------------|----------------|-----------------|-----------------------|
  | `exclusive_scan`           | ![][c17]       | ![][c20]        |                       |
  | `transform_exclusive_scan` | ![][c17]       | ![][c20]        |                       |
  | `inclusive_scan`           | ![][c17]       | ![][c20]        |                       |
  | `transform_inclusive_scan` | ![][c17]       | ![][c20]        |                       |
  | `gcd`                      | ![][c17]       |                 |                       |
  | `lcm`                      | ![][c17]       |                 |                       |
  | `reduce`                   | ![][c17]       |                 | N/A                   |
  | `transform_reduce`         | ![][c17]       |                 | N/A                   |
  | `midpoint`                 | ![][c17]       |                 |                       |
  | `ranges::iota`             | ![][c23]       |                 |                       |
  | `add_sat`                  | ![][c26]       |                 |                       |
  | `sub_sat`                  | ![][c26]       |                 |                       |
  | `mul_sat`                  | ![][c26]       |                 |                       |
  | `div_sat`                  | ![][c26]       |                 |                       |
  | `saturate_cast`            | ![][c26]       |                 |                       |

* `<optional>`

  |                       | Introduced in | Latest standard | Implementation status |
  |-----------------------|---------------|-----------------|-----------------------|
  | `optional`            | ![][c17]      | ![][c23]        |                       |
  | `bad_optional_access` | ![][c17]      |                 |                       |
  | `std::hash<optional>` | ![][c17]      |                 |                       |
  | `nullopt`             | ![][c17]      |                 |                       |
  | `nullopt_t`           | ![][c17]      |                 |                       |
  | `swap(optional)`      | ![][c17]      |                 |                       |
  | `make_optional`       | ![][c17]      |                 |                       |

* `<random>`

  |                                | Introduced in | Latest standard | Implementation status |
  |--------------------------------|---------------|-----------------|-----------------------|
  | `uniform_random_bit_generator` | ![][c20]      |                 |                       |

* `<ranges>`

  |                                                                   | Introduced in | Latest standard | Implementation status |
  |-------------------------------------------------------------------|---------------|-----------------|-----------------------|
  | `ranges::range`                                                   | ![][c20]      |                 |                       |
  | `ranges::borrowed_range`                                          | ![][c20]      |                 |                       |
  | `ranges::sized_range`                                             | ![][c20]      |                 |                       |
  | `ranges::view`                                                    | ![][c20]      |                 |                       |
  | `ranges::input_range`                                             | ![][c20]      |                 |                       |
  | `ranges::output_range`                                            | ![][c20]      |                 |                       |
  | `ranges::forward_range`                                           | ![][c20]      |                 |                       |
  | `ranges::bidirectional_range`                                     | ![][c20]      |                 |                       |
  | `ranges::random_access_range`                                     | ![][c20]      |                 |                       |
  | `ranges::contiguous_range`                                        | ![][c20]      |                 |                       |
  | `ranges::common_range`                                            | ![][c20]      |                 |                       |
  | `ranges::viewable_range`                                          | ![][c20]      |                 |                       |
  | `ranges::constant_range`                                          | ![][c20]      |                 |                       |
  | `ranges::to`                                                      | ![][c23]      |                 |                       |
  | `ranges::iterator_t`                                              | ![][c20]      |                 |                       |
  | `ranges::const_iterator_t`                                        | ![][c23]      |                 |                       |
  | `ranges::sentinel_t`                                              | ![][c20]      |                 |                       |
  | `ranges::const_sentinel_t`                                        | ![][c23]      |                 |                       |
  | `ranges::range_difference_t`                                      | ![][c20]      |                 |                       |
  | `ranges::range_size_t`                                            | ![][c20]      |                 |                       |
  | `ranges::range_value_t`                                           | ![][c20]      |                 |                       |
  | `ranges::range_refernce_t`                                        | ![][c20]      |                 |                       |
  | `ranges::range_const_reference_t`                                 | ![][c20]      |                 |                       |
  | `ranges::range_rvalue_reference_t`                                | ![][c20]      |                 |                       |
  | `ranges::range_common_reference_t`                                | ![][c20]      |                 |                       |
  | `ranges::view_interface`                                          | ![][c20]      |                 |                       |
  | `ranges::subrange`                                                | ![][c20]      |                 |                       |
  | `std::tuple_size<ranges::subrange>`                               | ![][c20]      |                 |                       |
  | `std::tuple_element<ranges::subrange>`                            | ![][c20]      |                 |                       |
  | `std::get(ranges::subrange)`                                      | ![][c20]      |                 |                       |
  | `ranges::dangling`                                                | ![][c20]      |                 |                       |
  | `ranges::borrowed_iterator_t`<br/>`ranges::borrowed_subrange_t`   | ![][c20]      |                 |                       |
  | `ranges::range_adaptor_closure`                                   | ![][c23]      |                 |                       |
  | `ranges::empty_view`<br/>`views::empty`                           | ![][c20]      |                 |                       |
  | `ranges::single_view`<br/>`views::single`                         | ![][c20]      |                 |                       |
  | `ranges::iota_view`<br/>`views::iota`                             | ![][c20]      |                 |                       |
  | `ranges::iota_view`<br/>`views::iota`                             | ![][c20]      |                 |                       |
  | `ranges::basic_istream_view`<br/>`views::istream`                 | ![][c20]      |                 |                       |
  | `ranges::repeat_view`<br/>`views::repeat`                         | ![][c23]      |                 |                       |
  | `ranges::cartesian_product_view`<br/>`views::cartesian_product`   | ![][c23]      |                 |                       |
  | `views::all_t`<br/>`views::all`                                   | ![][c20]      |                 |                       |
  | `ranges::ref_view`                                                | ![][c20]      |                 |                       |
  | `ranges::owning_view`                                             | ![][c20]      |                 |                       |
  | `ranges::filter_view`<br/>`views::filter`                         | ![][c20]      |                 |                       |
  | `ranges::transform_view`<br/>`views::transform`                   | ![][c20]      |                 |                       |
  | `ranges::take_view`<br/>`views::take`                             | ![][c20]      |                 |                       |
  | `ranges::take_while_view`<br/>`views::take_while`                 | ![][c20]      |                 |                       |
  | `ranges::drop_view`<br/>`views::drop`                             | ![][c20]      |                 |                       |
  | `ranges::drop_while_view`<br/>`views::drop_while`                 | ![][c20]      |                 |                       |
  | `ranges::join_view`<br/>`views::join`                             | ![][c20]      |                 |                       |
  | `ranges::lazy_split_view`<br/>`views::lazy_split`                 | ![][c20]      |                 |                       |
  | `ranges::split_view`<br/>`views::split`                           | ![][c20]      |                 |                       |
  | `views::counted`                                                  | ![][c20]      |                 |                       |
  | `ranges::common_view`<br/>`views::common`                         | ![][c20]      |                 |                       |
  | `ranges::reverse_view`<br/>`views::reverse`                       | ![][c20]      |                 |                       |
  | `ranges::as_const_view`<br/>`views::as_const`                     | ![][c23]      |                 |                       |
  | `ranges::as_rvalue_view`<br/>`views::as_rvalue`                   | ![][c23]      |                 |                       |
  | `ranges::elements_view`<br/>`views::elements`                     | ![][c20]      |                 |                       |
  | `ranges::keys_view`<br/>`views::keys`                             | ![][c20]      |                 |                       |
  | `ranges::values_view`<br/>`views::values`                         | ![][c20]      |                 |                       |
  | `ranges::enumerate_view`<br/>`views::enumerate`                   | ![][c23]      |                 |                       |
  | `ranges::zip_view`<br/>`views::zip`                               | ![][c23]      |                 |                       |
  | `ranges::zip_transform_view`<br/>`views::zip_transform`           | ![][c23]      |                 |                       |
  | `ranges::adjacent_view`<br/>`views::adjacent`                     | ![][c23]      |                 |                       |
  | `ranges::adjacent_transform_view`<br/>`views::adjacent_transform` | ![][c23]      |                 |                       |
  | `ranges::join_with_view`<br/>`views::join_with`                   | ![][c23]      |                 |                       |
  | `ranges::stride_view`<br/>`views::stride`                         | ![][c23]      |                 |                       |
  | `ranges::slide_view`<br/>`views::slide`                           | ![][c23]      |                 |                       |
  | `ranges::chunk_view`<br/>`views::chunk`                           | ![][c23]      |                 |                       |
  | `ranges::chunk_by_view`<br/>`views::chunk_by`                     | ![][c23]      |                 |                       |
  | `ranges::concat_view`<br/>`views::concat`                         | ![][c26]      |                 |                       |
  | `ranges::begin`                                                   | ![][c20]      |                 |                       |
  | `ranges::end`                                                     | ![][c20]      |                 |                       |
  | `ranges::cbegin`                                                  | ![][c20]      |                 |                       |
  | `ranges::cend`                                                    | ![][c20]      |                 |                       |
  | `ranges::crbegin`                                                 | ![][c20]      |                 |                       |
  | `ranges::crend`                                                   | ![][c20]      |                 |                       |
  | `ranges::size`                                                    | ![][c20]      |                 |                       |
  | `ranges::ssize`                                                   | ![][c20]      |                 |                       |
  | `ranges::empty`                                                   | ![][c20]      |                 |                       |
  | `ranges::data`                                                    | ![][c20]      |                 |                       |
  | `ranges::cdata`                                                   | ![][c20]      |                 |                       |
  | `ranges::subrange_kind`                                           | ![][c20]      |                 |                       |
  | `ranges::from_range_t`<br/>`ranges::from_range`                   | ![][c20]      |                 |                       |

* `<semaphore>`

  |                      | Introduced in | Latest standard | Implementation status |
  |----------------------|---------------|-----------------|-----------------------|
  | `counting_semaphore` | ![][c20]      |                 |                       |
  | `binary_semaphore`   | ![][c20]      |                 |                       |

* `<stop_token>`

  |                 | Introduced in | Latest standard | Implementation status |
  |-----------------|---------------|-----------------|-----------------------|
  | `stop_token`    | ![][c20]      |                 |                       |
  | `stop_source`   | ![][c20]      |                 |                       |
  | `stop_callback` | ![][c20]      |                 |                       |
  | `nostopstate_t` | ![][c20]      |                 |                       |
  | `nostopstate`   | ![][c20]      |                 |                       |

* `<string>`

  |                               | Introduced in | Latest standard | Implementation status |
  |-------------------------------|---------------|-----------------|-----------------------|
  | `erase(std::basic_string)`    | ![][c20]      | ![][c26]        |                       |
  | `erase_if(std::basic_string)` | ![][c20]      | ![][c26]        |                       |

* `<string_view>`

  |                                | Introduced in | Latest standard | Implementation status |
  |--------------------------------|---------------|-----------------|-----------------------|
  | `basic_string_view`            | ![][c17]      | ![][c20]        |                       |
  | `std::hash<basic_string_view>` | ![][c17]      |                 |                       |
  | `swap(optional)`               | ![][c17]      |                 |                       |
  | `operator""_sv`                | ![][c17]      |                 |                       |

* `<source_location>` N/A

* `<syncstream>`

  |                            | Introduced in | Latest standard | Implementation status |
  |----------------------------|---------------|-----------------|-----------------------|
  | `basic_syncbuf`            | ![][c20]      |                 |                       |
  | `basic_osyncstream`        | ![][c20]      |                 |                       |
  | `std::swap(basic_syncbuf)` | ![][c20]      |                 |                       |

* `<span>`

  |                     | Introduced in | Latest standard | Implementation status |
  |---------------------|---------------|-----------------|-----------------------|
  | `span`              | ![][c20]      | ![][c26]        |                       |
  | `dynamic_extent`    | ![][c20]      |                 |                       |
  | `as_bytes`          | ![][c20]      |                 |                       |
  | `as_writable_bytes` | ![][c20]      |                 |                       |

* `<spanstream>`

  |                     | Introduced in | Latest standard | Implementation status |
  |---------------------|---------------|-----------------|-----------------------|
  | `basic_spanbuf`     | ![][c23]      |                 |                       |
  | `basic_ispanstream` | ![][c23]      |                 |                       |
  | `basic_ospanstream` | ![][c23]      |                 |                       |
  | `basic_spanstream`  | ![][c23]      |                 |                       |

* `<thread>`

  |                   | Introduced in | Latest standard | Implementation status |
  |-------------------|---------------|-----------------|-----------------------|
  | `jthread`         | ![][c20]      |                 |                       |

* `<tuple>`

  |                       | Introduced in | Latest standard | Implementation status |
  |-----------------------|---------------|-----------------|-----------------------|
  | `apply`               | ![][c17]      | ![][c23]        |                       |
  | `make_from_tuple`     | ![][c17]      | ![][c23]        |                       |

* `<type_traits>`

  |                                                 | Introduced in | Latest standard | Implementation status |
  |-------------------------------------------------|---------------|-----------------|-----------------------|
  | `conjunction`                                   | ![][c17]      |                 | ![][c17]              |
  | `bool_constant`                                 | ![][c17]      |                 |                       |
  | `disjunction`                                   | ![][c17]      |                 |                       |
  | `has_unique_object_representations`             | ![][c17]      |                 | N/A                   |
  | `is_aggregate`                                  | ![][c17]      |                 | N/A                   |
  | `is_swappable`                                  | ![][c17]      |                 |                       |
  | `is_invocable`                                  | ![][c17]      |                 |                       |
  | `invoke_result`                                 | ![][c17]      |                 |                       |
  | `negation`                                      | ![][c17]      |                 |                       |
  | `void_t`                                        | ![][c17]      |                 |                       |
  | `common_reference`<br/>`basic_common_reference` | ![][c20]      |                 |                       |
  | `is_bounded_array`                              | ![][c20]      |                 |                       |
  | `is_constant_evaluated`                         | ![][c20]      |                 |                       |
  | `is_corresponding_member`                       | ![][c20]      |                 |                       |
  | `is_layout_compatible`                          | ![][c20]      |                 | N/A                   |
  | `is_nothrow_convertible`                        | ![][c20]      |                 |                       |
  | `is_pointer_interconvertible_base_of`           | ![][c20]      |                 |                       |
  | `is_pointer_interconvertible_with_class`        | ![][c20]      |                 |                       |
  | `is_scoped_enum`                                | ![][c20]      |                 | N/A                   |
  | `is_unbounded_array`                            | ![][c20]      |                 |                       |
  | `remove_cvref`                                  | ![][c20]      |                 |                       |
  | `type_identity`                                 | ![][c20]      |                 |                       |
  | `is_implicit_lifetime`                          | ![][c23]      |                 | N/A                   |
  | `is_within_lifetime`                            | ![][c26]      |                 | N/A                   |

* `<utility>`

  |                             | Introduced in | Latest standard | Implementation status |
  |-----------------------------|---------------|-----------------|-----------------------|
  | `as_const`                  | ![][c17]      |                 |                       |
  | `in_place`<br/>`in_place_t` | ![][c17]      |                 |                       |
  | `cmp_equal`                 | ![][c20]      |                 |                       |
  | `in_range`                  | ![][c20]      |                 |                       |
  | `forward_like`              | ![][c23]      |                 |                       |
  | `to_underlying`             | ![][c23]      |                 |                       |
  | `nontype`<br/>`nontype_t`   | ![][c23]      |                 |                       |
  | `unreachable`               | ![][c23]      |                 | N/A                   |

* `<variant>`

  |                       | Introduced in | Latest standard | Implementation status |
  |-----------------------|---------------|-----------------|-----------------------|
  | `variant`             | ![][c17]      | ![][c26]        |                       |
  | `monostate`           | ![][c17]      |                 |                       |
  | `bad_variant_access`  | ![][c17]      |                 |                       |
  | `variant_size`        | ![][c17]      |                 |                       |
  | `variant_alternative` | ![][c17]      |                 |                       |
  | `std::hash<variant>`  | ![][c17]      |                 |                       |
  | `variant_npos`        | ![][c17]      |                 |                       |
  | `visit`               | ![][c17]      |                 |                       |
  | `std::get(variant)`   | ![][c17]      |                 |                       |
  | `get_if`              | ![][c17]      |                 |                       |
  | `swap(variant)`       | ![][c17]      |                 |                       |

* `<version>`

  |                      | Introduced in | Latest standard | Implementation status |
  |----------------------|---------------|-----------------|-----------------------|
  |                      |               |                 |                       |

* Others

  |                       | Introduced in | Latest standard | Implementation status |
  |-----------------------|---------------|-----------------|-----------------------|
  | `size(C)`             | ![][c17]      | ![][c20]        |                       |
  | `empty(C)`            | ![][c17]      | ![][c20]        |                       |
  | `data(C)`             | ![][c17]      |                 |                       |

[c17]: https://img.shields.io/badge/C++17-0055AA
[c20]: https://img.shields.io/badge/C++20-4477BB
[c23]: https://img.shields.io/badge/C++23-skyblue
[c26]: https://img.shields.io/badge/C++26-77EEFF
