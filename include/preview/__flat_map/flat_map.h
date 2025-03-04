//
// Created by yonggyulee on 15/01/2025
//

#ifndef PREVIEW_FLAT_MAP_FLAT_MAP_H_
#define PREVIEW_FLAT_MAP_FLAT_MAP_H_

#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "preview/__algorithm/ranges/sort.h"
#include "preview/__core/std_version.h"
#include "preview/__iterator/basic_const_iterator.h"
#include "preview/__flat_map/sorted_unique.h"
#include "preview/__ranges/container_compatible_range.h"
#include "preview/__ranges/from_range.h"
#include "preview/__ranges/views/zip.h"

namespace preview {
namespace detail {

template<typename T>
class flat_map_iterator {
 public:
  constexpr flat_map_iterator() noexcept = default;

  explicit constexpr flat_map_iterator(T* ptr) noexcept : ptr_(ptr) {}

  constexpr T& operator*() noexcept { return *ptr_; }
  constexpr const T& operator*() const noexcept { return *ptr_; }

  constexpr T* operator->() noexcept { return ptr_; }
  constexpr const T* operator->() const noexcept { return ptr_; }

  constexpr flat_map_iterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }

  constexpr flat_map_iterator operator++(int) noexcept {
    flat_map_iterator tmp{*this};
    ++ptr_;
    return tmp;
  }

  constexpr flat_map_iterator& operator--() noexcept {
    --ptr_;
    return *this;
  }

  constexpr flat_map_iterator operator--(int) noexcept {
    flat_map_iterator tmp{*this};
    --ptr_;
    return tmp;
  }

  constexpr flat_map_iterator& operator+=(std::ptrdiff_t n) noexcept {
    ptr_ += n;
    return *this;
  }

  constexpr flat_map_iterator& operator-=(std::ptrdiff_t n) noexcept {
    ptr_ -= n;
    return *this;
  }

  friend constexpr flat_map_iterator operator+(flat_map_iterator i, std::ptrdiff_t n) noexcept {
    return flat_map_iterator{i.ptr_ += n};
  }

  friend constexpr flat_map_iterator operator+(std::ptrdiff_t n, flat_map_iterator i) noexcept {
    return flat_map_iterator{i.ptr_ += n};
  }

  friend constexpr flat_map_iterator operator-(flat_map_iterator i, std::ptrdiff_t n) noexcept {
    return flat_map_iterator{i.ptr_ -= n};
  }

  friend constexpr std::ptrdiff_t operator-(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return lhs.ptr_ - rhs.ptr_;
  }

  friend constexpr bool operator==(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return lhs.ptr_ == rhs.ptr_;
  }

  friend constexpr bool operator<(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return lhs.ptr_ < rhs.ptr_;
  }

#if PREVIEW_CXX_VERSION < 20
  friend constexpr bool operator!=(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return !(lhs == rhs);
  }

  friend constexpr bool operator<=(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return !(rhs < lhs);
  }

  friend constexpr bool operator>(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return rhs < lhs;
  }

  friend constexpr bool operator>=(const flat_map_iterator& lhs, const flat_map_iterator& rhs) noexcept {
    return !(lhs < rhs);
  }
#endif

 private:
  T* ptr_{};
};

} // namespace detail

template<typename Key, typename T, typename Compare = std::less<Key>,
         typename KeyContainer = std::vector<Key>, typename MappedContainer = std::vector<T>>
class flat_map {
 public:
  using key_type               = Key;
  using mapped_type            = T;
  using value_type             = std::pair<key_type, mapped_type>;
  using key_compare            = Compare;
  using reference              = std::pair<const key_type&, mapped_type&>;
  using const_reference        = std::pair<const key_type&, const mapped_type&>;
  using size_type              = size_t;
  using difference_type        = ptrdiff_t;
  using iterator               = detail::flat_map_iterator<value_type>;
  using const_iterator         = detail::flat_map_iterator<const value_type>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using key_container_type     = KeyContainer;
  using mapped_container_type  = MappedContainer;

  class value_compare {
    key_compare comp;
    value_compare(key_compare c) : comp(c) {}

   public:
    bool operator()(const_reference x, const_reference y) const {
      return comp(x.first, y.first);
    }
  };

  struct containers {
    key_container_type keys;
    mapped_container_type values;
  };

  flat_map()
      : flat_map(key_compare()) {}

  explicit flat_map(const key_compare& comp)
      : containers_()
      , compare_(comp) {}

  // TODO
  flat_map(key_container_type key_container, mapped_container_type mapped_container,
           const key_compare& comp = key_compare())
      : containers_{std::move(key_container), std::move(mapped_container)}
      , compare_(comp)
  {

  }

  // TODO
  flat_map(sorted_unique_t, key_container_type key_container, mapped_container_type mapped_container,
           const key_compare& comp = key_compare()) {}

