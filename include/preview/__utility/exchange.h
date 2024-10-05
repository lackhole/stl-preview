//
// Created by yonggyulee on 2024. 10. 5.
//

#ifndef PREVIEW_UTILITY_EXCHANGE_H_
#define PREVIEW_UTILITY_EXCHANGE_H_

#include <type_traits>
#include <utility>

namespace preview {

template<class T, class U = T>
constexpr T exchange(T& obj, U&& new_value)
    noexcept(std::is_nothrow_move_constructible<T>::value &&
             std::is_nothrow_assignable<T&, U>::value)
{
  T old_value = std::move(obj);
  obj = std::forward<U>(new_value);
  return old_value;
}

} // namespace preview

#endif // PREVIEW_UTILITY_EXCHANGE_H_
