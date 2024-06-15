# /*
#  * Created by YongGyu Lee on 2021/05/23.
#  */
#
# ifndef PREVIEW_OPTIONAL_OPTIONAL_H_
# define PREVIEW_OPTIONAL_OPTIONAL_H_
#
# include <type_traits>
# include <new>
# include <utility>
#
# include "preview/__concepts/copy_constructible.h"
# include "preview/__concepts/invocable.h"
# include "preview/__concepts/move_constructible.h"
# include "preview/__functional/invoke.h"
# include "preview/__optional/internal/check_overload.h"
# include "preview/__optional/internal/move_assignment.h"
# include "preview/__optional/internal/traits.h"
# include "preview/__optional/bad_optional_access.h"
# include "preview/__optional/nullopt_t.h"
# include "preview/__optional/swap.h"
# include "preview/__type_traits/is_swappable.h"
# include "preview/__type_traits/is_invocable.h"
# include "preview/__type_traits/remove_cvref.h"
# include "preview/__utility/in_place.h"

namespace preview {

template<typename T>
class optional :
  private internal::optional::move_assign<T>,
  private internal::optional::check_copy_constructible<T>,
  private internal::optional::check_move_constructible<T>,
  private internal::optional::check_copy_assignable<T>,
  private internal::optional::check_move_assignable<T> {
  using base = internal::optional::move_assign<T>;
  using base::base;

 public:
  using value_type = T;

  static_assert(!std::is_reference<T>::value,
                "preview::optional : T must not be a reference type");
  static_assert(!std::is_same<std::remove_reference_t<std::decay_t<T>>, nullopt_t>::value,
                "preview::optional : T must not be preview::nullopt_t");
  static_assert(!std::is_same<std::remove_reference_t<std::decay_t<T>>, in_place_t>::value,
                "preview::optional : T must not be preview::in_place_t");
  static_assert(std::is_destructible<T>::value,
                "preview::optional : T must be destructible");
  static_assert(!std::is_array<T>::value,
                "preview::optional : T must be an array type");

  // constructors
  constexpr optional() noexcept = default;
  constexpr optional(nullopt_t) noexcept {};
  constexpr optional(const optional& other) = default;
  constexpr optional(optional&& other) = default;

  template<typename U,
    std::enable_if_t<
      std::is_constructible<value_type, const U&>::value &&
      internal::optional::check_constructible<value_type, optional<U>>::value &&
      internal::optional::check_convertible  <value_type, optional<U>>::value,
    int> = 0>
  constexpr optional(const optional<U>& other) {
    this->construct_if(other);
  }

  template<typename U,
    std::enable_if_t<
      std::is_constructible<value_type, const U&>::value &&
      internal::optional::check_constructible<value_type, optional<U>>::value &&
      internal::optional::check_convertible  <value_type, optional<U>>::value &&
      !std::is_convertible<const U&, value_type>::value,
    int> = 0>
  explicit constexpr optional(const optional<U>& other) {
    this->construct_if(other);
  }

  template<typename U,
    std::enable_if_t<
      std::is_constructible<value_type, U&&>::value &&
      internal::optional::check_constructible<value_type, optional<U>>::value &&
      internal::optional::check_convertible  <value_type, optional<U>>::value,
    int> = 0>
  constexpr optional(optional<U>&& other) {
    this->construct_if(std::move(other));
  }

  template<typename U,
    std::enable_if_t<
      std::is_constructible<value_type, U&&>::value &&
      internal::optional::check_constructible<value_type, optional<U>>::value &&
      internal::optional::check_convertible  <value_type, optional<U>>::value &&
      !std::is_convertible<U&&, value_type>::value,
    int> = 0>
  explicit constexpr optional(optional<U>&& other) {
    this->construct_if(std::move(other));
  }

  // Split into 2 overloads to prevent MSVC from making an ambiguous call in C++14
  template<typename InPlaceT,
    std::enable_if_t<
      std::is_same<InPlaceT, in_place_t>::value &&
      std::is_constructible<value_type>::value,
    int> = 0>
  constexpr explicit optional(InPlaceT)
    : base(in_place) {}

  template<typename Arg, typename ...Args,
    std::enable_if_t<
      std::is_constructible<value_type, Arg, Args...>::value,
    int> = 0>
  constexpr explicit optional(in_place_t, Arg&& arg, Args&&... args)
    : base(in_place, std::forward<Arg>(arg), std::forward<Args>(args)...) {}

  template<typename U, typename ...Args,
    std::enable_if_t<
      std::is_constructible<value_type, std::initializer_list<U>&, Args&&...>::value,
    int> = 0>
  constexpr explicit optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
    : base(in_place, ilist, std::forward<Args>(args)...) {}

  template<typename U = value_type,
    std::enable_if_t<
      std::is_constructible<value_type,U&&>::value &&
      std::is_convertible<U&&, value_type>::value &&
      !std::is_same<internal::optional::strip_t<U>, in_place_t>::value &&
      !std::is_same<internal::optional::strip_t<U>, optional<value_type>>::value,
    int> = 0>
  constexpr optional(U&& value)
    : base(in_place, std::forward<U>(value)) {}

  template<typename U = value_type,
    std::enable_if_t<
      std::is_constructible<value_type,U&&>::value &&
      !std::is_convertible<U&&, value_type>::value &&
      !std::is_same<internal::optional::strip_t<U>, in_place_t>::value &&
      !std::is_same<internal::optional::strip_t<U>, optional<value_type>>::value,
    int> = 0>
  constexpr explicit optional(U&& value)
    : base(in_place, std::forward<U>(value)) {}

  // assignment operators

  optional& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  constexpr optional& operator=(optional const&) = default;
  constexpr optional& operator=(optional &&) = default;

  template<typename U,
    std::enable_if_t<
      (std::is_constructible<value_type, U>::value &&
       std::is_assignable<value_type&, U>::value &&
       !std::is_same<internal::optional::strip_t<U>, optional>::value) &&
      (!std::is_scalar<value_type>::value ||
       !std::is_same<std::decay_t<U>, value_type>::value),
    int> = 0>
  optional& operator=(U&& value) {
    if (has_value()) {
      this->val = std::forward<U>(value);
    } else {
      this->construct(std::forward<U>(value));
    }
    return *this;
  }

  template<typename U,
    std::enable_if_t<
      internal::optional::check_constructible<value_type, optional<U>>::value &&
      internal::optional::check_convertible  <value_type, optional<U>>::value &&
      internal::optional::check_assignable   <value_type, optional<U>>::value &&
      std::is_constructible<value_type , const U&>::value &&
      std::is_assignable   <value_type&, const U&>::value,
    int> = 0>
  optional& operator=(const optional<U>& other) {
    if (other.has_value()) {
      if (this->has_value())
        this->val = *other;
      else
        this->construct(*other);
    } else { // !other.has_value()
      if (this->has_value())
        this->reset();
    }
    return *this;
  }

  template<typename U,
    std::enable_if_t<
      internal::optional::check_constructible<value_type, optional<U>>::value &&
      internal::optional::check_convertible  <value_type, optional<U>>::value &&
      internal::optional::check_assignable   <value_type, optional<U>>::value &&
      std::is_constructible<value_type , U>::value &&
      std::is_assignable   <value_type&, U>::value,
    int> = 0>
  optional& operator=(optional<U>&& other) {
    if (other.has_value()) {
      if (this->has_value())
        this->val = std::move(*other);
      else
        this->construct(std::move(*other));
    } else { // !other.has_value()
      if (this->has_value())
        this->reset();
    }
    return *this;
  }

  constexpr inline const value_type*  operator->() const noexcept { return this->pointer(); }
  constexpr inline       value_type*  operator->()       noexcept { return this->pointer(); }

  constexpr inline const value_type&  operator*() const&  noexcept { return this->ref(); }
  constexpr inline       value_type&  operator*()      &  noexcept { return this->ref(); }
  constexpr inline const value_type&& operator*() const&& noexcept { return std::move(this->ref()); }
  constexpr inline       value_type&& operator*()      && noexcept { return std::move(this->ref()); }

  constexpr explicit inline operator bool() const noexcept {
    return this->valid;
  }
  constexpr inline bool has_value() const noexcept {
    return this->valid;
  }

  constexpr inline value_type& value() & {
    if (!this->has_value())
      throw bad_optional_access{};
    return this->ref();
  }
  constexpr inline const value_type& value() const& {
    if (!this->has_value())
      throw bad_optional_access{};
    return this->ref();
  }
  constexpr inline value_type&& value() && {
    if (!this->has_value())
      throw bad_optional_access{};
    return std::move(this->ref());
  }
  constexpr inline const value_type&& value() const && {
    if (!this->has_value())
      throw bad_optional_access{};
    return std::move(this->ref());
  }

  template<typename U>
  constexpr value_type value_or(U&& default_value) const & {
    static_assert(std::is_copy_constructible<value_type>::value,
                  "preview::optional<T>::value_or : T must be copy constructible");
    static_assert(std::is_convertible<U&&, value_type>::value,
                  "preview::optional<T>::value_or : U&& must be convertible to T");

    return this->has_value() ? **this : static_cast<value_type>(std::forward<U>(default_value));
  }

  template<typename U>
  constexpr value_type value_or(U&& default_value) && {
    static_assert(std::is_move_constructible<value_type>::value,
                  "preview::optional<T>::value_or : T must be move constructible");
    static_assert(std::is_convertible<U&&, value_type>::value,
                  "preview::optional<T>::value_or : U&& must be convertible to T");

    return this->has_value() ? std::move(**this) : static_cast<value_type>(std::forward<U>(default_value));
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, value_type&>, optional>::value, int> = 0>
  constexpr auto and_then(F&& f) & {
    if (*this)
      return preview::invoke(std::forward<F>(f), **this);
    return remove_cvref_t<invoke_result_t<F, value_type&>>{};
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, const value_type&>, optional>::value, int> = 0>
  constexpr auto and_then(F&& f) const & {
    if (*this)
      return preview::invoke(std::forward<F>(f), **this);
    return remove_cvref_t<invoke_result_t<F, const value_type&>>{};
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, value_type&&>, optional>::value, int> = 0>
  constexpr auto and_then(F&& f) && {
    if (*this)
      return preview::invoke(std::forward<F>(f), std::move(**this));
    return remove_cvref_t<invoke_result_t<F, value_type>>{};
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, const value_type&&>, optional>::value, int> = 0>
  constexpr auto and_then(F&& f) const && {
    if (*this)
      return preview::invoke(std::forward<F>(f), std::move(**this));
    return remove_cvref_t<invoke_result_t<F, const value_type>>{};
  }

  template<typename F, std::enable_if_t<is_invocable<F, value_type&>::value, int> = 0>
  constexpr auto transform(F&& f) & {
    using U = std::remove_cv_t<invoke_result_t<F, T&>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), **this));
    return optional<U>{};
  }

  template<typename F, std::enable_if_t<is_invocable<F, const value_type&>::value, int> = 0>
  constexpr auto transform(F&& f) const & {
    using U = std::remove_cv_t<invoke_result_t<F, const T&>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), **this));
    return optional<U>{};
  }

  template<typename F, std::enable_if_t<is_invocable<F, value_type&>::value, int> = 0>
  constexpr auto transform(F&& f) && {
    using U = std::remove_cv_t<invoke_result_t<F, T>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), std::move(**this)));
    return optional<U>{};
  }

  template<typename F, std::enable_if_t<is_invocable<F, value_type&>::value, int> = 0>
  constexpr auto transform(F&& f) const && {
    using U = std::remove_cv_t<invoke_result_t<F, const T>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), std::move(**this)));
    return optional<U>{};
  }

  template<typename F, std::enable_if_t<conjunction<
      copy_constructible<T>,
      invocable<F>
  >::value, int> = 0>
  constexpr optional or_else(F&& f) const& {
    static_assert(std::is_same<remove_cvref_t<invoke_result_t<F>>, optional>::value, "Invalid return type from f");
    return *this ? *this : std::forward<F>(f)();
  }
  template<typename F, std::enable_if_t<conjunction<
      move_constructible<T>,
      invocable<F>
  >::value, int> = 0>
  constexpr optional or_else(F&& f) && {
    static_assert(std::is_same<remove_cvref_t<invoke_result_t<F>>, optional>::value, "Invalid return type from f");
    return *this ? std::move(*this) : std::forward<F>(f)();
  }

  void swap(optional& other)
  noexcept(std::is_nothrow_move_constructible<value_type>::value && preview::is_nothrow_swappable<value_type>::value)
  {
    static_assert(std::is_move_constructible<value_type>::value,
                  "preview::optional<T>::swap : T must be move constructible");

    if (other.has_value()) {
      if (this->has_value()) {
        std::swap(**this, *other);
      } else { // !this->has_value()
        this->construct(std::move(*other));
        other.reset();
      }
    } else {
      if (this->has_value()) {
        other.construct(**this);
        this->reset();
      }
    }
  }

  template<typename Dummy = void,
    std::enable_if_t<
      std::is_same<Dummy, void>::value &&
      std::is_constructible<value_type>::value,
    int> = 0>
  value_type& emplace() {
    this->reset();
    this->construct();
    return **this;
  }

  template<typename Arg, typename ...Args,
    std::enable_if_t<
      std::is_constructible<value_type, Arg, Args...>::value,
    int> = 0>
  value_type& emplace(Arg&& arg, Args&&... args) {
    this->reset();
    this->construct(std::forward<Arg>(arg), std::forward<Args>(args)...);
    return **this;
  }

  template<typename U, typename ...Args,
    std::enable_if_t<
      std::is_constructible<value_type, std::initializer_list<U>&, Args&&...>::value,
    int> = 0>
  value_type& emplace(std::initializer_list<U> ilist, Args&&... args) {
    this->reset();
    this->construct(ilist, std::forward<Args>(args)...);
    return **this;
  }

  using base::reset;
};

