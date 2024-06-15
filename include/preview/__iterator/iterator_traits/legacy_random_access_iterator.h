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
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename I, typename D, typename = void>
struct is_iter_add_assignmentable : std::false_type {};
template<typename I, typename D>
struct is_iter_add_assignmentable<I, D, void_t<decltype(std::declval<I&>() += std::declval<D&>())>>
    : same_as<decltype(std::declval<I&>() += std::declval<D&>()), I&> {};

template<typename I, typename D, typename = void>
struct is_iter_sub_assignmentable : std::false_type {};
template<typename I, typename D>
struct is_iter_sub_assignmentable<I, D, void_t<decltype(std::declval<I&>() -= std::declval<D&>())>>
    : same_as<decltype(std::declval<I&>() -= std::declval<D&>()), I&> {};

template<typename I, typename D, typename = void, typename = void>
struct is_iter_addable : std::false_type {};
template<typename I, typename D>
struct is_iter_addable<I, D,
                       void_t<decltype(std::declval<I&>() + std::declval<D&>())>,
                       void_t<decltype(std::declval<D&>() + std::declval<I&>())>>
    : conjunction<
        same_as<decltype(std::declval<I&>() + std::declval<D&>()), I>,
        same_as<decltype(std::declval<D&>() + std::declval<I&>()), I>
      >{};

template<typename I, typename D, typename = void, typename = void>
struct is_iter_subtractable : std::false_type {};
template<typename I, typename D>
struct is_iter_subtractable<I, D,
                            void_t<decltype(std::declval<I&>() - std::declval<D&>())>,
                            void_t<decltype(std::declval<I&>() - std::declval<I&>())>>
    : conjunction<
        same_as<decltype(std::declval<I&>() - std::declval<D&>()), I>,
        same_as<decltype(std::declval<I&>() - std::declval<I&>()), D>
      > {};

template<typename I, typename D, typename = void>
struct is_iter_subscriptable : std::false_type {};

template<typename I, typename D>
struct is_iter_subscriptable<I, D, void_t<decltype( std::declval<I&>()[std::declval<D&>()] )>>
    : convertible_to<decltype( std::declval<I&>()[std::declval<D&>()] ), iter_reference_t<I>> {};

template<typename I, typename D>
struct LegacyRandomAccessIteratorStage2
    : conjunction<
        is_iter_add_assignmentable<I, D>,
        is_iter_sub_assignmentable<I, D>,
        is_iter_addable<I, D>,
        is_iter_subtractable<I, D>,
        is_iter_subscriptable<I, D>
      > {};

template<typename I, typename D, bool = is_referencable<D>::value>
struct LegacyRandomAccessIteratorStage1 : std::false_type {};

template<typename I, typename D>
struct LegacyRandomAccessIteratorStage1<I, D, true> : LegacyRandomAccessIteratorStage2<I, D> {};

} // namespace detail

template<typename I, bool = conjunction<LegacyBidirectionalIterator<I>, totally_ordered<I>>::value>
struct LegacyRandomAccessIterator : std::false_type {};

template<typename I>
struct LegacyRandomAccessIterator<I, true>
    : detail::LegacyRandomAccessIteratorStage1<I, typename incrementable_traits<I>::difference_type> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_RANDOM_ACCESS_ITERATOR_H_
