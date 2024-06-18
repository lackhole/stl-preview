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
# include "preview/core.h"
# include "preview/__concepts/copy_constructible.h"
# include "preview/__concepts/invocable.h"
# include "preview/__concepts/move_constructible.h"
# include "preview/__functional/invoke.h"
# include "preview/__memory/construct_at.h"
# include "preview/__memory/destroy_at.h"
# include "preview/__optional/bad_optional_access.h"
# include "preview/__optional/nullopt_t.h"
# include "preview/__optional/swap.h"
# include "preview/__type_traits/detail/control_special.h"
# include "preview/__type_traits/is_swappable.h"
# include "preview/__type_traits/is_invocable.h"
# include "preview/__type_traits/is_specialization.h"
# include "preview/__type_traits/remove_cvref.h"
# include "preview/__utility/in_place.h"

namespace preview {
namespace detail {

template<typename T, bool = std::is_trivially_destructible<T>::value /* true */>
struct optional_storage {
  using value_type = T;

  constexpr optional_storage() noexcept
      : null{}
      , has_value(false) {}

  template<typename... Args>
  constexpr optional_storage(in_place_t, Args&&... args)
      : value(std::forward<Args>(args)...)
      , has_value(true) {}

  union {
    char null;
    T value;
  };
  bool has_value;
};

template<typename T>
struct optional_storage<T, false> {
  using value_type = T;

  constexpr optional_storage() noexcept
      : null{}
      , has_value(false) {}

  template<typename... Args>
  constexpr optional_storage(in_place_t, Args&&... args)
      : value(std::forward<Args>(args)...)
      , has_value(true) {}

  PREVIEW_CONSTEXPR_AFTER_CXX20 ~optional_storage() {
    if (has_value)
      preview::destroy_at(std::addressof(value));
  }

  union {
    char null;
    T value;
  };
  bool has_value;
};

template<typename T>
struct optional_base {
 public:
  using value_type = T;

  constexpr optional_base() noexcept = default;

  template<typename... Args>
  constexpr explicit optional_base(in_place_t, Args&&... args)
      : storage_(in_place_t{}, std::forward<Args>(args)...) {}

  void reset() {
    if (storage_.has_value) {
      preview::destroy_at(std::addressof(storage_.value));
      storage_.has_value = false;
    }
  }

  PREVIEW_CONSTEXPR_AFTER_CXX17 const value_type* operator->() const noexcept { return std::addressof(storage_.value); }
  PREVIEW_CONSTEXPR_AFTER_CXX17       value_type* operator->()       noexcept { return std::addressof(storage_.value); }

  constexpr const value_type&  operator*() const&  noexcept { return storage_.value;             }
  constexpr       value_type&  operator*()      &  noexcept { return storage_.value;             }
  constexpr const value_type&& operator*() const&& noexcept { return std::move(storage_.value);  }
  constexpr       value_type&& operator*()      && noexcept { return std::move(storage_.value);  }

  constexpr explicit operator bool() const noexcept {
    return storage_.has_value;
  }

  constexpr bool has_value() const noexcept {
    return storage_.has_value;
  }

  constexpr value_type& value() & {
    throw_if_empty();
    return storage_.value;
  }

  constexpr const value_type& value() const& {
    throw_if_empty();
    return storage_.value;
  }

  constexpr value_type&& value() && {
    throw_if_empty();
    return std::move(storage_.value);
  }

  constexpr const value_type&& value() const&& {
    throw_if_empty();
    return std::move(storage_.value);
  }

  // control-special
  constexpr void construct_from(const optional_base& other) {
    if (other.has_value())
      construct_with(*other);
  }

  // control-special
  constexpr void construct_from(optional_base&& other) {
    if (other.has_value())
      construct_with(std::move(*other));
  }

  // control-special
  constexpr void assign_from(const optional_base& other) {
    if (!other.has_value()) {
      reset();
    } else {
      if (has_value())
        storage_.value = other.value();
      else
        construct_with(other.value());
    }
  }

  // control-special
  constexpr void assign_from(optional_base&& other) {
    if (!other.has_value()) {
      reset();
    } else {
      if (has_value())
        storage_.value = std::move(other.value());
      else
        construct_with(std::move(other.value()));
    }
  }

  template<typename ...Args>
  void construct_with(Args&&... args) {
    preview::construct_at(std::addressof(storage_.value), std::forward<Args>(args)...);
    storage_.has_value = true;
  }

  void destruct() {
    if (has_value())
      preview::destroy_at(std::addressof(storage_.value));
  }

 private:
  void throw_if_empty() const {
    if (!has_value())
      throw bad_optional_access{};
  }

  optional_storage<T> storage_;
};

template<typename T>
struct optional_nontrivial_dtor : optional_base<T> {
  using base = optional_base<T>;
  using base::base;

