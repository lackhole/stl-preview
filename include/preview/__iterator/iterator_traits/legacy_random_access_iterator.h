//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_RANDOM_ACCESS_ITERATOR_H_
#define PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_RANDOM_ACCESS_ITERATOR_H_

#include <type_traits>

#include "preview/__concepts/convertible_to.h"
#include "preview/__concepts/same_as.h"
#include "preview/__concepts/totally_ordered.h"
#include "preview/__iterator/iterator_traits/legacy_bidirectional_iterator.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_addible.h"
#include "preview/__type_traits/is_add_assignable.h"
#include "preview/__type_traits/is_subscriptable.h"
#include "preview/__type_traits/is_subtractable.h"
#include "preview/__type_traits/is_subtract_assignable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename I, typename D, bool = is_referencable<D>::value>
struct LegacyRandomAccessIteratorImpl2 : std::false_type {};

template<typename I, typename D>
struct LegacyRandomAccessIteratorImpl2<I, D, true>
  : conjunction<
      is_add_assignable<I&, D&, same_as, I&>,
      is_subtract_assignable<I&, D&, same_as, I&>,
      is_addible<I&, D&, same_as, I>,
      is_addible<D&, I&, same_as, I>,
      is_subtractable<I&, D&, same_as, I>,
      is_subtractable<I&, I&, same_as, D>,
      is_subscriptable<I&, D&, convertible_to, iter_reference_t<I>>
  > {};

template<typename I, bool = conjunction<LegacyBidirectionalIterator<I>, totally_ordered<I>>::value>
struct LegacyRandomAccessIteratorImpl1 : std::false_type {};

template<typename I>
struct LegacyRandomAccessIteratorImpl1<I, true>
    : LegacyRandomAccessIteratorImpl2<I, typename incrementable_traits<I>::difference_type> {};

} // namespace detail

template<typename I>
struct LegacyRandomAccessIterator : detail::LegacyRandomAccessIteratorImpl1<I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_RANDOM_ACCESS_ITERATOR_H_
