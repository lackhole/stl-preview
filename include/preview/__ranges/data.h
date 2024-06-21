//
// Created by cosge on 2023-12-02.
//

#ifndef PREVIEW_RANGES_DATA_H_
#define PREVIEW_RANGES_DATA_H_

#include <type_traits>

#include "preview/__core/inline_variable.h"
#include "preview/__core/decay_copy.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__memory/to_address.h"
#include "preview/__ranges/detail/not_incomplete_array.h"
#include "preview/__ranges/begin.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

struct data_niebloid {
 private:
  template<typename T>
  struct is_pointer_to_object : std::false_type {};
  template<typename T>
  struct is_pointer_to_object<T*> : std::is_object<T*> {};

  template<typename T, typename = void>
  struct member_function_data_is_pointer_to_object
      : std::false_type {};
  template<typename T>
  struct member_function_data_is_pointer_to_object<T, void_t<decltype( preview_decay_copy(std::declval<T>().data()) )>>
      : conjunction<
          is_pointer_to_object<decltype( preview_decay_copy(std::declval<T>().data()) )>,
          std::is_convertible<
              decltype( preview_decay_copy(std::declval<T>().data()) ),
              std::remove_reference_t<ranges::range_reference_t<T>>*
          >
      > {};

  template<typename T, typename = void>
  struct ranges_begin_is_contiguous_iterator
      : std::false_type {};
  template<typename T>
  struct ranges_begin_is_contiguous_iterator<T, void_t<decltype( ranges::begin(std::declval<T>()) )>>
      : conjunction<
          contiguous_iterator<decltype( ranges::begin(std::declval<T>()) )>,
          std::is_convertible<
              decltype( ranges::begin(std::declval<T>()) ),
              std::remove_reference_t<ranges::range_reference_t<T>>*
          >
      > {};

  template<typename T>
  constexpr std::remove_reference_t<ranges::range_reference_t<T>>*
  call(T&& t, std::true_type) const {
    return preview_decay_copy(t.data());
  }

  template<typename T>
  constexpr std::remove_reference_t<ranges::range_reference_t<T>>*
  call(T&& t, std::false_type) const {
    return preview::to_address(ranges::begin(t));
  }

 public:
  template<typename T, std::enable_if_t<conjunction<
      preview::detail::not_incomplete_array<T>,
      disjunction<
          std::is_lvalue_reference<T&&>,
          enable_borrowed_range_t<remove_cvref_t<T>>
      >,
      has_typename_type<range_reference<T>>,
      disjunction<
          member_function_data_is_pointer_to_object<T&&>,
          ranges_begin_is_contiguous_iterator<T&&>
      >
  >::value, int> = 0>
  constexpr std::remove_reference_t<ranges::range_reference_t<T>>*
  operator()(T&& t) const {
    return call(std::forward<T>(t), member_function_data_is_pointer_to_object<T&&>{});
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