  PREVIEW_CONSTEXPR_AFTER_CXX20 ~optional_nontrivial_dtor() {
    base::destruct();
  }

  optional_nontrivial_dtor() = default;
  constexpr optional_nontrivial_dtor(const optional_nontrivial_dtor&) = default;
  constexpr optional_nontrivial_dtor(optional_nontrivial_dtor&&) = default;
  constexpr optional_nontrivial_dtor& operator=(const optional_nontrivial_dtor&) = default;
  constexpr optional_nontrivial_dtor& operator=(optional_nontrivial_dtor&&) = default;
};

template<typename T>
using optional_control_smf =
    std::conditional_t<
        std::is_trivially_destructible<T>::value,
        control_special<optional_base<T>, T>,
        control_special<optional_nontrivial_dtor<T>, T>
    >;

template<typename To, typename From>
using never_constructible_from = conjunction<
    negation<std::is_constructible<To, From &       >>,
    negation<std::is_constructible<To, From const&  >>,
    negation<std::is_constructible<To, From &&      >>,
    negation<std::is_constructible<To, From const&& >>
>;

template<typename To, typename From>
using never_convertible_from = conjunction<
    negation<std::is_convertible<From &      , To>>,
    negation<std::is_convertible<From const& , To>>,
    negation<std::is_convertible<From &&     , To>>,
    negation<std::is_convertible<From const&&, To>>
>;

template<typename To, typename From>
using never_constructible_or_convertible_from = conjunction<
    never_constructible_from<To, From>,
    never_convertible_from<To, From>
>;

template<typename To, typename From>
using never_assignable_from = conjunction<
    negation<std::is_assignable<To&, From &      >>,
    negation<std::is_assignable<To&, From const& >>,
    negation<std::is_assignable<To&, From &&     >>,
    negation<std::is_assignable<To&, From const&&>>
>;

} // namespace detail

template<typename T>
class optional : private detail::optional_control_smf<T> {
  using base = detail::optional_control_smf<T>;
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

  template<typename U, std::enable_if_t<conjunction<
      std::is_constructible<value_type, const U&>,
      disjunction<
          std::is_same<std::remove_cv_t<T>, bool>,
          detail::never_constructible_or_convertible_from<value_type, optional<U>> >,
      std::is_convertible<const U&, T> // explicit(false)
  >::value, int> = 0>
  constexpr optional(const optional<U>& other) {
    if (other.has_value())
      this->construct_with(*other);
  }

  template<typename U, std::enable_if_t<conjunction<
      std::is_constructible<value_type, const U&>,
      disjunction<
          std::is_same<std::remove_cv_t<T>, bool>,
          detail::never_constructible_or_convertible_from<value_type, optional<U>> >,
      negation<std::is_convertible<const U&, T>> // explicit(true)
  >::value, int> = 0>
  constexpr explicit optional(const optional<U>& other) {
    if (other.has_value())
      this->construct_with(*other);
  }

  template<typename U, std::enable_if_t<conjunction<
      std::is_constructible<value_type, U&&>,
      disjunction<
          std::is_same<std::remove_cv_t<T>, bool>,
          detail::never_constructible_or_convertible_from<value_type, optional<U>> >,
      std::is_convertible<U&&, T> // explicit(false)
  >::value, int> = 0>
  constexpr optional(optional<U>&& other) {
    if (other.has_value())
      this->construct_with(std::move(*other));
  }

  template<typename U, std::enable_if_t<conjunction<
      std::is_constructible<value_type, U&&>,
      disjunction<
          std::is_same<std::remove_cv_t<T>, bool>,
          detail::never_constructible_or_convertible_from<value_type, optional<U>> >,
      negation<std::is_convertible<U&&, T>> // explicit(true)
  >::value, int> = 0>
  constexpr explicit optional(optional<U>&& other) {
    if (other.has_value())
      this->construct_with(std::move(*other));
  }

  // Split into 2 overloads to prevent MSVC from making an ambiguous call in C++14
  template<typename InPlaceT, std::enable_if_t<conjunction<
      std::is_same<InPlaceT, in_place_t>,
      std::is_constructible<value_type>
  >::value, int> = 0>
  constexpr explicit optional(InPlaceT)
    : base(in_place) {}

  template<typename Arg, typename ...Args, std::enable_if_t<
      std::is_constructible<value_type, Arg, Args...>::value,
  int> = 0>
  constexpr explicit optional(in_place_t, Arg&& arg, Args&&... args)
    : base(in_place, std::forward<Arg>(arg), std::forward<Args>(args)...) {}

  template<typename U, typename ...Args, std::enable_if_t<
      std::is_constructible<value_type, std::initializer_list<U>&, Args&&...>::value,
  int> = 0>
  constexpr explicit optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
    : base(in_place, ilist, std::forward<Args>(args)...) {}

