//
// Created by yonggyulee on 2024-09-10
//

#ifndef PREVIEW_TYPE_TRAITS_IS_PRE_INCREMENTABLE_H_
#define PREVIEW_TYPE_TRAITS_IS_PRE_INCREMENTABLE_H_

#include <type_traits>

#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/meta.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/type_sequence.h"

namespace preview {

template<typename T, template<typename...> class Constraint = always_true,
         typename Args = type_sequence<>, template<typename, typename...> class Proj = type_identity_t, typename = void>
struct is_pre_incrementable : std::false_type {};

template<typename T, template<typename...> class Constraint,
         typename CallArgs, template<typename, typename...> class Proj>
struct is_pre_incrementable<T, Constraint, CallArgs, Proj, void_t<decltype(++std::declval<T>())>>
    : meta::bind_first<Constraint, Proj<decltype(++std::declval<T>())>, CallArgs> {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_IS_PRE_INCREMENTABLE_H_