# if __cplusplus >= 201703
template<class T> optional(T) -> optional<T>;
# endif

template<typename T>
constexpr inline optional<std::decay_t<T>> make_optional(T&& value) {
  return optional<std::decay_t<T>>(std::forward<T>(value));
}

template<typename T>
constexpr inline optional<T> make_optional() {
  return optional<T>(in_place);
}

template<typename T, typename Arg, typename ...Args>
constexpr inline optional<T> make_optional(Arg&& arg, Args&&... args) {
  return optional<T>(in_place, std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template<typename T, typename U, typename ...Args>
constexpr inline optional<T> make_optional(std::initializer_list<U> ilist, Args&&... args) {
  return optional<T>(in_place, ilist, std::forward<Args>(args)...);
}

// compare two optional objects

template<typename T, typename U >
constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs) {
  if (bool(lhs) != bool(rhs))
    return false;
  if (!lhs)
    return true;
  return *lhs == *rhs;
}

template<typename T, typename U >
constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs) {
  if (bool(lhs) != bool(rhs))
    return true;
  if (!lhs)
    return false;
  return lhs != rhs;
}

template<typename T, typename U >
constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs) {
  if (!rhs.has_value())
    return false;
  if (!lhs.has_value())
    return true;
  return *lhs < *rhs;
}

