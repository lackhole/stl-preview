# /*
#  * Created by YongGyu Lee on 2020/12/08.
#  */
#
# ifndef PREVIEW_TUPLE_APPLY_H_
# define PREVIEW_TUPLE_APPLY_H_
#
# include <type_traits>
# include <utility>
#
# include "preview/__functional/invoke.h"

namespace preview {
namespace detail {

template<class F, class Tuple, std::size_t... I>
constexpr inline decltype(auto)
apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
  return preview::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
}

} // namespace detail

template<class F, class Tuple>
constexpr inline
decltype(auto)
apply(F&& f, Tuple&& t) {
  return detail::apply_impl(
      std::forward<F>(f), std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}

} // namespace preview

# endif // PREVIEW_TUPLE_APPLY_H_
