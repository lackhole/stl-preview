//
// Created by yonggyulee on 18/11/2024
//

#ifndef PREVIEW_INCLUDE_PREVIEW_UTILITY_UNREACHABLE_H_
#define PREVIEW_INCLUDE_PREVIEW_UTILITY_UNREACHABLE_H_

#include "preview/__core/cxx_version.h"

#if PREVIEW_CXX_VERSION >= 23

#include <utility>

namespace preview {
using std::unreachable;
} // namespace preview

#else

#include "preview/__core/noreturn.h"

#  if defined(_MSC_VER)

namespace preview {
PREVIEW_NORETURN inline void unreachable() {
  __assume(0);
}
} // namespace preview

#  else

#include <cstdlib>
#include <type_traits>

#include "preview/__type_traits/void_t.h"


namespace preview {
namespace detail_unreachable {

template<typename = void>
struct adl_unreachable : std::false_type {};
template<>
struct adl_unreachable<void_t<decltype(__builtin_unreachable())>> : std::true_type {};

PREVIEW_NORETURN [[maybe_unused]] inline void unreachable_impl(std::true_type) {
  __builtin_unreachable();
}

PREVIEW_NORETURN [[maybe_unused]] inline void unreachable_impl(std::false_type) {
  abort();
}

} // namespace detail_unreachable

PREVIEW_NORETURN inline void unreachable() {
  preview::detail_unreachable::unreachable_impl(detail_unreachable::adl_unreachable<>{});
}

} // namespace preview

#  endif // defined(_MSC_VER)
#endif // PREVIEW_CXX_VERSION >= 23

#endif // PREVIEW_INCLUDE_PREVIEW_UTILITY_UNREACHABLE_H_
