//
// Created by yonggyulee on 23/04/2025
//
// Algorithm 26.11.4 uninitialized_value_construct (uninitialized.construct.value)
// https://eel.is/c++draft/algorithms#uninitialized.construct.value

#ifndef PREVIEW_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_H_
#define PREVIEW_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_H_

#include "preview/__concepts/default_initializable.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__memory/detail/voidify.h"
#include "preview/__memory/detail/special_memory_concepts.h"
#include "preview/__ranges/borrowed_iterator_t.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/end.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__type_traits/conjunction.h"

#if !PREVIEW_CONFORM_CXX20_STANDARD
#include "preview/__utility/cxx20_rel_ops.h"
#endif

namespace preview {

template<typename NoThrowForwardIterator>
constexpr void uninitialized_value_construct(NoThrowForwardIterator first, NoThrowForwardIterator last) {
#if !PREVIEW_CONFORM_CXX20_STANDARD
  using namespace preview::rel_ops;
#endif
  for (; first != last; ++first) {
    ::new (preview::detail::voidify(*first))
        typename iterator_traits<NoThrowForwardIterator>::value_type();
  }
}

namespace ranges {
namespace detail {

struct uninitialized_value_construct_niebloid {
  template<typename I, typename S, std::enable_if_t<conjunction_v<
      nothrow_forward_iterator<I>,
      nothrow_sentinel_for<S, I>,
      default_initializable<iter_value_t<I>>
  >, int> = 0>
  constexpr I operator()(I first, S last) const {
#if !PREVIEW_CONFORM_CXX20_STANDARD
    using namespace preview::rel_ops;
#endif
    for (; first != last; ++first)
      ::new (preview::detail::voidify(*first)) std::remove_reference_t<iter_reference_t<I>>();
    return first;
  }

  template<typename R, std::enable_if_t<conjunction_v<
      nothrow_forward_range<R>,
      default_initializable<range_value_t<R>>
  >, int> = 0>
  constexpr borrowed_iterator_t<R> operator()(R&& r) const {
    return (*this)(ranges::begin(r), ranges::end(r));
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::uninitialized_value_construct_niebloid uninitialized_value_construct{};

} // namespace ranges
} // namespace preview 

#endif // PREVIEW_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_H_