template<typename T, typename U >
constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs) {
  if (!lhs.has_value())
    return true;
  if (!rhs.has_value())
    return false;
  return *lhs <= *rhs;
}

template<typename T, typename U >
constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs) {
  if (!lhs.has_value())
    return false;
  if (!rhs.has_value())
    return true;
  return *lhs > *rhs;
}

template<typename T, typename U >
constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs) {
  if (!rhs.has_value())
    return true;
  if (!lhs.has_value())
    return false;
  return *lhs >= *rhs;
}


// compare an optional object with a nullopt

template<typename T>
constexpr inline bool operator==(const optional<T>& opt, nullopt_t) noexcept {
  return !opt;
}

template<typename T>
constexpr inline bool operator==(nullopt_t, const optional<T>& opt) noexcept {
  return !opt;
}

template<typename T>
constexpr inline bool operator!=(const optional<T>& opt, nullopt_t) noexcept {
  return bool(opt);
}

template<typename T>
constexpr inline bool operator!=(nullopt_t, const optional<T>& opt) noexcept {
  return bool(opt);
}

template<typename T>
constexpr inline bool operator<(const optional<T>& opt, nullopt_t) noexcept {
  return false;
}

template<typename T>
constexpr inline bool operator<(nullopt_t, const optional<T>& opt) noexcept {
  return bool(opt);
}

