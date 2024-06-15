# /*
#  * Created by YongGyu Lee on 2021/05/24.
#  */
#
# ifndef PREVIEW_OPTIONAL_INTERNAL_DTOR_H_
# define PREVIEW_OPTIONAL_INTERNAL_DTOR_H_
#
# include <memory>
# include <type_traits>
# include <utility>
#
# include "preview/__memory/addressof.h"
# include "preview/__utility/in_place.h"

namespace preview {
namespace internal {
namespace optional {

template<typename T, bool v = /* true */ std::is_trivially_destructible<T>::value>
struct dtor {
  using value_type = T;

  constexpr dtor() noexcept
    : null{} {}

  template<typename ...Args>
  constexpr explicit dtor(in_place_t, Args&&... args)
    : val(std::forward<Args>(args)...),
      valid(true) {}

  void reset() {
    if (valid) {
      val.~value_type();
      valid = false;
    }
  }

  constexpr inline const value_type* pointer() const noexcept { return preview::addressof(val); }
  constexpr inline       value_type* pointer()       noexcept { return preview::addressof(val); }

  constexpr inline const value_type& ref() const&  noexcept { return val;             }
  constexpr inline       value_type& ref()      &  noexcept { return val;             }
  constexpr inline const value_type& ref() const&& noexcept { return std::move(val);  }
  constexpr inline       value_type& ref()      && noexcept { return std::move(val);  }

  template<typename ...Args>
  void construct(Args&&... args) {
    ::new((void*)preview::addressof(val)) value_type(std::forward<Args>(args)...);
    valid = true;
  }

  template<typename Other>
  void construct_if(Other&& other) {
    if (other)
      construct(*std::forward<Other>(other));
  }

  ~dtor() = default;

  union {
    char null;
    value_type val;
  };
  bool valid = false;
};

template<typename T>
struct dtor<T, false> {
  using value_type = T;

  constexpr dtor() noexcept
    : null{} {}

  template<typename ...Args>
  constexpr explicit dtor(in_place_t, Args&&... args)
    : val(std::forward<Args>(args)...),
      valid(true) {}

  void reset() {
    if (valid) {
      val.~value_type();
      valid = false;
    }
  }

  constexpr inline const value_type* pointer() const noexcept { return preview::addressof(val); }
  constexpr inline       value_type* pointer()       noexcept { return preview::addressof(val); }

  constexpr inline const value_type& ref() const&  noexcept { return val;             }
  constexpr inline       value_type& ref()      &  noexcept { return val;             }
  constexpr inline const value_type& ref() const&& noexcept { return std::move(val);  }
  constexpr inline       value_type& ref()      && noexcept { return std::move(val);  }

  template<typename ...Args>
  void construct(Args&&... args) {
    ::new((void*)preview::addressof(val)) value_type(std::forward<Args>(args)...);
    valid = true;
  }

  template<typename Other>
  void construct_if(Other&& other) {
    if (other)
      construct(*std::forward<Other>(other));
  }

  ~dtor() {
    if (valid)
      val.T::~T();
  };

  union {
    char null;
    T val;
  };
  bool valid = false;
};

} // namespace optional
} // namespace internal
} // namespace preview

# endif // PREVIEW_OPTIONAL_INTERNAL_DTOR_H_
