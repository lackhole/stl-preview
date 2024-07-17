//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_RANGES_DATA_H_
#define PREVIEW_RANGES_DATA_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/decay_copy.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/random_access_iterator.h"
#include "preview/__memory/to_address.h"
#include "preview/__ranges/detail/not_incomplete_array.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

struct data_niebloid {
 private:
  template<typename T>
  using is_pointer_to_object = conjunction<std::is_pointer<T>, std::is_object<std::remove_pointer_t<T>>>;

  template<typename T, typename = void>
  struct has_mem_fn_data : std::false_type {};
  template<typename T>
  struct has_mem_fn_data<T, void_t<decltype( preview_decay_copy(std::declval<T>().data()) )>>
      : is_pointer_to_object<decltype( preview_decay_copy(std::declval<T>().data()) )> {};

  template<typename T, bool = /* false */has_typename_type<iterator<T>>::value>
  struct ranges_begin_is_contiguous_iterator
      : std::false_type {};
  template<typename T>
  struct ranges_begin_is_contiguous_iterator<T, true>
      : disjunction<
          contiguous_iterator<iterator_t<T>>
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
          ,
          conjunction<
              random_access_iterator<iterator_t<T>>,
              std::is_convertible<iterator_t<T>, std::remove_reference_t<ranges::range_reference_t<T>>*>
          >
#endif
      > {};

  template<typename T>
  static constexpr auto call(T&& t, std::true_type) {
    return preview_decay_copy(t.data());
  }

  template<typename T>
  static constexpr auto call(T&& t, std::false_type) {
    return preview::to_address(ranges::begin(t));
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      disjunction<
          std::is_lvalue_reference<T&&>,
          enable_borrowed_range_t<std::remove_cv_t<T>>
      >,
      preview::detail::not_incomplete_array<T>,
      disjunction<
          has_mem_fn_data<T>,
          ranges_begin_is_contiguous_iterator<T>
      >
  >::value, int> = 0>
  constexpr auto operator()(T&& t) const {
    return call(std::forward<T>(t), has_mem_fn_data<T>{});
  }

#if PREVIEW_CXX_VERSION < 17
  // non-const version of std::basic_string::data was introduced since C++17

  template<typename CharT, typename Traits, typename Allocator>
  constexpr CharT* operator()(std::basic_string<CharT, Traits, Allocator>& str) const {
    return const_cast<CharT*>(str.data());
  }
#endif
};

} // namespace detail

namespace niebloid {

PREVIEW_INLINE_VARIABLE constexpr detail::data_niebloid data{};

} // namespace niebloid
using namespace niebloid;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_DATA_H_
