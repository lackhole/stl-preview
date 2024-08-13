//
// Created by yonggyulee on 8/13/24.
//

#ifndef PREVIEW_ALGORITHM_RANGES_COPY_N_H_
#define PREVIEW_ALGORITHM_RANGES_COPY_N_H_

#include <cstring>
#include <type_traits>
#include <utility>

#include "preview/__algorithm/ranges/in_out_result.h"
#include "preview/__core/inline_variable.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__iterator/detail/contiguous_iterator_tag.h"
#include "preview/__iterator/indirectly_copyable.h"
#include "preview/__iterator/input_iterator.h"
#include "preview/__iterator/iter_difference_t.h"
#include "preview/__iterator/weakly_incrementable.h"
#include "preview/__memory/addressof.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {

template< class I, class O >
using copy_n_result = ranges::in_out_result<I, O>;

namespace detail {

struct copy_n_niebloid {
 private:
  template<typename T>
  struct is_pointer_to_trivial : std::false_type {};
  template<typename T>
  struct is_pointer_to_trivial<T*> : std::is_trivial<T> {};

  template<typename I, typename O>
  constexpr copy_n_result<I, O> call(I first, iter_difference_t<I> n, O result, std::true_type) const {
    static_assert(std::is_trivial<std::remove_reference_t<decltype(*first)>>::value, "Invalid type");
    static_assert(std::is_trivial<std::remove_reference_t<decltype(*result)>>::value, "Invalid type");
    std::memmove(preview::addressof(*result), preview::addressof(*first), n * sizeof(*first));
    return {first + n, result + n};
  }

  template<typename I, typename O>
  constexpr copy_n_result<I, O> call(I first, iter_difference_t<I> n, O result, std::false_type) const {
    for (; n > 0; --n) {
      *result = *first;
      ++result;
      ++first;
    }
    return {std::move(first), std::move(result)};
  }

 public:
  template<typename I, typename O, std::enable_if_t<conjunction<
      input_iterator<I>,
      weakly_incrementable<O>,
      indirectly_copyable<I, O>
  >::value, int> = 0>
  constexpr copy_n_result<I, O> operator()(I first, iter_difference_t<I> n, O result) const {
    using pointer_to_trivial = conjunction<
        is_pointer_to_trivial<I>,
        is_pointer_to_trivial<O>
    >;
    using contiguous_iterator_to_trivial = conjunction<
        // Must check if `std::contiguous_iterator_tag` is defined since
        // `preview::contiguous_iterator` may yield false positive result without `std::contiguous_iterator_tag`.
        have_std_contiguous_iterator_tag,
        contiguous_iterator<I>,
        contiguous_iterator<O>,
        std::is_trivial<std::remove_reference_t<iter_reference_t<I>>>,
        std::is_trivial<std::remove_reference_t<iter_reference_t<O>>>
    >;
    using use_memmove = disjunction<pointer_to_trivial, contiguous_iterator_to_trivial>;

    return call(std::move(first), std::move(n), std::move(result), use_memmove{});
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::copy_n_niebloid copy_n;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ALGORITHM_RANGES_COPY_N_H_