  template<typename InputIterator>
  flat_map(InputIterator first, InputIterator last, const key_compare& comp = key_compare())
      : containers_()
      , compare_(comp)
  {
    insert(first, last);
  }

  template<typename InputIterator>
  flat_map(sorted_unique_t s, InputIterator first, InputIterator last, const key_compare& comp = key_compare())
      : containers_()
      , compare_(comp)
  {
    insert(s, first, last);
  }

  template<typename R, std::enable_if_t<detail::container_compatible_range<R, value_type>::value, int> = 0>
  flat_map(from_range_t, R&& rg)
      : flat_map(from_range, std::forward<R>(rg), key_compare()) {}

  template<typename R, std::enable_if_t<detail::container_compatible_range<R, value_type>::value, int> = 0>
  flat_map(from_range_t, R&& rg, const key_compare& comp)
      : flat_map(comp)
  {
    insert_range(std::forward<R>(rg));
  }

  flat_map(std::initializer_list<value_type> il, const key_compare& comp = key_compare())
      : flat_map(il.begin(), il.end(), comp) {}

  flat_map(sorted_unique_t s, std::initializer_list<value_type> il, const key_compare& comp = key_compare())
      : flat_map(s, il.begin(), il.end(), comp) {}

  template<typename Alloc>
  explicit flat_map(const Alloc& a);

  template<typename Alloc>
  flat_map(const key_compare& comp, const Alloc& a);

  template<typename Alloc>
  flat_map(const key_container_type& key_cont, const mapped_container_type& mapped_cont, const Alloc& a);

  template<typename Alloc>
  flat_map(const key_container_type& key_cont, const mapped_container_type& mapped_cont,
           const key_compare& comp, const Alloc& a);

  template<typename Alloc>
  flat_map(sorted_unique_t, const key_container_type& key_cont,
           const mapped_container_type& mapped_cont, const Alloc& a);

  template<typename Alloc>
  flat_map(sorted_unique_t, const key_container_type& key_cont,
           const mapped_container_type& mapped_cont,
           const key_compare& comp, const Alloc& a);

  template<typename Alloc>
  flat_map(const flat_map&, const Alloc& a);

  template<typename Alloc>
  flat_map(flat_map&&, const Alloc& a);

  template<typename InputIterator, typename Alloc>
  flat_map(InputIterator first, InputIterator last, const Alloc& a);

  template<typename InputIterator, typename Alloc>
  flat_map(InputIterator first, InputIterator last,
           const key_compare& comp, const Alloc& a);

  template<typename InputIterator, typename Alloc>
  flat_map(sorted_unique_t, InputIterator first, InputIterator last, const Alloc& a);

  template<typename InputIterator, typename Alloc>
  flat_map(sorted_unique_t, InputIterator first, InputIterator last,
           const key_compare& comp, const Alloc& a);

  template<typename R, typename Alloc, std::enable_if_t<detail::container_compatible_range<R, value_type>::value, int> = 0>
  flat_map(from_range_t, R&& rg, const Alloc& a);

  template<typename R, typename Alloc, std::enable_if_t<detail::container_compatible_range<R, value_type>::value, int> = 0>
  flat_map(from_range_t, R&& rg, const key_compare& comp, const Alloc& a);

  template<typename Alloc>
  flat_map(std::initializer_list<value_type> il, const Alloc& a);

  template<typename Alloc>
  flat_map(std::initializer_list<value_type> il, const key_compare& comp, const Alloc& a);

  template<typename Alloc>
  flat_map(sorted_unique_t, std::initializer_list<value_type> il, const Alloc& a);

  template<typename Alloc>
  flat_map(sorted_unique_t, std::initializer_list<value_type> il, const key_compare& comp, const Alloc& a);

  flat_map& operator=(std::initializer_list<value_type>);

  iterator               begin() noexcept;
  const_iterator         begin() const noexcept;
  iterator               end() noexcept;
  const_iterator         end() const noexcept;

  reverse_iterator       rbegin() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  reverse_iterator       rend() noexcept;
  const_reverse_iterator rend() const noexcept;

  const_iterator         cbegin() const noexcept;
  const_iterator         cend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

 private:
  containers containers_;
  key_compare compare_;
};

} // namespace preview

namespace std {

template< class Key, class T, class Compare,
    class KeyContainer, class MappedContainer, class Allocator >
struct uses_allocator<preview::flat_map<Key, T, Compare, KeyContainer, MappedContainer>, Allocator>
    : std::integral_constant<bool,
        std::uses_allocator<KeyContainer, Allocator>::value &&
        std::uses_allocator<MappedContainer, Allocator>::value
    > {};

} // namespace std

#endif // PREVIEW_FLAT_MAP_FLAT_MAP_H_