  template<typename U = value_type, std::enable_if_t<conjunction<
      std::is_constructible<value_type,U&&>,
      negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
      negation<std::is_same<remove_cvref_t<U>, optional<T>>>,
      disjunction<
          negation< std::is_same<std::remove_cv_t<T>, bool> >,
          negation< is_specialization<remove_cvref_t<U>, optional> >
      >,
      std::is_convertible<U&&, value_type> // explicit(false)
  >::value, int> = 0>
  constexpr optional(U&& value)
    : base(in_place, std::forward<U>(value)) {}

  template<typename U = value_type, std::enable_if_t<conjunction<
      std::is_constructible<value_type,U&&>,
      negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
      negation<std::is_same<remove_cvref_t<U>, optional<T>>>,
      disjunction<
          negation< std::is_same<std::remove_cv_t<T>, bool> >,
          negation< is_specialization<remove_cvref_t<U>, optional> >
      >,
      negation<std::is_convertible<U&&, value_type>> // explicit(true)
  >::value, int> = 0>
  constexpr explicit optional(U&& value)
    : base(in_place, std::forward<U>(value)) {}

  // assignment operators

  optional& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  constexpr optional& operator=(optional const&) = default;
  constexpr optional& operator=(optional &&) = default;

  template<typename U = T, std::enable_if_t<conjunction<
      conjunction<
          negation< std::is_same<remove_cvref_t<U>, optional<T>> >,
          std::is_constructible<T, U>,
          std::is_assignable<T&, U>
      >,
      disjunction<
          negation< std::is_scalar<T> >,
          negation< std::is_same<std::decay_t<U>, T> >
      >
  >::value, int> = 0>
  optional& operator=(U&& value) {
    if (has_value()) {
      this->val = std::forward<U>(value);
    } else {
      this->construct_with(std::forward<U>(value));
    }
    return *this;
  }

  template<typename U, std::enable_if_t<conjunction<
      detail::never_constructible_or_convertible_from<T, optional<U>>,
      detail::never_assignable_from<T, optional<U>>,
      std::is_constructible<T , const U&>,
      std::is_assignable<T&, const U&>
  >::value, int> = 0>
  optional& operator=(const optional<U>& other) {
    if (other.has_value()) {
      if (this->has_value())
        this->val = *other;
      else
        this->construct_with(*other);
    } else { // !other.has_value()
      if (this->has_value())
        this->reset();
    }
    return *this;
  }

  template<typename U, std::enable_if_t<conjunction<
      detail::never_constructible_or_convertible_from<T, optional<U>>,
      detail::never_assignable_from<T, optional<U>>,
      std::is_constructible<T , U>,
      std::is_assignable<T&, U>
  >::value, int> = 0>
  optional& operator=(optional<U>&& other) {
    if (other.has_value()) {
      if (this->has_value())
        this->val = std::move(*other);
      else
        this->construct_with(std::move(*other));
    } else { // !other.has_value()
      if (this->has_value())
        this->reset();
    }
    return *this;
  }

  using base::operator->;
  using base::operator*;
  using base::operator bool;
  using base::has_value;
  using base::value;
  using base::reset;

