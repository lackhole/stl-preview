//
// Created by yonggyulee on 18/11/2024
//

#ifndef PREVIEW_INCLUDE_PREVIEW_UTILITY_UNREACHABLE_H_
#define PREVIEW_INCLUDE_PREVIEW_UTILITY_UNREACHABLE_H_

#include <cstdlib>
#include <type_traits>

#include "preview/__core/std_version.h"

#if PREVIEW_CXX_VERSION >= 23
#include <utility>
#endif

#include "preview/__type_traits/void_t.h"

namespace preview {
namespace detail_unreachable {

template<typename = void>
struct adl_unreachable : std::false_type {};
template<>
struct adl_unreachable<void_t<decltype(__builtin_unreachable())>> : std::true_type {};

[[noreturn]] [[maybe_unused]] inline void unreachable_impl(std::true_type) {
  __builtin_unreachable();
}

[[noreturn]] [[maybe_unused]] inline void unreachable_impl(std::false_type) {
  abort();
}

} // namespace detail_unreachable

#if PREVIEW_CXX_VERSION >= 23

using std::unreachable;

#else

[[noreturn]] inline void unreachable() {
#if defined(_MSC_VER)
  __assume(0);
#else
  preview::detail_unreachable::unreachable_impl(detail_unreachable::adl_unreachable<>{});
#endif
}

#endif

} // namespace preview

#endif // PREVIEW_INCLUDE_PREVIEW_UTILITY_UNREACHABLE_H_
