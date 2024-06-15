//
// Created by yonggyulee on 2023/12/20.
//

#ifndef PREVIEW_TYPE_TRAITS_DETAIL_IS_SWAPPABLE_CXX14_H_
#define PREVIEW_TYPE_TRAITS_DETAIL_IS_SWAPPABLE_CXX14_H_

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <queue>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_swappable.h"

namespace preview {
namespace internal {

// std::pair

template<typename T1, typename T2>
struct is_swappable_with_impl<std::pair<T1, T2>&, std::pair<T1, T2>&>
    : conjunction<is_swappable<T1>, is_swappable<T2>> {
    static constexpr bool nothrow = is_nothrow_swappable<T1>::value && is_nothrow_swappable<T2>::value;
};

template<typename T1, typename T2>
struct is_swappable_with_impl<const std::pair<T1, T2>&, const std::pair<T1, T2>&>
    : conjunction<is_swappable<const T1>, is_swappable<const T2>> {
    static constexpr bool nothrow = is_nothrow_swappable<const T1>::value && is_nothrow_swappable<const T2>::value;
};


// std::tuple

template<typename... Types>
struct is_swappable_with_impl<std::tuple<Types...>&, std::tuple<Types...>&>
    : conjunction<is_swappable<Types>...> {
    static constexpr bool nothrow = conjunction<is_nothrow_swappable<Types>...>::value;
};

template<typename... Types>
struct is_swappable_with_impl<const std::tuple<Types...>&, const std::tuple<Types...>&>
    : conjunction<is_swappable<const Types>...> {
    static constexpr bool nothrow = conjunction<is_nothrow_swappable<const Types>...>::value;
};


// std::unique_ptr

template<typename T, typename D>
struct is_swappable_with_impl<std::unique_ptr<T, D>&, std::unique_ptr<T, D>&> : is_swappable<D> {
    static constexpr bool nothrow = true;
};


// std::basic_string

template<typename CharT, typename Traits, typename Allocator>
struct is_swappable_with_impl<std::basic_string<CharT, Traits, Allocator>&, std::basic_string<CharT, Traits, Allocator>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
        std::allocator_traits<Allocator>::is_always_equal::value;
};

// std::array

template<typename T>
struct is_swappable_with_impl<std::array<T, 0>&, std::array<T, 0>&> : std::true_type {
  static constexpr bool nothrow = true;
};

template<typename T, std::size_t N>
struct is_swappable_with_impl<std::array<T, N>&, std::array<T, N>&> : is_swappable<T> {
  static constexpr bool nothrow = is_nothrow_swappable<T>::value;
};


// std::deque

template<typename T, typename Alloc>
struct is_swappable_with_impl<std::deque<T, Alloc>&, std::deque<T, Alloc>&> : std::true_type {
    static constexpr bool nothrow = std::allocator_traits<Alloc>::is_always_equal::value;
};


// std::forward_list

template<typename T, typename Alloc>
struct is_swappable_with_impl<std::forward_list<T, Alloc>&, std::forward_list<T, Alloc>&> : std::true_type {
    static constexpr bool nothrow = std::allocator_traits<Alloc>::is_always_equal::value;
};


// std::list

template<typename T, typename Alloc>
struct is_swappable_with_impl<std::list<T, Alloc>&, std::list<T, Alloc>&> : std::true_type {
    static constexpr bool nothrow = std::allocator_traits<Alloc>::is_always_equal::value;
};


// std::vector

template<typename T, typename Alloc>
struct is_swappable_with_impl<std::vector<T, Alloc>&, std::vector<T, Alloc>&> : std::true_type {
  static constexpr bool nothrow =
      std::allocator_traits<Alloc>::propagate_on_container_swap::value ||
      std::allocator_traits<Alloc>::is_always_equal::value;
};


// std::map

template<typename Key, typename T, typename Compare, typename Alloc>
struct is_swappable_with_impl<std::map<Key, T, Compare, Alloc>&, std::map<Key, T, Compare, Alloc>&> : std::true_type {
  static constexpr bool nothrow =
      std::allocator_traits<Alloc>::is_always_equal::value &&
      is_nothrow_swappable<Compare>::value;
};


// std::multimap

template<typename Key, typename T, typename Compare, typename Alloc>
struct is_swappable_with_impl<std::multimap<Key, T, Compare, Alloc>&, std::multimap<Key, T, Compare, Alloc>&> : std::true_type {
  static constexpr bool nothrow =
      std::allocator_traits<Alloc>::is_always_equal::value &&
      is_nothrow_swappable<Compare>::value;
};


// std::set

template<typename Key, typename Compare, typename Alloc>
struct is_swappable_with_impl<std::set<Key, Compare, Alloc>&, std::set<Key, Compare, Alloc>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Alloc>::is_always_equal::value &&
        is_nothrow_swappable<Compare>::value;
};


