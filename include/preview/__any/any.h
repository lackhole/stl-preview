//
// Created by yonggyulee on 2024. 10. 2.
//

#ifndef PREVIEW_ANY_ANY_H_
#define PREVIEW_ANY_ANY_H_

#include "preview/core.h"

// Since any provides non-explicit constructors, constructing std::any with preview::any or vice versa
// usually leads runtime error that is hard to detect. To prevent this, preview::any is alias of std::any
// when C++17 or later is available.
// I could add a constructor that takes std::any and operator std::any(), but it would cause additional
// runtime overhead since any always copies the value.
//
// Bad example when preview::any still exists after C++17(disabled by default):
// preview::any a = 42;               // holds int
// int x = preview::any_cast<int>(a); // well-formed
// int y = std::any_cast<int>(a); // throws; preview::any is implicitly converted to std::any, which holds preview::any

#include "preview/__utility/in_place.h"

#if PREVIEW_CXX_VERSION < 17

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include "preview/byte.h"
#include "preview/__memory/construct_at.h"
#include "preview/__memory/destroy_at.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__utility/exchange.h"

#ifndef PREVIEW_ANY_SMALL_OBJECT_SIZE
#define PREVIEW_ANY_SMALL_OBJECT_SIZE 64
#endif

namespace preview {

class any;

namespace detail {

template<std::size_t BufferSize>
struct any_storage_t {
  static_assert(BufferSize % sizeof(void*) == 0, "Invalid buffer size");

#if PREVIEW_CXX_VERSION < 23
  constexpr any_storage_t() : ptr{} {}
#else
  constexpr any_storage() = default;
#endif

  template<typename T>
  using is_small_object = conjunction<
      bool_constant<(sizeof(T) <= BufferSize)>,
      std::is_nothrow_move_constructible<T>
  >;

  union {
    void* ptr;
    alignas(void*) preview::byte buffer[BufferSize];
  };
};

using any_storage = any_storage_t<PREVIEW_ANY_SMALL_OBJECT_SIZE>;

template<typename T>
struct any_small_handler;

template<typename T>
struct any_big_handler;

template<typename T>
using any_handler = std::conditional_t<
    any_storage::is_small_object<T>::value,
    any_small_handler<T>,
    any_big_handler<T>>;

enum class any_op {
  kTypeInfo,
  kCopy,
  kMove,
  kDestroy,
  kSwap,
};

union any_arg {
  PREVIEW_CONSTEXPR_AFTER_CXX23 any_arg() {}

  constexpr explicit any_arg(any* obj) : obj(obj) {}

  any* obj;
  const std::type_info* type_info;
};

} // namespace detail

class any {
  using any_vptr = void (*) (const any&, detail::any_op, detail::any_arg*);
  using any_arg = detail::any_arg;
  template<typename T>
  using handler = detail::any_handler<std::decay_t<T>>;

 public:
  constexpr any()
      : vptr_(nullptr) {}

  any(const any& other)
      : vptr_(nullptr)
  {
    if (other.has_value())
      other.copy_to(this);
  }

  any(any&& other) noexcept
      : vptr_(nullptr)
  {
    if (other.has_value())
      other.move_to(this);
  }

  template<typename T, std::enable_if_t<conjunction<
      negation< std::is_same<std::decay_t<T>, any> >,
      negation< is_specialization<std::decay_t<T>, in_place_type_t> >,
      std::is_copy_constructible<std::decay_t<T>>
  >::value, int> = 0>
  any(T&& value);

  ~any() {
    reset();
  }

  template<typename T, typename... Args, std::enable_if_t<conjunction<
      std::is_copy_constructible<std::decay_t<T>>,
      std::is_constructible<std::decay_t<T>, Args...>
  >::value, int> = 0>
  explicit any(in_place_type_t<T>, Args&&... args);

  template<typename T, typename U, typename... Args, std::enable_if_t<conjunction<
      std::is_copy_constructible<std::decay_t<T>>,
      std::is_constructible<std::decay_t<T>, std::initializer_list<U>&, Args...>
  >::value, int> = 0>
  explicit any(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args);

  any& operator=(const any& rhs) {
    any(rhs).swap(*this);
    return *this;
  }

  any& operator=(any&& rhs) noexcept {
    any(std::move(rhs)).swap(*this);
    return *this;
  }

  template<typename T, std::enable_if_t<conjunction<
      negation< std::is_same<std::decay_t<T>, any> >,
      std::is_copy_constructible<std::decay_t<T>>
  >::value, int> = 0>
  any& operator=(T&& rhs) {
    any(std::forward<T>(rhs)).swap(*this);
    return *this;
  }

  template<typename T, typename... Args, std::enable_if_t<conjunction<
      std::is_copy_constructible<std::decay_t<T>>,
      std::is_constructible<std::decay_t<T>, Args...>
  >::value, int> = 0>
  std::decay_t<T>& emplace(Args&&... args);

