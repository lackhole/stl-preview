//
// Created by yonggyulee on 23/04/2025
//
// Algorithm 26.11.6 uninitialized_move (uninitialized.move)
// https://eel.is/c++draft/algorithms#uninitialized.move

#ifndef PREVIEW_MEMORY_UNINITIALIZED_MOVE_H_
#define PREVIEW_MEMORY_UNINITIALIZED_MOVE_H_

#include <utility>

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/sentinel_for.h"
#include "preview/__memory/detail/deref_move.h"
#include "preview/__memory/detail/special_memory_concepts.h"
#include "preview/__memory/detail/voidify.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__type_traits/conjunction.h"

#if !PREVIEW_CONFORM_CXX20_STANDARD
#include "preview/__utility/cxx20_rel_ops.h"
#endif

namespace preview {

template<typename InputIterator, typename NoThrowForwardIterator>
constexpr void uninitialized_move(InputIterator first, InputIterator last, NoThrowForwardIterator result)
    // Preconditions: result + [0, (last - first)) does not overlap with [first, last).
{
#if !PREVIEW_CONFORM_CXX20_STANDARD
  using namespace preview::rel_ops;
#endif
  for (; first != last; (void)++result, ++first) {
    ::new (preview::detail::voidify(*result))
        typename iterator_traits<NoThrowForwardIterator>::value_type(preview::detail::deref_move(first));
  }
}

namespace ranges {

template<typename I, typename O>
using uninitialized_move_result = in_out_result<I, O>;

namespace detail {

struct uninitialized_move_niebloid {
  template<typename I, typename S, typename O, typename S2, std::enable_if_t<conjunction_v<
      input_iterator<I>,
      sentinel_for<S, I>,
      nothrow_forward_iterator<O>,
      nothrow_sentinel_for<S2, O>,
      constructible_from<iter_value_t<O>, iter_reference_t<I>>
  >, int> = 0>
  constexpr uninitialized_move_result<I, O> operator()(I ifirst, S ilast, O ofirst, S2 olast) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    for (; ifirst != ilast && ofirst != olast; ++ofirst, (void)++ifirst)
      ::new (preview::detail::voidify(*ofirst)) std::remove_reference_t<iter_reference_t<O>>(*ifirst);
    return {std::move(ifirst), ofirst};
  }

  template<typename IR, typename OR, std::enable_if_t<conjunction_v<
      input_range<IR>,
      nothrow_forward_range<OR>,
      constructible_from<range_value_t<OR>, range_reference_t<IR>>
  >, int> = 0>
  constexpr uninitialized_move_result<borrowed_iterator_t<IR>, borrowed_iterator_t<OR>>
  operator()(IR&& in_range, OR&& out_range) const {
    return (*this)(ranges::begin(in_range), ranges::end(out_range), ranges::begin(out_range), ranges::end(out_range));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::uninitialized_move_niebloid uninitialized_move{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_MEMORY_UNINITIALIZED_MOVE_H_
