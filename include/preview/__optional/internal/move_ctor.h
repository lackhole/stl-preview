# /*
#  * Created by YongGyu Lee on 2021/05/24.
#  */
#
# ifndef PREVIEW_OPTIONAL_INTERNAL_MOVE_CTOR_H_
# define PREVIEW_OPTIONAL_INTERNAL_MOVE_CTOR_H_
#
# include <type_traits>
#
# include "preview/__optional/internal/copy_ctor.h"

namespace preview {
namespace internal {
namespace optional {

template<typename T, bool v = std::is_trivially_move_constructible<T>::value>
struct move_ctor : copy_ctor<T> {
  using base = copy_ctor<T>;
  using base::base;
};

template<typename T>
struct move_ctor<T, false> : copy_ctor<T> {
  using base = copy_ctor<T>;
  using base::base;

  move_ctor() = default;
  move_ctor(move_ctor const&) = default;
  move_ctor(move_ctor && other) noexcept(std::is_nothrow_move_constructible<T>::value) {
    if (other.valid) {
      this->construct(std::move(other.val));
    }
  }
  move_ctor& operator=(move_ctor const&) = default;
  move_ctor& operator=(move_ctor &&) = default;
};

} // namespace optional
} // namespace internal
} // namespace preview


# endif // PREVIEW_OPTIONAL_INTERNAL_MOVE_CTOR_H_
