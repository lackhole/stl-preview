//
// Created by yonggyulee on 23/04/2025
//
// Algorithm 26.11.5 uninitialized_copy (uninitialized.copy)
// https://eel.is/c++draft/algorithms#uninitialized.copy

#ifndef PREVIEW_MEMORY_UNINITIALIZED_COPY_N_H_
#define PREVIEW_MEMORY_UNINITIALIZED_COPY_N_H_

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__concepts/constructible_from.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/counted_iterator.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__memory/detail/voidify.h"
#include "preview/__memory/detail/special_memory_concepts.h"
#include "preview/__memory/uninitialized_copy.h"
#include "preview/__type_traits/conjunction.h"

#if !PREVIEW_CONFORM_CXX20_STANDARD
#include "preview/__utility/cxx20_rel_ops.h"
#endif

namespace preview {

template<typename InputIterator, typename Size, typename NoThrowForwardIterator>
constexpr NoThrowForwardIterator uninitialized_copy_n(InputIterator first, Size n, NoThrowForwardIterator result)
    // Preconditions: result + [0, n) does not overlap with first + [0, n).
{
#if !PREVIEW_CONFORM_CXX20_STANDARD
  using namespace preview::rel_ops;
#endif
  for (; n > 0; ++result, (void)++first, --n) {
    ::new (preview::detail::voidify(*result))
        typename iterator_traits<NoThrowForwardIterator>::value_type(*first);
  }
  return result;
}

namespace ranges {

template<typename I, typename O>
using uninitialized_copy_n_result = in_out_result<I, O>;

namespace detail {

struct uninitialized_copy_n_niebloid {
  template<typename I, typename O, typename S, std::enable_if_t<conjunction_v<
      input_iterator<I>,
      nothrow_forward_iterator<O>,
      nothrow_sentinel_for<S, O>,
      constructible_from<iter_value_t<O>, iter_reference_t<I>>
  >, int> = 0>
  constexpr uninitialized_copy_n_result<I, O> operator()(I ifirst, iter_difference_t<I> n, O ofirst, S olast) const {
    auto t = preview::uninitialized_copy(counted_iterator<I>{std::move(ifirst), n}, default_sentinel, ofirst, olast);
    return {std::move(t.in).base(), t.out};
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::uninitialized_copy_n_niebloid uninitialized_copy_n{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_MEMORY_UNINITIALIZED_COPY_N_H_