template<typename T>
constexpr inline bool operator<=(const optional<T>& opt, nullopt_t) noexcept {
  return !opt;
}

template<typename T>
constexpr inline bool operator<=(nullopt_t, const optional<T>& opt) noexcept {
  return true;
}

template<typename T>
constexpr inline bool operator>(const optional<T>& opt, nullopt_t) noexcept {
  return bool(opt);
}

template<typename T>
constexpr inline bool operator>(nullopt_t, const optional<T>& opt) noexcept {
  return false;
}

template<typename T>
constexpr inline bool operator>=(const optional<T>& opt, nullopt_t) noexcept {
  return true;
}

template<typename T>
constexpr inline bool operator>=(nullopt_t, const optional<T>& opt) noexcept {
  return !opt;
}


// compare an optional object with a value

template<typename T, typename U>
constexpr inline bool operator==(const optional<T>& opt, const U& value) {
  return bool(opt) ? *opt == value : false;
}

template<typename T, typename U>
constexpr inline bool operator==(const T& value, const optional<U>& opt) {
  return bool(opt) ? value == *opt : false;
}

template<typename T, typename U>
constexpr inline bool operator!=(const optional<T>& opt, const U& value) {
  return bool(opt) ? opt != value : true;
}

template<typename T, typename U>
constexpr inline bool operator!=(const T& value, const optional<U>& opt) {
  return bool(opt) ? value != opt : true;
}

