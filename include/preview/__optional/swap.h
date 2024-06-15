# /*
#  * Created by YongGyu Lee on 2021/05/24.
#  */
#
# ifndef PREVIEW_OPTIONAL_SWAP_H_
# define PREVIEW_OPTIONAL_SWAP_H_
#
# include <algorithm>
# include <type_traits>
# include <utility>
#
# include "preview/__optional/forward_declare.h"
# include "preview/__type_traits/conjunction.h"
# include "preview/__type_traits/is_swappable.h"

namespace std {

template<typename T,
  ::std::enable_if_t<
    ::std::is_move_constructible<T>::value && ::preview::is_swappable<T>::value,
  int> = 0>
void swap(::preview::optional<T>& lhs, ::preview::optional<T>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

} // namespace std

# endif // PREVIEW_OPTIONAL_SWAP_H_
