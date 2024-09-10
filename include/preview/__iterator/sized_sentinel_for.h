//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_SIZED_SENTINEL_FOR_H_
#define PREVIEW_ITERATOR_SIZED_SENTINEL_FOR_H_

#include <type_traits>

#include "preview/__concepts/same_as.h"
#include "preview/__concepts/subtractable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_subtractable.h"
#include "preview/__type_traits/negation.h"

namespace preview {

template<typename S, typename I>
struct disable_sized_sentinel_for : std::false_type {};

namespace detail {

template<typename S, typename I, bool = conjunction<
    sentinel_for<S, I>,
    negation<disable_sized_sentinel_for<std::remove_cv_t<S>, std::remove_cv_t<I>>>,
    has_typename_type<iter_difference<I>>
>::value>
struct sized_sentinel_for_impl : std::false_type {};

template<typename S, typename I>
struct sized_sentinel_for_impl<S, I, true>
  : conjunction<
      is_subtractable<const S&, const I&, same_as, iter_difference_t<I>>,
      is_subtractable<const I&, const S&, same_as, iter_difference_t<I>>
  > {};

} // namespace detail

template<typename S, typename I>
struct sized_sentinel_for : detail::sized_sentinel_for_impl<S, I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_SIZED_SENTINEL_FOR_H_
