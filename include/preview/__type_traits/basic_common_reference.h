//
// Created by yonggyulee on 2024. 8. 11.
//

#ifndef PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__functional/is_reference_wrapper.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace detail {
namespace bcr_specialization {

// TODO: Concept
// true specialization is defined in basic_common_reference_std_specializations.h
template<class R, class T, class RQ, class TQ, bool = /* false */ is_reference_wrapper<R>::value>
struct ref_wrap_common_reference_exists_with : std::false_type {};

enum category_t : int {
  kDefault = 0,
  kTupleLike = 1,
  kPair = 2,
  kReferenceWrapper = 3,
};

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual,
         bool SatisfyRequires = /* false */ conjunction_v<tuple_like<T>, tuple_like<U>>>
struct satisfy_tuple_like_constraints : std::false_type {};

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual>
struct satisfy_pair_constraints : std::false_type {};

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual,
         bool SatisfyRequires = /* false */(
                (ref_wrap_common_reference_exists_with<T, U, TQual<T>, UQual<U>>::value &&
                 !ref_wrap_common_reference_exists_with<U, T, UQual<U>, TQual<T>>::value) ||
                (ref_wrap_common_reference_exists_with<U, T, UQual<U>, TQual<T>>::value &&
                 !ref_wrap_common_reference_exists_with<T, U, TQual<T>, UQual<U>>::value)
        )>
struct satisfy_ref_wrap_constraints : bool_constant<SatisfyRequires> {};

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual>
PREVIEW_INLINE_VARIABLE constexpr int category = preview::conditional_t<
    satisfy_tuple_like_constraints      <T, U, TQual, UQual>, tag_v<kTupleLike>,
    satisfy_pair_constraints            <T, U, TQual, UQual>, tag_v<kPair>,
    satisfy_ref_wrap_constraints        <T, U, TQual, UQual>, tag_v<kReferenceWrapper>,
    tag_v<kDefault>
>::value;

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual, int Tag>
struct basic_common_reference_base;

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference_base<T, U, TQual, UQual, kDefault> {};

} // namespace bcr_specialization
} // namespace detail

// Primary template
template<typename T, typename U, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference
    : detail::bcr_specialization::basic_common_reference_base<
        T, U,
        TQual, UQual,
        detail::bcr_specialization::category<T, U, TQual, UQual>
    > {};

} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_H_
