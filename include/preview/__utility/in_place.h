# /*
#  * Created by YongGyu Lee on 2021/05/23.
#  */
#
# ifndef PREVIEW_UTILITY_IN_PLACE_H_
# define PREVIEW_UTILITY_IN_PLACE_H_
#
# include <cstddef>
#if __cplusplus >= 201703L
# include <utility>
#endif
#
# include "preview/__core/inline_variable.h"

namespace preview {
#if __cplusplus < 201703L

struct in_place_t {
  constexpr explicit in_place_t() = default;
};

template<typename T>
struct in_place_type_t {
  constexpr explicit in_place_type_t() = default;
};

template<std::size_t I>
struct in_place_index_t {
  constexpr explicit in_place_index_t() = default;
};

#else

using in_place_t = std::in_place_t;
template<typename T> using in_place_type_t = std::in_place_type_t<T>;
template<std::size_t I> using in_place_index_t = std::in_place_index_t<I>;

#endif

PREVIEW_INLINE_VARIABLE constexpr in_place_t in_place{};

template<typename T>
PREVIEW_INLINE_VARIABLE constexpr in_place_type_t<T> in_place_type{};

template<std::size_t I>
PREVIEW_INLINE_VARIABLE constexpr in_place_index_t<I> in_place_index{};

} // namespace preview

# endif // PREVIEW_UTILITY_IN_PLACE_H_
