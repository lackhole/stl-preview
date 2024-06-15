//
// Created by YongGyu Lee on 11/3/23.
//

#ifndef PREVIEW_VARIANT_MONOSTATE_H_
#define PREVIEW_VARIANT_MONOSTATE_H_

#include <functional>

namespace preview {

struct monostate {};

constexpr bool operator==(monostate, monostate) noexcept { return true; }
constexpr bool operator!=(monostate, monostate) noexcept { return false; }
constexpr bool operator< (monostate, monostate) noexcept { return false; }
constexpr bool operator> (monostate, monostate) noexcept { return false; }
constexpr bool operator<=(monostate, monostate) noexcept { return true; }
constexpr bool operator>=(monostate, monostate) noexcept { return true; }


} // namespace preview

template<>
struct std::hash<::preview::monostate> {
  std::size_t operator()(const ::preview::monostate&) const noexcept {
    return 1998;
  }
};

#endif // PREVIEW_VARIANT_MONOSTATE_H_
