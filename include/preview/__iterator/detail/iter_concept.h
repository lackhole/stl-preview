//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_DETAIL_ITER_CONCEPT_H_
#define PREVIEW_ITERATOR_DETAIL_ITER_CONCEPT_H_

#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/iterator_tag.h"

namespace preview {
namespace detail {

template<typename I>
struct ITER_TRAITS_T {
  using type = std::conditional_t<
      is_primary_iterator_traits<iterator_traits<I> >::value,
      I,
      iterator_traits<I>>;
};

template<typename I>
using ITER_TRAITS = typename ITER_TRAITS_T<I>::type;


template<typename I, bool = is_primary_iterator_traits<iterator_traits<I> >::value /* false */>
struct ITER_CONCEPT_IMPL_3 {};
template<typename I>
struct ITER_CONCEPT_IMPL_3<I, true> {
  using type = random_access_iterator_tag;
};

template<typename I, bool = has_typename_iterator_category<ITER_TRAITS<I>>::value /* false */>
struct ITER_CONCEPT_IMPL_2 : ITER_CONCEPT_IMPL_3<I> {};
template<typename I>
struct ITER_CONCEPT_IMPL_2<I, true> {
  using type = typename ITER_TRAITS<I>::iterator_category;
};

template<typename I, bool = has_typename_iterator_concept<ITER_TRAITS<I>>::value /* false */>
struct ITER_CONCEPT_IMPL_1 : ITER_CONCEPT_IMPL_2<I> {};
template<typename I>
struct ITER_CONCEPT_IMPL_1<I, true> {
  using type = typename ITER_TRAITS<I>::iterator_concept;
};

template<typename I>
struct ITER_CONCEPT_T : ITER_CONCEPT_IMPL_1<I> {};

template<typename I>
using ITER_CONCEPT = typename ITER_CONCEPT_T<I>::type;

} // namespace detail
} // namespace preview

#endif // PREVIEW_ITERATOR_DETAIL_ITER_CONCEPT_H_
