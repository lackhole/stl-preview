//
// Created by yonggyulee on 2024. 7. 13..
//

#ifndef PREVIEW_RANGES_DETAIL_RESERVABLE_CONTAINER_H_
#define PREVIEW_RANGES_DETAIL_RESERVABLE_CONTAINER_H_

#include <type_traits>

#include "preview/__ranges/range_size_t.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename T, typename N, typename = void>
struct has_mem_fn_reserve : std::false_type {};
template<typename T, typename N>
struct has_mem_fn_reserve<T, N, void_t<decltype( std::declval<T&>().reserve(std::declval<N>()) )>> : std::true_type {};

template<typename T, typename N, typename = void>
struct has_mem_fn_capacity : std::false_type {};
template<typename T, typename N>
struct has_mem_fn_capacity<T, N,
         void_t<decltype(std::declval<T&>().capacity())   >
    > : same_as<decltype(std::declval<T&>().capacity()), N> {};

template<typename T, typename N, typename = void>
struct has_mem_fn_max_size : std::false_type {};
template<typename T, typename N>
struct has_mem_fn_max_size<T, N,
         void_t<decltype(std::declval<T&>().max_size())   >
    > : same_as<decltype(std::declval<T&>().max_size()), N> {};

template<typename Container, bool = sized_range<Container>::value /* false */>
struct reservable_container_impl : std::false_type {};

template<typename Container>
struct reservable_container_impl<Container, true>
    : conjunction<
        has_mem_fn_reserve<Container, range_size_t<Container>>,
        has_mem_fn_capacity<Container, range_size_t<Container>>,
        has_mem_fn_max_size<Container, range_size_t<Container>>
    >{};

} // namespace detail

template<typename Container>
struct reservable_container : detail::reservable_container_impl<Container> {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_DETAIL_RESERVABLE_CONTAINER_H_
