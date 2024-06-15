//
// Created by yonggyulee on 2024/01/13.
//

#ifndef PREVIEW_RANGES_RANGE_MAPPED_T_H_
#define PREVIEW_RANGES_RANGE_MAPPED_T_H_

#include <tuple>
#include <type_traits>

#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, typename = void>
struct has_second_type : std::false_type {};
template<typename T>
struct has_second_type<T, void_t<typename T::second_type>> : std::true_type {};

template<typename T, typename = void>
struct has_second_tuple_element : std::false_type {};
template<typename T>
struct has_second_tuple_element<T, void_t<std::tuple_element_t<1, T>>> : std::true_type {};

template<typename RV, bool = has_second_type<RV>::value, bool = has_second_tuple_element<RV>::value>
struct guide_map {};
template<typename RV, bool v>
struct guide_map<RV, true, v> {
  using type = typename RV::second_type;
};
template<typename RV>
struct guide_map<RV, false, true> {
  using type = std::tuple_element_t<1, RV>;
};

template<typename R, bool = input_range<R>::value /* false */>
struct range_mapped_impl {};
template<typename R>
struct range_mapped_impl<R, true> : guide_map<range_value_t<R>> {};

} // namespace ranges

template<typename R>
struct range_mapped : detail::range_mapped_impl<R> {};

template<typename R>
using range_mapped_t = typename range_mapped<R>::type;

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_RANGE_MAPPED_T_H_
