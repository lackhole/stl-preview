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
# include "preview/__optional/optional.h"

namespace preview {
namespace detail {

template<typename Optional>
struct optional_enabled_hash;

template<typename T>
struct optional_enabled_hash<optional<T>> {
  std::size_t operator()(const optional<T>& key) const noexcept {
    return key.has_value() ? std::hash<std::remove_const_t<T>>()(*key) : 0;
  }
};

struct optional_disabled_hash {
  optional_disabled_hash() = delete;
};

template<typename Optional>
struct optional_hash;

template<typename T>
struct optional_hash<optional<T>>
    : std::conditional_t<
        std::is_default_constructible<std::hash<T>>::value,
        optional_enabled_hash<optional<T>>,
        optional_disabled_hash
    > {};

} // namespace detail
} // namespace preview

namespace std {

template<typename T>
struct hash<preview::optional<T>>
    : preview::detail::optional_hash<preview::optional<T>>
{
  using argument_type = preview::optional<T>;
  using result_type = std::size_t;
};

} // namespace std

# endif // PREVIEW_OPTIONAL_HASH_H_
