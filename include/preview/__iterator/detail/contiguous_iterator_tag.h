//
// Created by yonggyulee on 2024. 8. 13.
//

#ifndef PREVIEW_ITERATOR_DETAIL_HAVE_CONTIGUOUS_ITERATOR_TAG_H_
#define PREVIEW_ITERATOR_DETAIL_HAVE_CONTIGUOUS_ITERATOR_TAG_H_

#include <iterator>
#include <type_traits>

#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

struct pseudo_contiguous_iterator_tag : std::random_access_iterator_tag {};

template<typename T, typename = void>
struct has_typename_iterator_concept : std::false_type {};
template<typename T>
struct has_typename_iterator_concept<T, void_t<typename T::iterator_concept>> : std::true_type {};

template<typename IteratorTraits = std::iterator_traits<int*>,
         bool = has_typename_iterator_concept<IteratorTraits>::value /* false */>
struct contiguous_iterator_tag_helper {
  static_assert(std::is_same<IteratorTraits, std::iterator_traits<int*>>::value, "Invalid param");
  using type = pseudo_contiguous_iterator_tag;
};
template<typename IteratorTraits>
struct contiguous_iterator_tag_helper<IteratorTraits, true> {
  static_assert(std::is_same<IteratorTraits, std::iterator_traits<int*>>::value, "Invalid param");
  using type = typename IteratorTraits::iterator_concept;
};

} // namespace detail

using contiguous_iterator_tag = detail::contiguous_iterator_tag_helper<>::type;

// Evaluates to `true` if `std::contiguous_iterator_tag` exists, `false` otherwise
struct have_std_contiguous_iterator_tag
    : negation<std::is_same<
        contiguous_iterator_tag,
        detail::pseudo_contiguous_iterator_tag
    >> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_DETAIL_HAVE_CONTIGUOUS_ITERATOR_TAG_H_
