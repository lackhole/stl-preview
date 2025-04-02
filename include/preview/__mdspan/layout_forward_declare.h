//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_LAYOUT_FORWARD_DECLARE_H_
#define PREVIEW_MDSPAN_LAYOUT_FORWARD_DECLARE_H_

#include <cstddef>
#include <type_traits>

#include "preview/config.h"
#include "preview/__concepts/same_as.h"
#include "preview/__mdspan/extents.h"
#include "preview/__span/dynamic_extent.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

struct layout_mapping_ctor_tag {};

template<typename Layout, typename Mapping>
struct is_mapping_of : std::is_same<typename Layout::template mapping<typename Mapping::extents_type>, Mapping> {};

template<typename Mapping, typename = void>
struct is_layout_left_padded_mapping_of : std::false_type {};

template<typename Mapping, typename = void>
struct is_layout_right_padded_mapping_of : std::false_type {};

// TODO: Make concept

struct constexpr_is_always_strided_tester {
  template<typename M>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype( M::is_always_strided() ), M::is_always_strided()>{}, std::true_type{});
  template<typename M>
  static constexpr auto test(...) -> std::false_type;
};

template<typename M>
struct has_constexpr_static_fn_is_always_strided_impl
    : decltype(constexpr_is_always_strided_tester::test<std::remove_reference_t<M>>(0)) {};

template<typename M, bool = /* false */ has_constexpr_static_fn_is_always_strided_impl<M>::value>
struct has_constexpr_static_fn_is_always_strided : std::false_type {};
template<typename M>
struct has_constexpr_static_fn_is_always_strided<M, true> : same_as<decltype( M::is_always_strided() ), bool> {};

struct constexpr_is_always_exhaustive_tester {
  template<typename M>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype( M::is_always_exhaustive() ), M::is_always_exhaustive()>{}, std::true_type{});
  template<typename M>
  static constexpr auto test(...) -> std::false_type;
};

template<typename M>
struct has_constexpr_static_fn_is_always_exhaustive_impl
    : decltype(constexpr_is_always_exhaustive_tester::test<std::remove_reference_t<M>>(0)) {};

template<typename M, bool = /* false */ has_constexpr_static_fn_is_always_exhaustive_impl<M>::value>
struct has_constexpr_static_fn_is_always_exhaustive : std::false_type {};
template<typename M>
struct has_constexpr_static_fn_is_always_exhaustive<M, true> : same_as<decltype( M::is_always_exhaustive() ), bool> {};

struct constexpr_is_always_unique_tester {
  template<typename M>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype( M::is_always_unique() ), M::is_always_unique()>{}, std::true_type{});
  template<typename M>
  static constexpr auto test(...) -> std::false_type;
};

template<typename M>
struct has_constexpr_static_fn_is_always_unique_impl
    : decltype(constexpr_is_always_unique_tester::test<std::remove_reference_t<M>>(0)) {};

template<typename M, bool = /* false */ has_constexpr_static_fn_is_always_unique_impl<M>::value>
struct has_constexpr_static_fn_is_always_unique : std::false_type {};
template<typename M>
struct has_constexpr_static_fn_is_always_unique<M, true> : same_as<decltype( M::is_always_unique() ), bool> {};

template<typename M, typename = void>
struct has_typename_extents_type : std::false_type {};
template<typename M>
struct has_typename_extents_type<M, void_t<typename M::extents_type>> : std::true_type {};

template<typename M, bool = /* false */ conjunction<
    has_typename_extents_type<M>,
    has_constexpr_static_fn_is_always_strided<M>,
    has_constexpr_static_fn_is_always_exhaustive<M>,
    has_constexpr_static_fn_is_always_unique<M>
>::value>
struct layout_mapping_alike : std::false_type {};

template<typename M>
struct layout_mapping_alike<M, true>
    : conjunction<
        is_extents<typename M::extents_type>
//        bool_constant<M::is_always_strided()>,
//        bool_constant<M::is_always_exhaustive()>,
//        bool_constant<M::is_always_unique()>
    > {};

} // namespace detail

struct layout_left {
  template<typename Extents>
  class mapping;
};

struct layout_right {
  template<typename Extents>
  class mapping;
};

struct layout_stride {
  template<typename Extents>
  class mapping;
};

template<std::size_t PaddingValue = dynamic_extent>
struct layout_left_padded {
  template<typename Extents>
  class mapping;
};

template<std::size_t PaddingValue = dynamic_extent>
struct layout_right_padded {
  template<typename Extents>
  class mapping;
};

namespace detail {

template<typename T>
struct is_layout_left_padded : std::false_type {};
template<std::size_t S>
struct is_layout_left_padded<layout_left_padded<S>> : std::true_type {};

template<typename Mapping>
struct is_layout_left_padded_mapping_of<Mapping, void_t<typename Mapping::layout_type>>
    : is_layout_left_padded<typename Mapping::layout_type> {};

template<typename T>
struct is_layout_right_padded : std::false_type {};
template<std::size_t S>
struct is_layout_right_padded<layout_right_padded<S>> : std::true_type {};

template<typename Mapping>
struct is_layout_right_padded_mapping_of<Mapping, void_t<typename Mapping::layout_type>>
    : is_layout_right_padded<typename Mapping::layout_type> {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_FORWARD_DECLARE_H_
