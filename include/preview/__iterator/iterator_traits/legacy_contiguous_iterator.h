//
// Created by yonggyulee on 2024/07/17.
//

#ifndef PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_CONTIGUOUS_ITERATOR_H_
#define PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_CONTIGUOUS_ITERATOR_H_

#include <cstddef>
#include <type_traits>

#include "preview/__concepts/dereferenceable.h"
#include "preview/__iterator/iterator_traits/legacy_iterator.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename I, typename D, typename = void>
struct iter_addable_dereferenceable : std::false_type {};
template<typename I, typename D>
struct iter_addable_dereferenceable<I, D, void_t<decltype(std::declval<I&>() + std::declval<D>())>>
    : dereferenceable<decltype(std::declval<I&>() + std::declval<D>())> {};

template<typename I, typename D,
         typename Ptr = std::add_pointer_t<std::remove_reference_t<decltype(*std::declval<I&>())>>, // std::addressof(*I)
         bool = /* false */ conjunction<
             iter_addable_dereferenceable<I, D>,
             iter_addable_dereferenceable<Ptr, std::ptrdiff_t>
         >::value>
struct LegacyContiguousIteratorImpl_ForDiff : std::false_type {};

template<typename I, typename D, typename Ptr>
struct LegacyContiguousIteratorImpl_ForDiff<I, D, Ptr, true>
    : std::is_same<
        decltype( *(std::declval<I&>()  + std::declval<D>()) ),
        decltype( *(std::declval<Ptr>() + std::declval<std::ptrdiff_t>()) )
    > {};

// std::addressof must be callable
template<typename I, bool = /* true */ std::is_rvalue_reference<decltype(*std::declval<I&>())>::value>
struct LegacyContiguousIteratorImpl_2 : std::false_type {};

template<typename I>
struct LegacyContiguousIteratorImpl_2<I, false>
    : LegacyContiguousIteratorImpl_ForDiff<I, iter_difference_t<I>> {};

template<typename I, bool = /* false */ LegacyIterator<I>::value>
struct LegacyContiguousIteratorImpl_1 : std::false_type {};

template<typename I>
struct LegacyContiguousIteratorImpl_1<I, true>
    : LegacyContiguousIteratorImpl_2<I> {};

} // namespace detail

template<typename I>
struct LegacyContiguousIterator : detail::LegacyContiguousIteratorImpl_1<I> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_CONTIGUOUS_ITERATOR_H_