  template<typename U, std::enable_if_t<conjunction<
      std::is_copy_constructible<T>,
      std::is_convertible<U&&, T>
  >::value, int> = 0>
  constexpr T value_or(U&& default_value) const & {
    return bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value));
  }

  template<typename U, std::enable_if_t<conjunction<
      std::is_move_constructible<T>,
      std::is_convertible<U&&, T>
  >::value, int> = 0>
  constexpr T value_or(U&& default_value) && {
    return bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, T&>, optional>
  ::value, int> = 0>
  constexpr auto and_then(F&& f) & {
    if (*this)
      return preview::invoke(std::forward<F>(f), **this);
    else
      return remove_cvref_t<invoke_result_t<F, T&>>{};
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, const T&>, optional>
  ::value, int> = 0>
  constexpr auto and_then(F&& f) const & {
    if (*this)
      return preview::invoke(std::forward<F>(f), **this);
    else
      return remove_cvref_t<invoke_result_t<F, const T&>>{};
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, T>, optional>
  ::value, int> = 0>
  constexpr auto and_then(F&& f) && {
    if (*this)
      return preview::invoke(std::forward<F>(f), std::move(**this));
    else
      return remove_cvref_t<invoke_result_t<F, T>>{};
  }

  template<typename F, std::enable_if_t<
      is_specialization<invoke_result_t<F, const T>, optional>
  ::value, int> = 0>
  constexpr auto and_then(F&& f) const && {
    if (*this)
      return preview::invoke(std::forward<F>(f), std::move(**this));
    else
      return remove_cvref_t<invoke_result_t<F, const T>>{};
  }

  template<typename F, std::enable_if_t<conjunction<
      is_invocable<F, T&>
  >::value, int> = 0>
  constexpr auto transform(F&& f) & {
    using U = std::remove_cv_t<invoke_result_t<F, T&>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), **this));
    else
      return optional<U>{};
  }

  template<typename F, std::enable_if_t<
      is_invocable<F, const T&>
  ::value, int> = 0>
  constexpr auto transform(F&& f) const & {
    using U = std::remove_cv_t<invoke_result_t<F, const T&>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), **this));
    else
      return optional<U>{};
  }

  template<typename F, std::enable_if_t<
      is_invocable<F, T>
  ::value, int> = 0>
  constexpr auto transform(F&& f) && {
    using U = std::remove_cv_t<invoke_result_t<F, T>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), std::move(**this)));
    else
      return optional<U>{};
  }

  template<typename F, std::enable_if_t<
      is_invocable<F, const T>
  ::value, int> = 0>
  constexpr auto transform(F&& f) const && {
    using U = std::remove_cv_t<invoke_result_t<F, const T>>;
    if (*this)
      return optional<U>(preview::invoke(std::forward<F>(f), std::move(**this)));
    else
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

  template<typename Dummy = void, std::enable_if_t<conjunction<
      std::is_void<Dummy>,
      is_swappable<T&>,
      std::is_move_constructible<T>
  >::value, int> = 0>
  void swap(optional& other)
      noexcept(std::is_nothrow_move_constructible<T>::value &&
               preview::is_nothrow_swappable<T>::value)
  {
    // TODO: Investigate exception safety guarantees
    if (other.has_value()) {
      if (this->has_value()) {
        using std::swap;
        swap(**this, *other);
      } else { // !this->has_value()
        this->construct_with(std::move(*other));
        other.reset();
      }
    } else {
      if (this->has_value()) {
        other.construct_with(std::move(**this));
        this->reset();
      }
    }
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<
      std::is_void<Dummy>,
      std::is_constructible<value_type>
  >::value, int> = 0>
  value_type& emplace() {
    this->reset();
    this->construct_with();
    return **this;
  }

  template<typename Arg, typename ...Args, std::enable_if_t<
      std::is_constructible<value_type, Arg, Args...>
  ::value, int> = 0>
  value_type& emplace(Arg&& arg, Args&&... args) {
    this->destruct();
    this->construct_with(std::forward<Arg>(arg), std::forward<Args>(args)...);
    return **this;
  }

  template<typename U, typename ...Args,
    std::enable_if_t<
      std::is_constructible<value_type, std::initializer_list<U>&, Args&&...>::value,
    int> = 0>
  value_type& emplace(std::initializer_list<U> ilist, Args&&... args) {
    this->destruct();
    this->construct_with(ilist, std::forward<Args>(args)...);
    return **this;
  }
};

# if PREVIEW_CXX_VERSION >= 17
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
constexpr inline optional<T> make_optional(std::initializer_list<U> il, Args&&... args) {
  return optional<T>(in_place, il, std::forward<Args>(args)...);
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
  return bool(opt) ? *opt != value : true;
}

template<typename T, typename U>
constexpr inline bool operator!=(const T& value, const optional<U>& opt) {
  return bool(opt) ? value != *opt : true;
}

template<typename T, typename U>
constexpr inline bool operator<(const optional<T>& opt, const U& value) {
  return bool(opt) ? *opt < value : true;
}

template<typename T, typename U>
constexpr inline bool operator<(const T& value, const optional<U>& opt) {
  return bool(opt) ? value < *opt : false;
}

template<typename T, typename U>
constexpr inline bool operator<=(const optional<T>& opt, const U& value) {
  return bool(opt) ? *opt <= value : true;
}

template<typename T, typename U>
constexpr inline bool operator<=(const T& value, const optional<U>& opt) {
  return bool(opt) ? value <= *opt : false;
}

template<typename T, typename U>
constexpr inline bool operator>(const optional<T>& opt, const U& value) {
  return bool(opt) ? *opt > value : false;
}

template<typename T, typename U>
constexpr inline bool operator>(const T& value, const optional<U>& opt) {
  return bool(opt) ? value > *opt : true;
}

template<typename T, typename U>
constexpr inline bool operator>=(const optional<T>& opt, const U& value) {
  return bool(opt) ? *opt >= value : false;
}

template<typename T, typename U>
constexpr inline bool operator>=(const T& value, const optional<U>& opt) {
  return bool(opt) ? value >= *opt : true;
}

} // namespace preview

# endif // PREVIEW_OPTIONAL_OPTIONAL_H_
