//
// Created by yonggyulee on 27/02/2025
//

#ifndef PREVIEW_STRING_VIEW_BASIC_STRING_VIEW_CXX17_BASE_H_
#define PREVIEW_STRING_VIEW_BASIC_STRING_VIEW_CXX17_BASE_H_

#include "preview/config.h"
#include "preview/core.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <ostream>

#include "preview/__functional/hash_array.h"
#include "preview/__iterator/contiguous_iterator.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__iterator/iter_value_t.h"
#include "preview/__memory/addressof.h"
#include "preview/__memory/to_address.h"
#include "preview/__ranges/borrowed_range.h"
#include "preview/__ranges/contiguous_range.h"
#include "preview/__ranges/data.h"
#include "preview/__ranges/enable_view.h"
#include "preview/__ranges/range_value_t.h"
#include "preview/__ranges/size.h"
#include "preview/__ranges/sized_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/has_conversion_operator.h"
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

template<typename CharT, typename Traits>
class basic_string_view_cxx17_base {
 public:
  using traits_type = Traits;
  using value_type = CharT;
  using pointer = CharT*;
  using const_pointer = const CharT*;
  using reference = CharT&;
  using const_reference = const CharT&;
  using const_iterator = const_pointer;
  using iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  static constexpr size_type npos = size_type(-1);

  constexpr basic_string_view_cxx17_base() noexcept
      : data_(nullptr), size_(0) {}

  constexpr basic_string_view_cxx17_base(const basic_string_view_cxx17_base& other ) noexcept = default;

  constexpr basic_string_view_cxx17_base(const CharT* s, size_type count )
      : data_(s), size_(count) {}

  constexpr basic_string_view_cxx17_base(const CharT* s )
      : data_(s), size_(traits_type::length(s)) {}

  // basic_string_view_cxx17_base does not have a constructor that accepts std::basic_string.
  // Rather, std::basic_string defines a operator string_view.
  // Add two custom constructors since std::basic_string cannot be modified
  // It is the programmer's responsibility to ensure that the resulting string view does not outlive the string.
  constexpr basic_string_view_cxx17_base(const std::basic_string<CharT, Traits>& s)
      : data_(s.data()), size_(s.size()) {}

  constexpr basic_string_view_cxx17_base(std::basic_string<CharT, Traits>&& s)
      : data_(s.data()), size_(s.size()) {}

  constexpr basic_string_view_cxx17_base& operator=(const basic_string_view_cxx17_base& other) noexcept = default;

  // Substitutaion for std::basic_string::basic_string(StringViewLike, ...)
  explicit operator std::basic_string<CharT, Traits>() const {
    return std::basic_string<CharT, Traits>(data(), size());
  }

  constexpr const_iterator begin() const noexcept { return data_; }

  constexpr const_iterator cbegin() const noexcept { return data_; }

  constexpr const_iterator end() const noexcept { return data_ + size_; }

  constexpr const_iterator cend() const noexcept { return data_ + size_; }

  constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(cend()); }

  constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

  constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(cbegin()); }

  constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

  constexpr const_reference operator[](size_type pos) const {
    return data_[pos];
  }

  constexpr const_reference at(size_type pos) const {
    if (pos >= size()) {
      throw std::out_of_range("preview::string_view::at : out of range");
    }
    return (*this)[pos];
  }

  constexpr const_reference front() const { return (*this)[0]; }

  constexpr const_reference back() const { return (*this)[size() - 1]; }

  constexpr const_pointer data() const noexcept { return data_; }

  constexpr size_type size() const noexcept { return size_; }

  constexpr size_type length() const noexcept { return size_; }

  constexpr size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(value_type); }

  constexpr bool empty() const noexcept { return size() == 0; }

  constexpr void remove_prefix(size_type n) {
    data_ += n;
    size_ -= n;
  }

  constexpr void remove_suffix(size_type n) {
    size_ -= n;
  }

  constexpr void swap(basic_string_view_cxx17_base& other) noexcept {
    const_pointer p = data_;
    data_ = other.data_;
    other.data_ = p;

    size_type s = size_;
    size_ = other.size_;
    other.size_ = s;
  }

  size_type copy(value_type* dest, size_type count, size_type pos = 0) const {
    if (pos > size()) {
      throw std::out_of_range("preview::string_view::copy : out of range");
    }
    return traits_type::copy(dest, data() + pos, (std::min)(count, size() - pos));
  }

  constexpr basic_string_view_cxx17_base substr(size_type pos = 0, size_type count = npos) const {
    if (pos > size()) {
      throw std::out_of_range("preview::string_view::substr : out of range");
    }
    return basic_string_view_cxx17_base(data() + pos, (std::min)(count, size() - pos));
  }

  constexpr int compare(basic_string_view_cxx17_base other) const noexcept {
    int r = traits_type::compare(data(), other.data(), (std::min)(size(), other.size()));
    if (r == 0) {
      return size() < other.size() ? -1 :
             size() > other.size() ?  1 :
             0;
    }
    return r;
  }

  constexpr int compare(size_type pos1, size_type count1, basic_string_view_cxx17_base other) const {
    return substr(pos1, count1).compare(other);
  }

  constexpr int compare(size_type pos1, size_type count1, basic_string_view_cxx17_base other, size_type pos2, size_type count2) const {
    return substr(pos1, count1).compare(other.substr(pos2, count2));
  }

  constexpr int compare(const CharT* s) const {
    return compare(basic_string_view_cxx17_base(s));
  }

  constexpr int compare(size_type pos1, size_type count1, const value_type* s) const {
    return substr(pos1, count1).compare(basic_string_view_cxx17_base(s));
  }

  constexpr int compare(size_type pos1, size_type count1, const value_type* s, size_type count2) const {
    return substr(pos1, count1).compare(basic_string_view_cxx17_base(s, count2));
  }

  constexpr size_type find(basic_string_view_cxx17_base sv, size_type pos = 0) const noexcept {
    while (pos <= size() - sv.size()) {
      if (traits_type::compare(data() + pos, sv.data(), sv.size()) == 0)
        return pos;
      ++pos;
    }
    return npos;
  }

  constexpr size_type find(value_type c, size_type pos = 0) const noexcept {
    return find(basic_string_view_cxx17_base(preview::addressof(c), 1), pos);
  }

  constexpr size_type find(const value_type* str, size_type pos, size_type count) const {
    return find(basic_string_view_cxx17_base(str, count), pos);
  }

  constexpr size_type find(const value_type* str, size_type pos = 0) const {
    return find(basic_string_view_cxx17_base(str), pos);
  }

  constexpr size_type rfind(basic_string_view_cxx17_base sv, size_type pos = npos) const noexcept {
    pos = (std::min)(size() - sv.size(), pos);
    while (pos <= size() - sv.size()) {
      if (traits_type::compare(data() + pos, sv.data(), sv.size()) == 0)
        return pos;
      --pos;
    }
    return npos;
  }

  constexpr size_type rfind(value_type c, size_type pos = npos) const noexcept {
    return rfind(basic_string_view_cxx17_base(preview::addressof(c), 1), pos);
  }

  constexpr size_type rfind(const value_type* str, size_type pos, size_type count) const {
    return rfind(basic_string_view_cxx17_base(str, count), pos);
  }

  constexpr size_type rfind(const value_type* str, size_type pos = npos) const {
    return rfind(basic_string_view_cxx17_base(str), pos);
  }

  constexpr size_type find_first_of(basic_string_view_cxx17_base sv, size_type pos = 0) const noexcept {
    while (pos < size()) {
      if (traits_type::find(sv.data(), sv.size(), (*this)[pos])) {
        return pos;
      }
      ++pos;
    }
    return npos;
  }

  constexpr size_type find_first_of(value_type c, size_type pos = 0) const noexcept {
    return find_first_of(basic_string_view_cxx17_base(preview::addressof(c), 1), pos);
  }

  constexpr size_type find_first_of(const value_type* s, size_type pos, size_type count) const {
    return find_first_of(basic_string_view_cxx17_base(s, count), pos);
  }

  constexpr size_type find_first_of(const value_type* s, size_type pos = 0) const {
    return find_first_of(basic_string_view_cxx17_base(s), pos);
  }

  constexpr size_type find_last_of(basic_string_view_cxx17_base sv, size_type pos = npos) const noexcept {
    pos = (std::min)(size() - 1, pos);
    while (pos < size()) {
      if (traits_type::find(sv.data(), sv.size(), (*this)[pos])) {
        return pos;
      }
      --pos;
    }
    return npos;
  }

  constexpr size_type find_last_of(value_type c, size_type pos = npos) const noexcept {
    return find_last_of(basic_string_view_cxx17_base(preview::addressof(c), 1), pos);
  }

  constexpr size_type find_last_of(const value_type* s, size_type pos, size_type count) const {
    return find_last_of(basic_string_view_cxx17_base(s, count), pos);
  }

  constexpr size_type find_last_of(const value_type* s, size_type pos = npos) const {
    return find_last_of(basic_string_view_cxx17_base(s), pos);
  }

  constexpr size_type find_first_not_of(basic_string_view_cxx17_base sv, size_type pos = 0) const noexcept {
    while (pos < size()) {
      if (!traits_type::find(sv.data(), sv.size(), (*this)[pos])) {
        return pos;
      }
      ++pos;
    }
    return npos;
  }

  constexpr size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept {
    return find_first_not_of(basic_string_view_cxx17_base(preview::addressof(c), 1), pos);
  }

  constexpr size_type find_first_not_of(const value_type* s, size_type pos, size_type count) const {
    return find_first_not_of(basic_string_view_cxx17_base(s, count), pos);
  }

  constexpr size_type find_first_not_of(const value_type* s, size_type pos = 0) const {
    return find_first_not_of(basic_string_view_cxx17_base(s), pos);
  }

  constexpr size_type find_last_not_of(basic_string_view_cxx17_base sv, size_type pos = npos) const noexcept {
    pos = (std::min)(size() - 1, pos);
    while (pos < size()) {
      if (!traits_type::find(sv.data(), sv.size(), (*this)[pos])) {
        return pos;
      }
      --pos;
    }
    return npos;
  }

  constexpr size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept {
    return find_last_not_of(basic_string_view_cxx17_base(preview::addressof(c), 1), pos);
  }

  constexpr size_type find_last_not_of(const value_type* s, size_type pos, size_type count) const {
    return find_last_not_of(basic_string_view_cxx17_base(s, count), pos);
  }

  constexpr size_type find_last_not_of(const value_type* s, size_type pos = npos) const {
    return find_last_not_of(basic_string_view_cxx17_base(s), pos);
  }

 private:
  const_pointer data_;
  size_type size_;
};

} // namespace detail

} // namespace preview

#endif // PREVIEW_STRING_VIEW_BASIC_STRING_VIEW_CXX17_BASE_H_
