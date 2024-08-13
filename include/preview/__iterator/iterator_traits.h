//
// Created by yonggyulee on 2023/12/25.
//

#ifndef PREVIEW_ITERATOR_ITERATOR_TRAITS_H_
#define PREVIEW_ITERATOR_ITERATOR_TRAITS_H_

#include <cstddef>
#include <type_traits>

#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits/forward_declare.h"
#include "preview/__iterator/iterator_traits/legacy_iterator.h"
#include "preview/__iterator/iterator_traits/legacy_input_iterator.h"
#include "preview/__iterator/iterator_traits/legacy_forward_iterator.h"
#include "preview/__iterator/iterator_traits/legacy_bidirectional_iterator.h"
#include "preview/__iterator/iterator_traits/legacy_random_access_iterator.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/has_operator_arrow.h"
#include "preview/__type_traits/has_typename_difference_type.h"
#include "preview/__type_traits/has_typename_value_type.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail {

template<typename T, typename = void>
struct has_typename_pointer : std::false_type {};
template<typename T>
struct has_typename_pointer<T, void_t<typename T::pointer>> : std::true_type {};

template<typename T, typename = void>
struct has_typename_reference : std::false_type {};
template<typename T>
struct has_typename_reference<T, void_t<typename T::reference>> : std::true_type {};

template<typename T, typename = void>
struct has_typename_iterator_category : std::false_type {};
template<typename T>
struct has_typename_iterator_category<T, void_t<typename T::iterator_category>>
    : negation< std::is_same<iterator_ignore, typename T::iterator_category> > {};

template<typename Iter, bool = has_typename_iterator_category<Iter>::value /* false */>
struct iterator_traits_typename_iterator_category {
  using type = conditional_t<
      LegacyRandomAccessIterator<Iter>, random_access_iterator_tag,
      LegacyBidirectionalIterator<Iter>, bidirectional_iterator_tag,
      LegacyForwardIterator<Iter>, forward_iterator_tag,
      input_iterator_tag
  >;
};

template<typename Iter>
struct iterator_traits_typename_iterator_category<Iter, true> {
  using type = typename Iter::iterator_category;
};

template<typename Iter, bool = has_typename_reference<Iter>::value /* false */>
struct iterator_traits_typename_reference {
  using type = iter_reference_t<Iter>;
};
template<typename Iter>
struct iterator_traits_typename_reference<Iter, true> {
  using type = typename Iter::reference;
};

template<typename Iter, bool = has_operator_arrow<Iter&>::value /* false */>
struct iterator_traits_typename_pointer_2 {
  using type = void;
};
template<typename Iter>
struct iterator_traits_typename_pointer_2<Iter, true> {
  using type = decltype( std::declval<Iter&>().operator->() );
};
template<typename Iter, bool = has_typename_pointer<Iter>::value /* false */>
struct iterator_traits_typename_pointer : iterator_traits_typename_pointer_2<Iter> {};
template<typename Iter>
struct iterator_traits_typename_pointer<Iter, true> {
  using type = typename Iter::pointer;
};

template<typename T, bool = has_typename_difference_type<T>::value /* false */>
struct difference_type_or_void {
  using type = void;
};
template<typename T>
struct difference_type_or_void<T, true> {
  using type = typename T::difference_type;
};

struct iterator_traits_empty_typedef {};

template<typename Iter>
struct iterator_traits_legacy_iterator {
  using difference_type = typename difference_type_or_void<incrementable_traits<Iter>>::type;
  using value_type = void;
  using pointer = void;
  using reference = void;
  using iterator_category = output_iterator_tag;
};

template<typename Iter>
struct iterator_traits_legacy_input_iterator {
  using difference_type = typename incrementable_traits<Iter>::difference_type;
  using value_type = typename indirectly_readable_traits<Iter>::value_type;
  using pointer = typename iterator_traits_typename_pointer<Iter>::type;
  using reference = typename iterator_traits_typename_reference<Iter>::type;
  using iterator_category = typename iterator_traits_typename_iterator_category<Iter>::type;;
};

template<
  typename Iter,
  bool = has_typename_difference_type<Iter>::value,
  bool = has_typename_value_type<Iter>::value,
  bool = has_typename_pointer<Iter>::value,
  bool = has_typename_reference<Iter>::value,
  bool = has_typename_iterator_category<Iter>::value
>
struct iterator_traits_sfinae
    : conditional_t<
        LegacyInputIterator<Iter>, iterator_traits_legacy_input_iterator<Iter>,
        LegacyIterator<Iter>, iterator_traits_legacy_iterator<Iter>,
        iterator_traits_empty_typedef
      > {};

// Iter does not have pointer, but has all four remaining nested types
template<typename Iter>
struct iterator_traits_sfinae<Iter, true, true, false, true, true> {
  using difference_type = typename Iter::difference_type;
  using value_type = typename Iter::value_type;
  using pointer = void;
  using reference = typename Iter::reference;
  using iterator_category = typename Iter::iterator_category;
};

// All five typedefs are defined
template<typename Iter>
struct iterator_traits_sfinae<Iter, true, true, true, true, true> {
  using difference_type = typename Iter::difference_type;
  using value_type = typename Iter::value_type;
  using pointer = typename Iter::pointer;
  using reference = typename Iter::reference;
  using iterator_category = typename Iter::iterator_category;
};

template<typename T, bool = std::is_object<T>::value /* false */>
struct iterator_traits_object_pointer
    : iterator_traits_sfinae<T*> {};

template<typename T>
struct iterator_traits_object_pointer<T, true> {
  using difference_type = std::ptrdiff_t;
  using value_type = std::remove_cv_t<T>;
  using pointer = T*;
  using reference = T&;
  using iterator_category = random_access_iterator_tag;
  using iterator_concept = contiguous_iterator_tag;
};

} // namespace detail

template<typename Iter>
struct iterator_traits : detail::iterator_traits_sfinae<Iter> {};

template<typename T>
struct iterator_traits<T*> : detail::iterator_traits_object_pointer<T> {};

} // namespace preview

#endif // PREVIEW_ITERATOR_ITERATOR_TRAITS_H_
