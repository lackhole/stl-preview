//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_ITERATOR_H_
#define PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_ITERATOR_H_

#include <type_traits>

#include "preview/__concepts/copyable.h"
#include "preview/__concepts/dereferenceable.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace detail {

template<typename I, bool = dereferenceable<decltype(std::declval<I&>()++)>::value>
struct LegacyIteratorCheckPostIncrement_2 : std::false_type {};
template<typename I>
struct LegacyIteratorCheckPostIncrement_2<I, true> : is_referencable<decltype(*std::declval<I&>()++)> {};

template<typename I, bool = preview::detail::is_post_incrementable<I>::value>
struct LegacyIteratorCheckPostIncrement : std::false_type {};
template<typename I>
struct LegacyIteratorCheckPostIncrement<I, true> : LegacyIteratorCheckPostIncrement_2<I> {};

} // namespace detail

template<typename I, bool = dereferenceable<I>::value>
struct LegacyIterator : std::false_type {};

template<typename I>
struct LegacyIterator<I, true>
    : conjunction<
        is_referencable<decltype(*std::declval<I&>())>,
        detail::is_pre_incrementable<I>,
        detail::LegacyIteratorCheckPostIncrement<I>,
        copyable<I>
> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_ITERATOR_H_