// std::multiset

template<typename Key, typename Compare, typename Alloc>
struct is_swappable_with_impl<std::multiset<Key, Compare, Alloc>&, std::multiset<Key, Compare, Alloc>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Alloc>::is_always_equal::value &&
        is_nothrow_swappable<Compare>::value;
};


// std::unordered_map

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
struct is_swappable_with_impl<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>&, std::unordered_map<Key, T, Hash, KeyEqual, Alloc>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Alloc>::is_always_equal::value &&
        is_nothrow_swappable<Hash>::value &&
        is_nothrow_swappable<KeyEqual>::value;
};


// std::unordered_multimap

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
struct is_swappable_with_impl<std::unordered_multimap<Key, T, Hash, KeyEqual, Alloc>&, std::unordered_multimap<Key, T, Hash, KeyEqual, Alloc>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Alloc>::is_always_equal::value &&
        is_nothrow_swappable<Hash>::value &&
        is_nothrow_swappable<KeyEqual>::value;
};


// std::unordered_set

template<typename Key, typename Hash, typename KeyEqual, typename Alloc>
struct is_swappable_with_impl<std::unordered_set<Key, Hash, KeyEqual, Alloc>&, std::unordered_set<Key, Hash, KeyEqual, Alloc>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Alloc>::is_always_equal::value &&
        is_nothrow_swappable<Hash>::value &&
        is_nothrow_swappable<KeyEqual>::value;
};


// std::unordered_multiset

template<typename Key, typename Hash, typename KeyEqual, typename Alloc>
struct is_swappable_with_impl<std::unordered_multiset<Key, Hash, KeyEqual, Alloc>&, std::unordered_multiset<Key, Hash, KeyEqual, Alloc>&> : std::true_type {
    static constexpr bool nothrow =
        std::allocator_traits<Alloc>::is_always_equal::value &&
        is_nothrow_swappable<Hash>::value &&
        is_nothrow_swappable<KeyEqual>::value;
};


// std::queue

template<typename T, typename Container>
struct is_swappable_with_impl<std::queue<T, Container>&, std::queue<T, Container>&> : is_swappable<Container> {
  static constexpr bool nothrow = is_nothrow_swappable<Container>::value;
};


// std::priority_queue

template<typename T, typename Container, typename Compare>
struct is_swappable_with_impl<std::priority_queue<T, Container, Compare>&, std::priority_queue<T, Container, Compare>&>
    : conjunction<is_swappable<Container>, is_swappable<Compare>> {
  static constexpr bool nothrow = conjunction<is_nothrow_swappable<Container>, is_nothrow_swappable<Compare>>::value;
};


// std::stack

template<typename T, typename Container>
struct is_swappable_with_impl<std::stack<T, Container>&, std::stack<T, Container>&> : is_swappable<Container> {
    static constexpr bool nothrow = is_nothrow_swappable<Container>::value;
};

}
} // namespace preview

#endif // PREVIEW_TYPE_TRAITS_DETAIL_IS_SWAPPABLE_CXX14_H_
