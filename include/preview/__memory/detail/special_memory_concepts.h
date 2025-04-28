//
// Created by yonggyulee on 23/04/2025
//
// 26.11.2 Special memory concepts (special.memory.concepts)
// https://eel.is/c++draft/algorithms#special.mem.concepts

#ifndef PREVIEW_MEMORY_DETAIL_SPECIAL_MEMORY_CONCEPTS_H_
#define PREVIEW_MEMORY_DETAIL_SPECIAL_MEMORY_CONCEPTS_H_

#include <type_traits>

#include "preview/__iterator/forward_iterator.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__concepts/same_as.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__ranges/range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename I, bool = /* false */ input_iterator<I>::value>
struct nothrow_input_iterator : std::false_type {};
template<typename I>
struct nothrow_input_iterator<I, true> : conjunction<
    std::is_lvalue_reference<iter_reference_t<I>>,
    same_as<remove_cvref_t<iter_reference_t<I>>, iter_value_t<I>>
> {};

template<typename S, typename I>
struct nothrow_sentinel_for : sentinel_for<S, I> {};

template<typename R, bool = /* false */ ranges::range<R>::value>
struct nothrow_input_range : std::false_type {};
template<typename R>
struct nothrow_input_range<R, true> : conjunction<
    nothrow_input_iterator<ranges::iterator_t<R>>,
    nothrow_sentinel_for<ranges::sentinel_t<R>, ranges::iterator_t<R>>
> {};

template<typename I>
struct nothrow_forward_iterator : conjunction<
    nothrow_input_iterator<I>,
    forward_iterator<I>,
    nothrow_sentinel_for<I, I>
> {};

template<typename R, bool = /* false */ nothrow_input_range<R>::value>
struct nothrow_forward_range : std::false_type {};
template<typename R>
struct nothrow_forward_range<R, true> : nothrow_forward_iterator<ranges::iterator_t<R>> {};

} // namespace ranges
} // namespace detail
} // namespace preview

#endif // PREVIEW_MEMORY_DETAIL_SPECIAL_MEMORY_CONCEPTS_H_
