//
// Created by yonggyulee on 2024. 8. 10.
//

#ifndef PREVIEW_TYPE_TRAITS_TYPENAME_TYPE_OR_H_
#define PREVIEW_TYPE_TRAITS_TYPENAME_TYPE_OR_H_

#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<typename MaybeHaveType, typename Alternative, bool = /* true */ has_typename_type<MaybeHaveType>::value>
struct typename_type_or_impl {
  using type = typename MaybeHaveType::type;
};

template<typename MaybeHaveType, typename Alternative>
struct typename_type_or_impl<MaybeHaveType, Alternative, false> {
  using type = typename Alternative::type;
};

template<typename MaybeHaveType, typename Alternative, bool = /* true */ has_typename_type<MaybeHaveType>::value>
struct typename_type_or_impl_inherit : MaybeHaveType {};

template<typename MaybeHaveType, typename Alternative>
struct typename_type_or_impl_inherit<MaybeHaveType, Alternative, false> : Alternative {};

} // namespace detail

// Define member typedef `type` equal to `MaybeHaveType::type` if exists, `Alternative::type` otherwise(SFINAE-friendly)
template<typename MaybeHaveType, typename Alternative, bool inherit = false>
struct typename_type_or
    : std::conditional_t<
        inherit,
        detail::typename_type_or_impl_inherit<MaybeHaveType, Alternative>,
        detail::typename_type_or_impl<MaybeHaveType, Alternative>
    > {};

template<typename MaybeHaveType, typename Alternative, bool inherit = false>
using typename_type_or_t = typename typename_type_or<MaybeHaveType, Alternative, inherit>::type;

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_TYPENAME_TYPE_OR_H_
