//
// Created by yonggyulee on 23/04/2025
//
// Algorithm 26.11.4 uninitialized_value_construct (uninitialized.construct.value)
// https://eel.is/c++draft/algorithms#uninitialized.construct.value

#ifndef PREVIEW_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_N_H_
#define PREVIEW_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_N_H_

#include "preview/__concepts/default_initializable.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/counted_iterator.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__memory/detail/voidify.h"
#include "preview/__memory/detail/special_memory_concepts.h"
#include "preview/__memory/uninitialized_value_construct.h"
#include "preview/__type_traits/conjunction.h"

#if !PREVIEW_CONFORM_CXX20_STANDARD
#include "preview/__utility/cxx20_rel_ops.h"
#endif

namespace preview {

template<typename NoThrowForwardIterator, typename Size>
constexpr NoThrowForwardIterator uninitialized_value_construct_n(NoThrowForwardIterator first, Size n) {
#if !PREVIEW_CONFORM_CXX20_STANDARD
  using namespace preview::rel_ops;
#endif
  for (; n > 0; (void)++first, --n) {
    ::new (preview::detail::voidify(*first))
        typename iterator_traits<NoThrowForwardIterator>::value_type();
  }
  return first;
}

namespace ranges {
namespace detail {

struct uninitialized_value_construct_n_niebloid {
  template<typename I, std::enable_if_t<conjunction_v<
      nothrow_forward_iterator<I>,
      default_initializable<iter_value_t<I>>
  >, int> = 0>
  constexpr I operator()(I first, iter_difference_t<I> n) const {
    return preview::uninitialized_value_construct(counted_iterator<I>{first, n}, default_sentinel).base();
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::uninitialized_value_construct_n_niebloid uninitialized_value_construct_n{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_N_H_
