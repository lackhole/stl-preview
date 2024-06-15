//
// Created by YongGyu Lee on 3/27/24.
//

#ifndef PREVIEW_RANGES_AS_CONST_POINTER_H_
#define PREVIEW_RANGES_AS_CONST_POINTER_H_

namespace preview {
namespace ranges {

template<typename T>
constexpr auto as_const_pointer(const T* p) noexcept {
  return p;
}

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_AS_CONST_POINTER_H_
