//
// Created by yonggyulee on 2024. 8. 10.
//

#ifndef PREVIEW_TYPE_TRAITS_OVERRIDE_CVREF_H_
#define PREVIEW_TYPE_TRAITS_OVERRIDE_CVREF_H_

#include <type_traits>

#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/copy_cvref.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {

template<typename From, typename To>
struct override_reference {
  using RawTo = std::remove_reference_t<To>;
  static_assert(is_referencable<RawTo>::value, "Constraints not satisfied");

 public:
  using type = conditional_t<
      std::is_rvalue_reference<From>, RawTo&&,
      std::is_lvalue_reference<From>, RawTo&,
      RawTo
  >;
};

template<typename From, typename To>
using override_reference_t = typename override_reference<From, To>::type;

template<typename From, typename To>
using override_cvref_t = override_reference_t<From, copy_cv_t<From, To>>;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_OVERRIDE_CVREF_H_
