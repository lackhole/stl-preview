//
// Created by cosge on 2023-12-29.
//

#ifndef PREVIEW_ITERATOR_PROJECTED_H_
#define PREVIEW_ITERATOR_PROJECTED_H_

#include "preview/__core/inline_variable.h"
#include "preview/__iterator/indirect_result_t.h"
#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/indirectly_regular_unary_invocable.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/meta.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/type_sequence.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-internal"

#elif defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 5046)
#endif

namespace preview {
namespace detail {

template<typename I, bool = weakly_incrementable<I>::value /* true */>
struct projected_impl_difference_type {
  using difference_type = iter_difference_t<I>;
};
template<typename I>
struct projected_impl_difference_type<I, false> {};

template<
    typename I,
    typename Proj,
    bool = conjunction<
               indirectly_readable<I>,
               indirectly_regular_unary_invocable<Proj, I>
           >::value /* true */
>
struct projected_impl {
  struct type : projected_impl_difference_type<I> {
    using value_type = remove_cvref_t<indirect_result_t<Proj&, I>>;

    indirect_result_t<Proj&, I> operator*() const;
  };
};

template<typename I, typename Proj>
struct projected_impl<I, Proj, false> {};

template<bool B /* false */ , typename I, typename Proj, template<typename, typename...> class Constraint, typename... Args>
struct projectable_and_impl : std::false_type {};

template<typename I, typename Proj, template<typename, typename...> class Constraint, typename... Args>
struct projectable_and_impl<true, I, Proj, Constraint, Args...>
    : Constraint<typename projected_impl<I, Proj>::type, Args...> {};

} // namespace detail

template<typename I, typename Proj>
using projected = typename detail::projected_impl<I, Proj>::type;

// Test if the expression `preview::projected<I, Proj>` is valid in an unqualified context (required until C++20)
template<typename I, typename Proj>
using projectable = has_typename_type<detail::projected_impl<I, Proj>>;
template<typename I, typename Proj>
PREVIEW_INLINE_VARIABLE constexpr bool projectable_v = projectable<I, Proj>::value;

template<typename I, typename Proj, template<typename, typename...> class Constraint, typename... Args>
using projectable_and = detail::projectable_and_impl<projectable<I, Proj>::value, I, Proj, Constraint, Args...>;
template<typename I, typename Proj, template<typename, typename...> class Constraint, typename... Args>
PREVIEW_INLINE_VARIABLE constexpr bool projectable_and_v = projectable_and<I, Proj, Constraint, Args...>::value;

} // namespace preview

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif // PREVIEW_ITERATOR_PROJECTED_H_
