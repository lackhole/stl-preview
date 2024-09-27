//
// Created by yonggyulee on 2023/10/27.
//

#ifndef PREVIEW_TYPE_TRAITS_HAS_TYPENAME_TYPE_H_
#define PREVIEW_TYPE_TRAITS_HAS_TYPENAME_TYPE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/meta.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/type_sequence.h"

namespace preview {

template<typename T, template<typename...> class Constraint = always_true, typename CallArgs = type_sequence<>,
         template<typename, typename...> class Proj = type_identity_t, typename = void>
struct has_typename_type : std::false_type {};

template<typename T, template<typename...> class Constraint, typename CallArgs, template<typename, typename...> class Proj>
struct has_typename_type<T, Constraint, CallArgs, Proj, void_t<typename T::type>>
    : meta::bind_first<Constraint, Proj<typename T::type>, CallArgs> {};

template<typename T, template<typename...> class Constraint, typename CallArgs = type_sequence<>,
         template<typename, typename...> class Proj = type_identity_t>
PREVIEW_INLINE_VARIABLE constexpr bool has_typename_type_v = has_typename_type<T, Constraint, CallArgs, Proj>::value;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_HAS_TYPENAME_TYPE_H_
