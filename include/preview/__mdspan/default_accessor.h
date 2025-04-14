//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_DEFAULT_ACCESSOR_H_
#define PREVIEW_MDSPAN_DEFAULT_ACCESSOR_H_

#include <cstddef>
#include <type_traits>

#include "preview/__type_traits/is_complete.h"

namespace preview {

template<typename ElementType>
struct default_accessor {
  static_assert(is_complete<ElementType>::value, "Invalid type");
  static_assert(!std::is_abstract<ElementType>::value, "Invalid type");
  static_assert(!std::is_array<ElementType>::value, "Invalid type");

  using offset_policy = default_accessor;
  using element_type = ElementType;
  using reference = ElementType&;
  using data_handle_type = ElementType*;

  constexpr default_accessor() noexcept = default;

  template<class OtherElementType, std::enable_if_t<
      std::is_convertible<OtherElementType(*)[], element_type(*)[]>::value, int> = 0>
  constexpr default_accessor(default_accessor<OtherElementType>) noexcept {}

  constexpr reference access(data_handle_type p, std::size_t i) const noexcept {
    return p[i];
  }

  constexpr data_handle_type offset(data_handle_type p, std::size_t i) const noexcept {
    return p + i;
  }
};

} // namespace preview

#endif // PREVIEW_MDSPAN_DEFAULT_ACCESSOR_H_
