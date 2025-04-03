//
// Created by yonggyulee on 18/11/2024
//

#ifndef PREVIEW_TUPLE_COMPRESSED_TUPLE_H_
#define PREVIEW_TUPLE_COMPRESSED_TUPLE_H_

#include <cstddef>

#include "preview/__utility/compressed_pair.h"

namespace preview {
namespace detail {

template<std::size_t Index, typename... T>
struct compressed_tuple_slot;

template<std::size_t Index>
struct compressed_tuple_slot<Index> {};

template<std::size_t Index, typename T, typename... U>
struct compressed_tuple_slot<Index, T, U...>
    : basic_compressed_slot<T>
    , compressed_tuple_slot<Index + 1, U...>
{
  using basic_compressed_slot<T>::basic_compressed_slot;
  using basic_compressed_slot<T>::value;

  template<std::size_t I> constexpr std::enable_if_t<(I == Index),       T&>  get()       &  noexcept { return value(); }
  template<std::size_t I> constexpr std::enable_if_t<(I == Index), const T&>  get() const &  noexcept { return value(); }
  template<std::size_t I> constexpr std::enable_if_t<(I == Index),       T&&> get()       && noexcept { return std::move(value()); }
  template<std::size_t I> constexpr std::enable_if_t<(I == Index), const T&&> get() const && noexcept { return std::move(value()); }
};


} // namespace detail

template<typename... T>
class compressed_tuple
    : detail::compressed_tuple_slot<0, T...>
{

};

} // namespace preview

#endif // PREVIEW_TUPLE_COMPRESSED_TUPLE_H_
