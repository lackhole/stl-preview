# /*
#  * Created by YongGyu Lee on 2021/05/24.
#  */
#
# ifndef PREVIEW_OPTIONAL_INTERNAL_COPY_CTOR_H_
# define PREVIEW_OPTIONAL_INTERNAL_COPY_CTOR_H_
#
# include <type_traits>
#
# include "preview/__optional/internal/dtor.h"

namespace preview {
namespace internal {
namespace optional {

template<typename T, bool v = std::is_trivially_copy_constructible<T>::value>
struct copy_ctor : dtor<T> {
  using base = dtor<T>;
  using base::base;
};

template<typename T>
struct copy_ctor<T, false> : dtor<T> {
  using base = dtor<T>;
  using base::base;

  copy_ctor() = default;
  copy_ctor(copy_ctor const& other) {
    if (other.valid) {
      this->construct(other.val);
    }
  }
  copy_ctor(copy_ctor &&) = default;
  copy_ctor& operator=(copy_ctor const&) = default;
  copy_ctor& operator=(copy_ctor &&) = default;
};

} // namespace optional
} // namespace internal
} // namespace preview


# endif // PREVIEW_OPTIONAL_INTERNAL_COPY_CTOR_H_
