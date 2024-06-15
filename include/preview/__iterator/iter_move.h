//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_ITERATOR_ITER_MOVE_H_
#define PREVIEW_ITERATOR_ITER_MOVE_H_

#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__core/inline_variable.h"
#include "preview/__concepts/dereferenceable.h"
#include "preview/__type_traits/detail/return_category.h"
#include "preview/__type_traits/is_class_or_enum.h"

namespace preview {
namespace ranges {
namespace detail_iter_move {

struct iter_move_niebloid {
 private:
  template<std::size_t N>
  using return_category = preview::detail::return_category<N>;

  template<
      typename T,
      bool = is_class_or_enum<std::decay_t<T>>::value,
      typename = void
  >
  struct has_adl : std::false_type {
    using category = return_category<0>;
  };
  template<typename T>
  struct has_adl<T, true, void_t<decltype(iter_move(std::declval<T>()))>> : std::true_type {
    using category = return_category<1>;
  };

  template<typename I, template<typename, typename...> class Test, bool = dereferenceable<I>::value /* false */>
  struct deref_is : std::false_type {};

  template<typename I, template<typename, typename...> class Test>
  struct deref_is<I, Test, true> : Test<decltype(*std::declval<I>())> {};

  template<typename T>
  struct category
      : std::conditional_t<
            has_adl<T>::value, return_category<1>,
        std::conditional_t<
            deref_is<T, std::is_lvalue_reference>::value, return_category<2>,
        std::conditional_t<
            dereferenceable<T>::value, return_category<3>,
            return_category<0>
        >>> {};

  template<typename T>
  constexpr decltype(auto) call(T&& t, return_category<1>) const {
    return iter_move(std::forward<T>(t));
  }

  template<typename T>
  constexpr decltype(auto) call(T&& t, return_category<2>) const {
    return std::move(*std::forward<T>(t));
  }

  template<typename T>
  constexpr decltype(auto) call(T&& t, return_category<3>) const {
    return *std::forward<T>(t);
  }

 public:
  template<typename T, std::enable_if_t<(category<T&&>::value > 0), int> = 0>
  constexpr decltype(auto) operator()(T &&t) const {
    return this->call(std::forward<T>(t), category<T&&>{});
  }
};

} // namespace detail_iter_move

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail_iter_move::iter_move_niebloid iter_move{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_ITERATOR_ITER_MOVE_H_
