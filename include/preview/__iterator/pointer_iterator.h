//
// Created by yonggyulee on 2024. 10. 20.
//

#ifndef PREVIEW_ITERATOR_POINTER_ITERATOR_H_
#define PREVIEW_ITERATOR_POINTER_ITERATOR_H_

#include <cstddef>
#include <type_traits>

#include "preview/__iterator/detail/contiguous_iterator_tag.h"
#include "preview/__iterator/iterator_tag.h"

namespace preview {
namespace detail {

template<typename T, typename Tag = void>
class pointer_iterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type      = std::remove_cv_t<T>;
  using pointer         = T*;
  using reference       = T&;
  using iterator_category = std::random_access_iterator_tag;
  using iterator_concept  = contiguous_iterator_tag;

  pointer_iterator() = default;

  constexpr pointer_iterator(pointer p)
      : ptr_(p) {}

  constexpr reference operator*() const noexcept { return *ptr_; }
  constexpr pointer operator->() const noexcept { return ptr_; }

  constexpr reference operator[](difference_type n) const noexcept {
    return ptr_[n];
  }

  constexpr pointer_iterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }
  constexpr pointer_iterator operator++(int) noexcept {
    pointer_iterator temp{*this};
    ++*this;
    return temp;
  }

  constexpr pointer_iterator& operator--() noexcept {
    --ptr_;
    return *this;
  }
  constexpr pointer_iterator operator--(int) noexcept {
    pointer_iterator temp{*this};
    --*this;
    return temp;
  }

  constexpr pointer_iterator& operator+=(difference_type n) const noexcept {
    ptr_ += n;
    return *this;
  }

  constexpr pointer_iterator& operator-=(difference_type n) const noexcept {
    ptr_ -= n;
    return *this;
  }

  friend constexpr bool operator==(pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ == y.ptr_; }
  friend constexpr bool operator!=(pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ != y.ptr_; }
  friend constexpr bool operator< (pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ <  y.ptr_; }
  friend constexpr bool operator<=(pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ <= y.ptr_; }
  friend constexpr bool operator> (pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ >  y.ptr_; }
  friend constexpr bool operator>=(pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ >= y.ptr_; }

  friend constexpr pointer_iterator operator+(pointer_iterator i, difference_type n) noexcept { return pointer_iterator{i.ptr_ + n}; }
  friend constexpr pointer_iterator operator+(difference_type n, pointer_iterator i) noexcept { return pointer_iterator{i.ptr_ + n}; }
  friend constexpr pointer_iterator operator-(pointer_iterator i, difference_type n) noexcept { return pointer_iterator{i.ptr_ - n}; }
  friend constexpr difference_type operator-(pointer_iterator x, pointer_iterator y) noexcept { return x.ptr_ - y.ptr_; }

 private:
  pointer ptr_ = nullptr;
};

} // namespace detail
} // namespace preview

#endif // PREVIEW_ITERATOR_POINTER_ITERATOR_H_
