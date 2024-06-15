//
// Created by YongGyu Lee on 1/26/24.
//

#ifndef PREVIEW_ALGORITHM_CLAMP_H_
#define PREVIEW_ALGORITHM_CLAMP_H_

#include <functional>

namespace preview {

template<typename T, typename Compare>
constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp) {
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
  return preview::clamp(v, lo, hi, std::less<>{});
}

} // namespace preview

#endif // PREVIEW_ALGORITHM_CLAMP_H_