template<typename T, typename U>
constexpr inline bool operator<(const optional<T>& opt, const U& value) {
  return bool(opt) ? opt < value : true;
}

template<typename T, typename U>
constexpr inline bool operator<(const T& value, const optional<U>& opt) {
  return bool(opt) ? value < opt : false;
}

template<typename T, typename U>
constexpr inline bool operator<=(const optional<T>& opt, const U& value) {
  return bool(opt) ? opt <= value : true;
}

template<typename T, typename U>
constexpr inline bool operator<=(const T& value, const optional<U>& opt) {
  return bool(opt) ? value <= opt : false;
}

template<typename T, typename U>
constexpr inline bool operator>(const optional<T>& opt, const U& value) {
  return bool(opt) ? opt > value : false;
}

template<typename T, typename U>
constexpr inline bool operator>(const T& value, const optional<U>& opt) {
  return bool(opt) ? value > opt : true;
}

template<typename T, typename U>
constexpr inline bool operator>=(const optional<T>& opt, const U& value) {
  return bool(opt) ? opt >= value : false;
}

template<typename T, typename U>
constexpr inline bool operator>=(const T& value, const optional<U>& opt) {
  return bool(opt) ? value >= opt : true;
}

} // namespace preview

# endif // PREVIEW_OPTIONAL_OPTIONAL_H_
