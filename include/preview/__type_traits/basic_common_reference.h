//
// Created by yonggyulee on 2024. 8. 11.
//

#ifndef PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_H_

#include <tuple>

#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/common_reference.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/no_traits.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace detail {

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual,
         bool = conjunction<tuple_like<T>, tuple_like<U>>::value /* false */>
struct basic_common_reference_tuple_like : no_traits {};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, typename Index>
struct basic_common_reference_tuple_like_impl_2;

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, std::size_t... I>
struct basic_common_reference_tuple_like_impl_2<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>> {
  using type = std::tuple<
      common_reference_t<
          TQual< std::tuple_element_t<I, TTuple> >,
          UQual< std::tuple_element_t<I, UTuple> >
      >... >;
};

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, typename Index>
struct basic_common_reference_tuple_like_impl_1;

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, std::size_t... I>
struct basic_common_reference_tuple_like_impl_1<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>>
    : std::conditional_t<
        conjunction<
            has_typename_type< common_reference<TQual<std::tuple_element_t<I, TTuple>>,
                                                UQual<std::tuple_element_t<I, UTuple>>> >...
        >::value,
        basic_common_reference_tuple_like_impl_2<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>>,
        no_traits
    > {};


template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference_tuple_like<TTuple, UTuple, TQual, UQual, true>
    : std::conditional_t<
        conjunction<
            disjunction< is_specialization<TTuple, std::tuple>, is_specialization<UTuple, std::tuple> >,
            std::is_same<TTuple, std::decay_t<TTuple>>,
            std::is_same<UTuple, std::decay_t<UTuple>>,
            bool_constant<(std::tuple_size<remove_cvref_t<TTuple>>::value == std::tuple_size<remove_cvref_t<UTuple>>::value)>
        >::value,
        basic_common_reference_tuple_like_impl_1<TTuple, UTuple, TQual, UQual, std::make_index_sequence<std::tuple_size<remove_cvref_t<TTuple>>::value>>,
        no_traits
    > {};

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual, bool /* false */>
struct basic_common_reference_pair {};

template<typename T1, typename T2, typename U1, typename U2, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference_pair<std::pair<T1, T2>, std::pair<U1, U2>, TQual, UQual, true> {
  using type = std::pair< common_reference_t<TQual<T1>, UQual<U1>>,
                          common_reference_t<TQual<T2>, UQual<U2>> >;
};

template<typename R, typename T, typename RQ, typename TQ,
         bool = conjunction<
             negation< is_specialization<T, std::reference_wrapper> >,
             has_typename_type< common_reference<R&, TQ> >
         >::value /* false */>
struct basic_common_reference_ref_wrap {};

template<typename R, typename T, typename RQ, typename TQ>
struct basic_common_reference_ref_wrap<R, T, RQ, TQ, true>
    : std::conditional_t<
        convertible_to<RQ, common_reference_t<R&, TQ>>::value,
        type_identity<common_reference_t<R&, TQ>>,
        no_traits
    > {};

} // namespace detail

// Primary template
template<typename T, typename U, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference : detail::basic_common_reference_tuple_like<T, U, TQual, UQual> {};

// specializations of basic_common_reference
template<typename T1, typename T2, typename U1, typename U2,
    template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference<std::pair<T1, T2>, std::pair<U1, U2>, TQual, UQual>
    : detail::basic_common_reference_pair<
        std::pair<T1, T2>, std::pair<U1, U2>,
        TQual, UQual,
        conjunction<
            has_typename_type< common_reference< TQual<T1>, UQual<U1> > >,
            has_typename_type< common_reference< TQual<T2>, UQual<U2> > >
        >::value
    > {};

template<typename R, typename T, template<typename> class RQual, template<typename> class TQual>
struct basic_common_reference<std::reference_wrapper<R>, T, RQual, TQual>
    : detail::basic_common_reference_ref_wrap<R, T, RQual<R>, TQual<T>> {};

template<typename R, typename T, template<typename> class RQual, template<typename> class TQual>
struct basic_common_reference<T, std::reference_wrapper<R>, TQual, RQual>
    : detail::basic_common_reference_ref_wrap<R, T, RQual<R>, TQual<T>> {};


} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_BASIC_COMMON_REFERENCE_H_
