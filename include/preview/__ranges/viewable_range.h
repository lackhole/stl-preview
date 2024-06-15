//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_RANGES_VIEWABLE_RANGE_H_
#define PREVIEW_RANGES_VIEWABLE_RANGE_H_

#include <type_traits>

#include "preview/__concepts/constructible_from.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/view.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_initializer_list.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, bool = range<T>::value /* false */>
struct viewable_range_impl : std::false_type {};

template<typename T>
struct viewable_range_impl<T, true>
    : disjunction<
          conjunction<
              view<remove_cvref_t<T>>,
              constructible_from<remove_cvref_t<T>, T> >,
          conjunction<
              negation< view<remove_cvref_t<T>> >,
              disjunction<
                  std::is_lvalue_reference<T>,
                  conjunction<
                      movable<std::remove_reference_t<T>>,
                      negation< is_initializer_list<remove_cvref_t<T>> >
                  >
              >
          >
      >{};

} // namespace detail

template<typename T>
struct viewable_range : detail::viewable_range_impl<T> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWABLE_RANGE_H_