  template<typename T, typename U, typename... Args, std::enable_if_t<conjunction<
      std::is_copy_constructible<std::decay_t<T>>,
      std::is_constructible<std::decay_t<T>, std::initializer_list<U>&, Args...>
  >::value, int> = 0>
  std::decay_t<T>& emplace(std::initializer_list<U> il, Args&&... args);

  void reset() {
    if (has_value()) {
      call(detail::any_op::kDestroy);
    }
  }

  void swap(any& rhs) noexcept {
    if (has_value()) {
      any_arg arg{&rhs};
      call(detail::any_op::kSwap, &arg);
    } else if (rhs.has_value()) {
      any_arg arg{this};
      rhs.call(detail::any_op::kSwap, &arg);
    }
  }

  bool has_value() const noexcept {
    return vptr_ != nullptr;
  }

  const std::type_info& type() const noexcept {
    if (has_value()) {
      any_arg arg;
      call(detail::any_op::kTypeInfo, &arg);
      return *arg.type_info;
    } else {
      return typeid(void);
    }
  }

 private:
  template<typename T> friend struct detail::any_big_handler;
  template<typename T> friend struct detail::any_small_handler;

  template<typename T>
  friend const T* any_cast(const any* operand) noexcept;
  template<typename T>
  friend T* any_cast(any* operand) noexcept;

  void call(detail::any_op op, detail::any_arg* arg = nullptr)       { vptr_(*this, op, arg); }

  void call(detail::any_op op, detail::any_arg* arg = nullptr) const { vptr_(*this, op, arg); }

  void copy_to(any* other) const {
    any_arg arg{other};
    call(detail::any_op::kCopy, &arg);
  }

  void move_to(any* other) {
    any_arg arg{other};
    call(detail::any_op::kMove, &arg);
  }

  template<typename T>
  const T* cast() const noexcept {
    return cast_impl<T>(typename detail::any_storage::is_small_object<T>{});
  }

  template<typename T>
  T* cast() noexcept {
    return const_cast<T*>(const_cast<const any*>(this)->cast<T>());
  }

  template<typename T>
  const T* cast_impl(std::true_type /* is_small_object<T> */) const noexcept {
    return reinterpret_cast<const T*>(storage_.buffer);
  }
  template<typename T>
  const T* cast_impl(std::false_type /* is_small_object<T> */) const noexcept {
    return static_cast<const T*>(storage_.ptr);
  }

  any_vptr vptr_;
  detail::any_storage storage_;
};

namespace detail {

template<typename T>
struct any_small_handler {
  static void handle(const any& self, any_op op, any_arg* arg) {
    switch (op) {
      case any_op::kTypeInfo:
        arg->type_info = any_small_handler<T>::type_info();
        break;
      case any_op::kCopy:
        any_small_handler<T>::copy(self, arg->obj);
        break;
      case any_op::kMove:
        any_small_handler<T>::move(const_cast<any&>(self), arg->obj);
        break;
      case any_op::kDestroy:
        any_small_handler<T>::destroy(const_cast<any&>(self));
        break;
      case any_op::kSwap:
        any_small_handler<T>::swap(const_cast<any&>(self), *arg->obj);
        break;
    }
  }

  template<typename... Args>
  static T& create(any* thiz, Args&&... args) {
    T* ptr = any_small_handler<T>::get_ptr(*thiz);
    preview::construct_at<T>(ptr, std::forward<Args>(args)...);
    thiz->vptr_ = &any_small_handler<T>::handle;
    return *ptr;
  }

 private:
  using pointer = T*;
  using const_pointer = std::add_const_t<T>*;

  static pointer       get_ptr(      any& self) noexcept { return reinterpret_cast<pointer      >(self.storage_.buffer); }
  static const_pointer get_ptr(const any& self) noexcept { return reinterpret_cast<const_pointer>(self.storage_.buffer); }

  static const std::type_info* type_info() {
    return &typeid(T);
  }

  static void copy(const any& self, any* other) {
    any_small_handler<T>::create(other, *any_small_handler<T>::get_ptr(self));
  }

  static void move(any& self, any* other) {
    any_small_handler<T>::create(other, std::move(*any_small_handler<T>::get_ptr(self)));
    any_small_handler<T>::destroy(self);
  }

  static void destroy(any& self) {
    preview::destroy_at(any_small_handler<T>::get_ptr(self));
    self.vptr_ = nullptr;
  }

