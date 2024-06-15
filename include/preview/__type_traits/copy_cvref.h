//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_COPY_CVREF_H_
#define PREVIEW_TYPE_TRAITS_COPY_CVREF_H_

#include <type_traits>

namespace preview {

template<typename From, typename To>
struct copy_const {
  using type = To;
};

template<typename From, typename To>
struct copy_const<const From, To> {
  using type = std::add_const_t<To>;
};

template<typename From, typename To>
using copy_const_t = typename copy_const<From, To>::type;

template<typename From, typename To>
struct copy_volatile {
  using type = To;
};

template<typename From, typename To>
struct copy_volatile<volatile From, To> {
  using type = std::add_volatile_t<To>;
};

template<typename From, typename To>
using copy_volatile_t = typename copy_volatile<From, To>::type;

template<typename From, typename To>
struct copy_cv {
  using type = copy_volatile_t<From, copy_const_t<From, To>>;
};

template<typename From, typename To>
using copy_cv_t = typename copy_cv<From, To>::type;

template<typename From, typename To>
struct copy_reference {
  using type = To;
};

template<typename From, typename To>
struct copy_reference<From&, To> {
  using type = std::add_lvalue_reference_t<To>;
};

template<typename From, typename To>
struct copy_reference<From&&, To> {
  using type = std::add_rvalue_reference_t<To>;
};

template<typename From, typename To>
using copy_reference_t = typename copy_reference<From, To>::type;

template<typename From, typename To>
struct copy_cvref {
  using type = copy_reference_t<From, copy_cv_t<From, To>>;
};

template<typename From, typename To>
using copy_cvref_t = typename copy_cvref<From, To>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_COPY_CVREF_H_
