//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_SIZED_SENTINEL_FOR_H_
#define PREVIEW_ITERATOR_SIZED_SENTINEL_FOR_H_

#include <type_traits>

#include "preview/__concepts/requires_expression.h"
#include "preview/__concepts/same_as.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"

namespace preview {

template<typename S, typename I>
struct disable_sized_sentinel_for : std::false_type {};

namespace detail {

template<typename S, typename I, typename = void, typename = void>
struct explicit_sized_sentinel_subtract_check : std::false_type {};

template<typename S, typename I>
struct explicit_sized_sentinel_subtract_check<
    S, I,
    void_t<decltype(std::declval<S>() - std::declval<I>())>,
    void_t<decltype(std::declval<I>() - std::declval<S>())>
  > : conjunction<
        same_as<decltype(std::declval<S>() - std::declval<I>()), iter_difference_t<I>>,
        same_as<decltype(std::declval<I>() - std::declval<S>()), iter_difference_t<I>>
      > {};

template<typename S, typename I>
struct sized_sentinel_requires : requires_expression<explicit_sized_sentinel_subtract_check, const S&, const I&> {};

template<
    typename S,
    typename I,
    bool = conjunction<
          sentinel_for<S, I>,
          negation<disable_sized_sentinel_for<std::remove_cv_t<S>, std::remove_cv_t<I>>>,
          has_typename_type<iter_difference<I>>
        >::value
>
struct sized_sentinel_for_impl : std::false_type {};

template<typename S, typename I>
struct sized_sentinel_for_impl<S, I, true> : sized_sentinel_requires<S, I> {};

} // namespace detail

template<typename S, typename I>
struct sized_sentinel_for : detail::sized_sentinel_for_impl<S, I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_SIZED_SENTINEL_FOR_H_