  static void swap(any& self, any& other) {
    // self always has value
    if (other.has_value()) {
      any temp;
      any_small_handler<T>::move(self, &temp);
      {
        any_arg arg{&self};
        other.call(any_op::kMove, &arg);
      }
      any_small_handler<T>::move(temp, &other);
    } else {
      any_small_handler<T>::move(self, &other);
    }
  }
};

template<typename T>
struct any_big_handler {
  static void handle(const any& self, any_op op, any_arg* arg) {
    switch (op) {
      case any_op::kTypeInfo:
        arg->type_info = any_big_handler<T>::type_info();
        break;
      case any_op::kCopy:
        any_big_handler<T>::copy(self, arg->obj);
        break;
      case any_op::kMove:
        any_big_handler<T>::move(const_cast<any&>(self), arg->obj);
        break;
      case any_op::kDestroy:
        any_big_handler<T>::destroy(const_cast<any&>(self));
        break;
      case any_op::kSwap:
        any_big_handler<T>::swap(const_cast<any&>(self), *arg->obj);
        break;
    }
  }

  template<typename... Args>
  static T& create(any* thiz, Args&&... args) {
    any_big_handler<T>::get_ptr(*thiz) = ::new T(std::forward<Args>(args)...);
    thiz->vptr_ = &any_big_handler<T>::handle;
    return *any_big_handler<T>::get_ptr(*thiz);
  }

 private:
  using pointer = T*;
  using const_pointer = std::add_const_t<T>*;

  static pointer       get_ptr(      any& self) noexcept { return static_cast<pointer      >(self.storage_.ptr); }
  static const_pointer get_ptr(const any& self) noexcept { return static_cast<const_pointer>(self.storage_.ptr); }

  static const std::type_info* type_info() {
    return &typeid(T);
  }

  static void copy(const any& self, any* other) {
    any_big_handler<T>::create(other, *any_big_handler<T>::get_ptr(self));
  }

  static void move(any& self, any* other) {
    any_big_handler<T>::get_ptr(*other) = any_big_handler<T>::get_ptr(self);
    other->vptr_ = self.vptr_;
    self.vptr_ = nullptr;
  }

  static void destroy(any& self) {
    ::delete any_big_handler<T>::get_ptr(self);
    self.vptr_ = nullptr;
  }

  static void swap(any& lhs, any& rhs) {
    preview::exchange(any_big_handler<T>::get_ptr(lhs), any_big_handler<T>::get_ptr(rhs));
    preview::exchange(lhs.vptr_, rhs.vptr_);
  }
};

} // namespace detail

template<typename T, std::enable_if_t<conjunction<
    negation<std::is_same<std::decay_t<T>, any>>,
    negation<is_specialization<std::decay_t<T>, in_place_type_t>>,
    std::is_copy_constructible<std::decay_t<T>>
>::value, int>>
any::any(T&& value)
    : vptr_(nullptr)
{
  handler<T>::create(this, std::forward<T>(value));
}


template<typename T, typename... Args, std::enable_if_t<conjunction<
    std::is_copy_constructible<std::decay_t<T>>,
    std::is_constructible<std::decay_t<T>, Args...>
>::value, int>>
any::any(in_place_type_t<T>, Args&&... args)
    : vptr_(nullptr)
{
  handler<T>::create(this, std::forward<Args>(args)...);
}

template<typename T, typename U, typename... Args, std::enable_if_t<conjunction<
    std::is_copy_constructible<std::decay_t<T>>,
    std::is_constructible<std::decay_t<T>, std::initializer_list<U>&, Args...>
>::value, int>>
any::any(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
    : vptr_(nullptr)
{
  handler<T>::create(this, il, std::forward<Args>(args)...);
}

template<typename T, typename... Args, std::enable_if_t<conjunction<
    std::is_copy_constructible<std::decay_t<T>>,
    std::is_constructible<std::decay_t<T>, Args...>
>::value, int>>
std::decay_t<T>& any::emplace(Args&&... args) {
  reset();
  return handler<T>::create(this, std::forward<Args>(args)...);
}

template<typename T, typename U, typename... Args, std::enable_if_t<conjunction<
    std::is_copy_constructible<std::decay_t<T>>,
    std::is_constructible<std::decay_t<T>, std::initializer_list<U>&, Args...>
>::value, int>>
std::decay_t<T>& any::emplace(std::initializer_list<U> il, Args&&... args) {
  reset();
  return handler<T>::create(this, il, std::forward<Args>(args)...);
}

void swap(any& x, any& y) noexcept {
  x.swap(y);
}

template<typename T, typename... Args>
any make_any(Args&&... args) {
  return any(in_place_type<T>, std::forward<Args>(args)...);
}

template<typename T, typename U, typename... Args>
any make_any(std::initializer_list<U> il, Args&&... args) {
  return any(in_place_type<T>, il, std::forward<Args>(args)...);
}

} // namespace preview

#else

#include <any>

namespace preview {

using std::any;
using std::make_any;

} // namespace preview

#endif

#endif // PREVIEW_ANY_ANY_H_
