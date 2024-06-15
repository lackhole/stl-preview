//
// Created by YongGyu Lee on 4/15/24.
//

#ifndef PREVIEW_ITERATOR_DETAIL_ITER_EXCHANGE_MOVE_H_
#define PREVIEW_ITERATOR_DETAIL_ITER_EXCHANGE_MOVE_H_

#include <type_traits>

#include "preview/__iterator/iter_move.h"
#include "preview/__iterator/iter_value_t.h"

namespace preview {
namespace detail {

template<typename T, typename U>
constexpr iter_value_t<std::remove_reference_t<T>> iter_exchange_move(T&& x, U&& y)
    noexcept(noexcept(iter_value_t<std::remove_reference_t<T>>(ranges::iter_move(x))))
{
  iter_value_t<std::remove_reference_t<T>> tmp(ranges::iter_move(x));
  *x = ranges::iter_move(y);
  return tmp;
}

} // namespace detail
} // namespace preview

#endif // PREVIEW_ITERATOR_DETAIL_ITER_EXCHANGE_MOVE_H_
