//
// Created by yonggyulee on 2023/12/29.
//

#ifndef PREVIEW_ITERATOR_INDIRECT_RESULT_T_H_
#define PREVIEW_ITERATOR_INDIRECT_RESULT_T_H_

#include "preview/__concepts/invocable.h"
#include "preview/__functional/invoke.h"
#include "preview/__iterator/indirectly_readable.h"
#include "preview/__iterator/iter_reference_t.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_typename_type.h"

namespace preview {
namespace detail {

template<bool /* true */, typename F, typename... Is>
struct indirect_result_impl_2 {
  using type = invoke_result_t<F, iter_reference_t<Is>...>;
};

template<typename F, typename... Is>
struct indirect_result_impl_2<false, F, Is...> {};

template<bool /* true */, typename F, typename... Is>
struct indirect_result_impl_1
    : indirect_result_impl_2<
          invocable<F, iter_reference_t<Is>...>::value,
          F,
          Is...
      > {};

template<typename F, typename... Is>
struct indirect_result_impl_1<false, F, Is...> {};

} // namespace detail

template<typename F, typename... Is>
struct indirect_result
    : detail::indirect_result_impl_1<
          conjunction<
            indirectly_readable<Is>...,
            has_typename_type<iter_reference<Is>>...
          >::value,
          F,
          Is...
      > {};

template<typename F, typename... Is>
using indirect_result_t = typename indirect_result<F, Is...>::type;

} // namespace preview

#endif // PREVIEW_ITERATOR_INDIRECT_RESULT_T_H_
