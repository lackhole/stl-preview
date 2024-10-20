//
// Created by yonggyulee on 2024. 10. 17.
//

#ifndef PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_STD_SPECIALIZATIONS_H_
#define PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_STD_SPECIALIZATIONS_H_

#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__concepts/convertible_to.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__tuple/tuple_integer_sequence.h"
#include "preview/__type_traits/basic_common_reference.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_specialization.h"

namespace preview {
namespace detail {
namespace bcr_specialization {

// basic_common_reference<tuple-like>

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, typename IndexSequence>
struct satisfy_tuple_like_constraints_impl_3;
template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, std::size_t... I>
struct satisfy_tuple_like_constraints_impl_3<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>> : conjunction<
    has_typename_type<
        common_reference<TQual<std::tuple_element_t<I, TTuple>>,
                         UQual<std::tuple_element_t<I, UTuple>>>
    >...
> {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual,
         bool = /* false */ (std::tuple_size<remove_cvref_t<TTuple>>::value == std::tuple_size<remove_cvref_t<UTuple>>::value)>
struct satisfy_tuple_like_constraints_impl_2 : std::false_type {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual>
struct satisfy_tuple_like_constraints_impl_2<TTuple, UTuple, TQual, UQual, true>
    : satisfy_tuple_like_constraints_impl_3<TTuple, UTuple, TQual, UQual, tuple_index_sequence<TTuple>> {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual,
         bool = /* false */ conjunction_v<
             disjunction<is_specialization<TTuple, std::tuple>, is_specialization<UTuple, std::tuple>>,
             std::is_same<TTuple, std::decay_t<TTuple>>,
             std::is_same<UTuple, std::decay_t<UTuple>>
         >
>
struct satisfy_tuple_like_constraints_impl : std::false_type {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual>
struct satisfy_tuple_like_constraints_impl<TTuple, UTuple, TQual, UQual, true>
    : satisfy_tuple_like_constraints_impl_2<TTuple, UTuple, TQual, UQual> {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual>
struct satisfy_tuple_like_constraints<TTuple, UTuple, TQual, UQual, true>
    : satisfy_tuple_like_constraints_impl<TTuple, UTuple, TQual, UQual> {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, typename ISeq>
struct basic_common_reference_base_tuple_like;

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, std::size_t... I>
struct basic_common_reference_base_tuple_like<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>> {
  using type = std::tuple<
      common_reference_t<TQual<std::tuple_element_t<I, TTuple>>,
                         UQual<std::tuple_element_t<I, UTuple>>>
      ...
  >;
};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference_base<TTuple, UTuple, TQual, UQual, kTupleLike>
    : basic_common_reference_base_tuple_like<TTuple, UTuple, TQual, UQual, tuple_index_sequence<TTuple>> {};



// basic_const_reference<pair>

template<typename T1, typename T2, typename U1, typename U2, template<typename> class TQual, template<typename> class UQual>
struct satisfy_pair_constraints<std::pair<T1, T2>, std::pair<U1, U2>, TQual, UQual> : conjunction<
    has_typename_type<common_reference<TQual<T1>, UQual<U1>>>,
    has_typename_type<common_reference<TQual<T2>, UQual<U2>>>
> {};

template<typename T1, typename T2, typename U1, typename U2, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference_base<std::pair<T1, T2>, std::pair<U1, U2>, TQual, UQual, kPair> {
  using type = std::pair<common_reference_t<T1, U1>, common_reference_t<T2, U2>>;
};



// basic_const_reference<std::reference_wrapper>
// basic_const_reference<preview::reference_wrapper>

template<class R, class T, class RQ, class TQ, bool = /* false */ has_typename_type<common_reference<typename R::type&, TQ>>::value>
struct ref_wrap_common_reference_exists_with_impl : std::false_type {};

template<class R, class T, class RQ, class TQ>
struct ref_wrap_common_reference_exists_with_impl<R, T, RQ, TQ, true>
    : convertible_to<RQ, common_reference_t<typename R::type&, TQ>> {};

template<class R, class T, class RQ, class TQ>
struct ref_wrap_common_reference_exists_with<R, T, RQ, TQ, true>
    : ref_wrap_common_reference_exists_with_impl<R, T, RQ, TQ> {};

template<class R, class T, template<class> class RQual, template<class> class TQual>
struct basic_common_reference_ref_wrap_t {
  using type = common_reference_t<typename R::type&, TQual<T>>;
};

template<class T, class U, template<class> class TQual, template<class> class UQual>
using basic_common_reference_ref_wrap = std::conditional_t<
    (ref_wrap_common_reference_exists_with<T, U, TQual<T>, UQual<U>>::value &&
     !ref_wrap_common_reference_exists_with<U, T, UQual<U>, TQual<T>>::value),
    basic_common_reference_ref_wrap_t<T, U, TQual, UQual>,
    basic_common_reference_ref_wrap_t<U, T, UQual, TQual>
>;

template<class T, class U, template<class> class TQual, template<class> class UQual>
struct basic_common_reference_base<T, U, TQual, UQual, kReferenceWrapper>
    : basic_common_reference_ref_wrap<T, U, TQual, UQual> {};

} // namespace bcr_specialization
} // namespace detail
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_STD_SPECIALIZATIONS_H_
