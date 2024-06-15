//
// Created by cosge on 2023-12-03.
//

#ifndef PREVIEW_TYPE_TRAITS_COMMON_REFERENCE_H_
#define PREVIEW_TYPE_TRAITS_COMMON_REFERENCE_H_

#include <functional>
#include <type_traits>
#include <tuple>
#include <utility>

#include "preview/__concepts/convertible_to.h"
#include "preview/__tuple/tuple_like.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/copy_cvref.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__type_traits/simple_common_reference.h"
#include "preview/__type_traits/type_identity.h"

namespace preview {
namespace detail {

struct no_common_reference {};

template<typename T, typename U,
         template<typename> class TQual, template<typename> class UQual,
         bool = conjunction<tuple_like<T>, tuple_like<U>>::value /* false */>
struct basic_common_reference_tuple_like : no_common_reference {};

}


template<typename...>
struct common_reference;

template<typename... T>
using common_reference_t = typename common_reference<T...>::type;

template<typename T, typename U, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference : detail::basic_common_reference_tuple_like<T, U, TQual, UQual> {};

namespace impl {

template<typename T>
struct basic_common_reference_qual_gen {
  template<typename U>
  using qual = copy_cvref_t<T, U>;
};

template<typename T1, typename T2>
struct common_reference_tag_1 : has_typename_type<simple_common_reference<T1, T2>> {};

template<typename T1, typename T2>
struct common_reference_tag_2
    : has_typename_type<
        basic_common_reference<
          remove_cvref_t<T1>,
          remove_cvref_t<T2>,
          basic_common_reference_qual_gen<T1>::template qual,
          basic_common_reference_qual_gen<T2>::template qual
        >
      > {};

template<typename T> T common_reference_val();

template<typename T1, typename T2, typename = void>
struct common_reference_tag_3 : std::false_type {};

template<typename T1, typename T2>
struct common_reference_tag_3<
    T1,
    T2,
    void_t<decltype( false ? common_reference_val<T1>() : common_reference_val<T2>() )>
    > : std::true_type {};

template<typename T1, typename T2, typename = void>
struct common_reference_tag_4 : std::false_type {};

template<typename T1, typename T2>
struct common_reference_tag_4<T1, T2, void_t<std::common_type_t<T1, T2>>> : std::true_type {};

template<typename T1, typename T2>
struct common_reference_tag
    : std::conditional_t<
        common_reference_tag_1<T1, T2>::value, std::integral_constant<int, 1>,
      std::conditional_t<
        common_reference_tag_2<T1, T2>::value, std::integral_constant<int, 2>,
      std::conditional_t<
        common_reference_tag_3<T1, T2>::value, std::integral_constant<int, 3>,
      std::conditional_t<
        common_reference_tag_4<T1, T2>::value, std::integral_constant<int, 4>,
        std::integral_constant<int, 0>
      >>>> {};


template<typename T1, typename T2, int = common_reference_tag<T1, T2>::value>
struct common_reference_two;

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 0> {}; // no common reference type

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 1> {
  using type = typename simple_common_reference<T1, T2>::type;
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 2> {
  using type = typename basic_common_reference<
      remove_cvref_t<T1>,
      remove_cvref_t<T2>,
      basic_common_reference_qual_gen<T1>::template qual,
      basic_common_reference_qual_gen<T2>::template qual>::type;
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 3> {
  using type = decltype( false ? common_reference_val<T1>() : common_reference_val<T2>() );
};

template<typename T1, typename T2>
struct common_reference_two<T1, T2, 4> {
  using type = std::common_type_t<T1, T2>;
};

template<bool /*true*/, typename T1, typename T2, typename... R>
struct common_reference_three : common_reference<common_reference_t<T1, T2>, R...> {};

template<typename T1, typename T2, typename... R>
struct common_reference_three<false, T1, T2, R...> {};

} // namespace impl

template<>
struct common_reference<> {};

template<typename T>
struct common_reference<T> {
  using type = T;
};

template<typename T1, typename T2>
struct common_reference<T1, T2> : impl::common_reference_two<T1, T2> {};

template<typename T1, typename T2, typename... R>
struct common_reference<T1, T2, R...>
    : impl::common_reference_three<
        has_typename_type<common_reference<T1, T2>>::value,
        T1,
        T2,
        R...
      > {};

// specializations of basic_common_reference

namespace detail {

template<typename TTuple, typename UTuple, template<typename> class TQual, template<typename> class UQual, typename Index>
struct basic_common_reference_tuple_like_impl_2;

template<
    typename TTuple, typename UTuple,
    template<typename> class TQual, template<typename> class UQual,
    std::size_t... I
>
struct basic_common_reference_tuple_like_impl_2<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>> {
  using type = std::tuple<
      common_reference_t<
          TQual< std::tuple_element_t<I, TTuple> >,
          UQual< std::tuple_element_t<I, UTuple> >
      >... >;
};


template<
    typename TTuple, typename UTuple,
    template<typename> class TQual, template<typename> class UQual,
    typename Index
>
struct basic_common_reference_tuple_like_impl_1;

template<
    typename TTuple, typename UTuple,
    template<typename> class TQual, template<typename> class UQual,
    std::size_t... I
>
struct basic_common_reference_tuple_like_impl_1<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>>
    : std::conditional_t<
          conjunction<
              has_typename_type< common_reference<TQual<std::tuple_element_t<I, TTuple>>,
                                                  UQual<std::tuple_element_t<I, UTuple>>> >...
          >::value,
          basic_common_reference_tuple_like_impl_2<TTuple, UTuple, TQual, UQual, std::index_sequence<I...>>,
          no_common_reference
      >{};


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
          no_common_reference
      >{};

template<typename T, typename U,
         template<typename> class TQual, template<typename> class UQual,
         bool /* false */>
struct basic_common_reference_pair {};

template<typename T1, typename T2, typename U1, typename U2,
         template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference_pair<std::pair<T1, T2>, std::pair<U1, U2>, TQual, UQual, true> {
  using type = std::pair< common_reference_t<TQual<T1>, UQual<U1>>,
                          common_reference_t<TQual<T2>, UQual<U2>> >;
};

template<
    typename R, typename T, typename RQ, typename TQ,
    bool =
        conjunction<
            negation< is_specialization<T, std::reference_wrapper> >,
            has_typename_type< common_reference<R&, TQ> >
        >::value /* false */
>
struct basic_common_reference_ref_wrap {};

template<
    typename R, typename T, typename RQ, typename TQ
>
struct basic_common_reference_ref_wrap<R, T, RQ, TQ, true>
    : std::conditional_t<
          convertible_to<RQ, common_reference_t<R&, TQ>>::value,
          type_identity<common_reference_t<R&, TQ>>,
          no_common_reference
      > {};

} // namespace detail

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

#endif // PREVIEW_TYPE_TRAITS_COMMON_REFERENCE_H_
