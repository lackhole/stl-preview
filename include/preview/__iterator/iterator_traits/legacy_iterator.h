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
#include "preview/__type_traits/is_post_incrementable.h"
#include "preview/__type_traits/is_pre_incrementable.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace detail {

template<typename I, bool = is_post_incrementable<I, dereferenceable>::value>
struct LegacyIteratorCheckPostIncrement : std::false_type {};
template<typename I>
struct LegacyIteratorCheckPostIncrement<I, true> : is_referencable<decltype(*std::declval<I&>()++)> {};

} // namespace detail

template<typename I, bool = dereferenceable<I>::value>
struct LegacyIterator : std::false_type {};

template<typename I>
struct LegacyIterator<I, true>
    : conjunction<
        is_referencable<decltype(*std::declval<I&>())>,
        is_pre_incrementable<I>,
        detail::LegacyIteratorCheckPostIncrement<I>,
        copyable<I>
> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_LEGACY_ITERATOR_H_
