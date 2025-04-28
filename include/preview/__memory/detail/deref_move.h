//
// Created by yonggyulee on 23/04/2025
//

#ifndef PREVIEW_MEMORY_DETAIL_DEREF_MOVE_H_
#define PREVIEW_MEMORY_DETAIL_DEREF_MOVE_H_

#include <type_traits>
#include <utility>

namespace preview {
namespace detail {

template<typename I>
constexpr decltype(auto) deref_move_impl(I& it, std::true_type) { return std::move(*it); }
template<typename I>
constexpr decltype(auto) deref_move_impl(I& it, std::false_type) { return *it; }

template<typename I>
constexpr decltype(auto) deref_move(I& it) {
  return preview::detail::deref_move_impl(it, std::is_lvalue_reference<decltype(*it)>{});
}

} // namespace detail
} // namespace preview 

#endif // PREVIEW_MEMORY_DETAIL_DEREF_MOVE_H_
