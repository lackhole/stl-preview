# /*
#  * Created by YongGyu Lee on 2021/05/24.
#  */
#
# ifndef PREVIEW_OPTIONAL_HASH_H_
# define PREVIEW_OPTIONAL_HASH_H_
#
# include <cstddef>
#
# include <functional>
# include <type_traits>
#
# include "preview/__optional/internal/hash.h"
# include "preview/__optional/optional.h"

namespace std {

template<typename T>
struct hash<preview::optional<T>> : preview::internal::optional::hash_constructible<std::remove_const_t<T>> {
  using argument_type = preview::optional<T>;
  using result_type = std::size_t;

  result_type operator()(const argument_type& key) const {
    return key.has_value() ? ::std::hash<std::remove_const_t<T>>()(*key) : 0;
  }
};

} // namespace std

# endif // PREVIEW_OPTIONAL_HASH_H_
