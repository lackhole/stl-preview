//
// Created by YongGyu Lee on 11/27/23.
//

#ifndef PREVIEW_STRING_VIEW_H_
#define PREVIEW_STRING_VIEW_H_

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

#include "preview/core.h"
#include "preview/__string_view/basic_string_view_cxx14_base.h"
#include "preview/__type_traits/has_conversion_operator.h"

#if PREVIEW_CXX_VERSION >= 17
#include <string_view>
#endif

#if PREVIEW_CXX_VERSION < 26 // P2697
#include <bitset>
#endif

#if PREVIEW_CXX_VERSION < 26 // P2495
#include <sstream>
#endif

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
#include "preview/__type_traits/has_typename_type.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/type_identity.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {

template<
    typename CharT,
    typename Traits = std::char_traits<CharT>>
class basic_string_view : public detail::basic_string_view_cxx14_base<CharT, Traits> {
  using base = detail::basic_string_view_cxx14_base<CharT, Traits>;
 public:
  using traits_type             = typename base::traits_type;
  using value_type              = typename base::value_type;
  using pointer                 = typename base::pointer;
  using const_pointer           = typename base::const_pointer;
  using reference               = typename base::reference;
  using const_reference         = typename base::const_reference;
  using const_iterator          = typename base::const_iterator;
  using iterator                = typename base::iterator;
  using const_reverse_iterator  = typename base::const_reverse_iterator;
  using reverse_iterator        = typename base::reverse_iterator;
  using size_type               = typename base::size_type;
  using difference_type         = typename base::difference_type;

  using base::npos;

  using base::base;
  using base::begin;
  using base::cbegin;
  using base::end;
  using base::cend;
  using base::rbegin;
  using base::crbegin;
  using base::rend;
  using base::crend;
  using base::operator[];
  using base::at;
  using base::front;
  using base::back;
  using base::data;
  using base::size;
  using base::length;
  using base::max_size;
  using base::empty;
  using base::remove_prefix;
  using base::remove_suffix;
  using base::swap;
//  using base::copy;
  using base::substr;
  using base::compare;
  using base::find;
  using base::rfind;
  using base::find_first_of;
  using base::find_last_of;
  using base::find_first_not_of;
  using base::find_last_not_of;

  constexpr basic_string_view() noexcept = default;

  constexpr basic_string_view(const basic_string_view& other) noexcept = default;

  constexpr basic_string_view(const CharT* s, size_type count)
      : base(s, count) {}

  constexpr basic_string_view(const CharT* s)
      : base(s) {}

  template<typename It, typename End, std::enable_if_t<conjunction<
      contiguous_iterator<It>,
      sized_sentinel_for<End, It>,
      same_as<iter_value_t<It>, CharT>,
      negation<convertible_to<End, std::size_t>>
  >::value, int> =0>
  constexpr basic_string_view(It first, End last)
      : base(preview::to_address(first), last - first) {}

  template<typename  R, std::enable_if_t<conjunction<
      negation<same_as<remove_cvref_t<R>, basic_string_view>>,
      ranges::contiguous_range<R>,
      ranges::sized_range<R>,
      negation<convertible_to<R, const CharT*>>,
      negation<has_conversion_operator<remove_cvref_t<R>, basic_string_view>>
#if PREVIEW_CXX_VERSION >= 17
      , negation<has_conversion_operator<remove_cvref_t<R>, std::basic_string_view<CharT, Traits>>>
#endif
  >::value, int> = 0>
  constexpr explicit basic_string_view(R&& r)
      : base(ranges::data(r), ranges::size(r)) {}

  // basic_string_view does not have a constructor that accepts std::basic_string.
  // Rather, std::basic_string defines a operator string_view.
  // Add two custom constructors since std::basic_string cannot be modified
  // It is the programmer's responsibility to ensure that the resulting string view does not outlive the string.
  constexpr basic_string_view(const std::basic_string<CharT, Traits>& s)
      : base(s.data(), s.size()) {}

  constexpr basic_string_view(std::basic_string<CharT, Traits>&& s)
      : base(s.data(), s.size()) {}

#if PREVIEW_CXX_VERSION < 17
  // Substitutaion for std::basic_string::basic_string(StringViewLike, ...)
  explicit operator std::basic_string<CharT, Traits>() const {
    return std::basic_string<CharT, Traits>(data(), size());
  }
#else
  constexpr operator std::basic_string_view<CharT, Traits>() const {
    return std::basic_string_view<CharT, Traits>(data(), size());
  }
#endif

  // P2697R1
#if PREVIEW_CXX_VERSION < 26
  template<typename T, std::enable_if_t<conjunction<
      detail::is_bitset<T>
#if PREVIEW_CXX_VERSION >= 17
      , negation<std::is_constructible<T, std::basic_string_view<CharT, Traits>>>
#endif
  >::value, int> = 0>
  constexpr explicit operator T() const {
    constexpr CharT kZero = '0';
    constexpr CharT kOne = '1';

    for (auto c : *this) {
      if (!Traits::eq(c, kZero) && !Traits::eq(c, kOne))
        throw std::invalid_argument("preview::string_view::operator std::bitset : invalid character");
    }

    T bitset;
    std::size_t len = (std::min)(bitset.size(), size());
    for (size_type i = 0; i < len; ++i) {
      bitset[i] = Traits::eq(data()[len - 1 - i], kOne);
    }

    return bitset;
  }
#endif

  // P2495R3
#if PREVIEW_CXX_VERSION < 26
  template<typename T, std::enable_if_t<conjunction<
      disjunction<
          is_specialization<T, std::basic_stringbuf>,
          is_specialization<T, std::basic_istringstream>,
          is_specialization<T, std::basic_ostringstream>,
          is_specialization<T, std::basic_stringstream>
      >
#if PREVIEW_CXX_VERSION >= 17
      , negation<std::is_constructible<T, std::basic_string_view<CharT, Traits>>>
#endif
  >::value, int> = 0>
  constexpr explicit operator T() const {
    // Cannot avoid memory allocation
    return T(std::basic_string<CharT, Traits, typename T::allocator_type>(data(), size()));
  }
#endif

  constexpr basic_string_view(std::nullptr_t) = delete;

  constexpr basic_string_view& operator=(const basic_string_view& other) noexcept = default;

  constexpr size_type copy(value_type* dest, size_type count, size_type pos = 0) const {
    if (pos > size()) {
      throw std::out_of_range("preview::string_view::copy : out of range");
    }
    return traits_type::copy(dest, data() + pos, (std::min)(count, size() - pos));
  }

  constexpr bool starts_with(basic_string_view prefix) const noexcept {
    return basic_string_view(data(), (std::min)(size(), prefix.size())) == prefix;
  }

  constexpr bool starts_with(value_type c) const noexcept {
    return !empty() && traits_type::eq(front(), c);
  }

  constexpr bool starts_with(const value_type* s) const {
    return starts_with(basic_string_view(s));
  }

  constexpr bool ends_with(basic_string_view sv) const noexcept {
    return size() >= sv.size() && compare(size() - sv.size(), npos, sv) == 0;
  }

  constexpr bool ends_with(value_type c) const noexcept {
    return !empty() && traits_type::eq(back(), c);
  }

  constexpr bool ends_with(const value_type* s) const {
    return ends_with(basic_string_view(s));
  }

  constexpr bool contains(basic_string_view sv) const noexcept {
    return find(sv) != npos;
  }

  constexpr bool contains(value_type c) const noexcept {
    return find(c) != npos;
  }

  constexpr bool contains(const value_type* str) const {
    return find(str) != npos;
  }
};

// Extra template parameters are used to workaround a issue in MSVC's ABI (name decoration) (VSO-409326)

// operator==
template<typename CharT, typename Traits>
constexpr bool operator==(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
}

template<typename CharT, typename Traits, int = 1>
constexpr bool operator==(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept {
  return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
}

template<typename CharT, typename Traits, int = 2>
constexpr bool operator==(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
}

// operator!=
template<typename CharT, typename Traits>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return !(lhs == rhs);
}

template<typename CharT, typename Traits, int = 1>
constexpr bool operator!=(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return !(lhs == rhs);
}

template<typename CharT, typename Traits, int = 2>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept {
  return !(lhs == rhs);
}

// operator<
template<typename CharT, typename Traits>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template<typename CharT, typename Traits, int = 1>
constexpr bool operator<(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template<typename CharT, typename Traits, int = 2>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

// operator<=
template<typename CharT, typename Traits>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template<typename CharT, typename Traits, int = 1>
constexpr bool operator<=(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template<typename CharT, typename Traits, int = 2>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

// operator>
template<typename CharT, typename Traits>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template<typename CharT, typename Traits, int = 1>
constexpr bool operator>(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template<typename CharT, typename Traits, int = 2>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

// operator>=
template<typename CharT, typename Traits>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template<typename CharT, typename Traits, int = 1>
constexpr bool operator>=(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template<typename CharT, typename Traits, int = 2>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, basic_string_view<CharT, Traits> sv) {
  using ostream_type = std::basic_ostream<CharT, Traits>;
  using iostate_type = typename ostream_type::iostate;
  using sentry_type = typename ostream_type::sentry;
  using size_type = std::size_t;

  iostate_type state = ostream_type::goodbit;
  sentry_type sentry(os);

  if (!sentry) {
    state |= ostream_type::badbit;
  } else {
    size_type pad;
    if (os.width() <= 0 || static_cast<size_type>(os.width()) <= sv.size()) {
      pad = 0;
    } else {
      pad = static_cast<size_type>(os.width()) - sv.size();
    }

    try {
      // pad on left
      if ((os.flags() & ostream_type::adjustfield) != ostream_type::left) {
        for (; 0 < pad; --pad) {
          if (Traits::eq_int_type(Traits::eof(), os.rdbuf()->sputc(os.fill()))) {
            state |= ostream_type::badbit; // insertion failed, quit
            break;
          }
        }
      }

      std::streamsize n = (std::max)(os.width(), static_cast<std::streamsize>(sv.size()));
      if (state == ostream_type::goodbit && os.rdbuf()->sputn(sv.data(), n) != n) {
        state |= ostream_type::badbit;
      }

      // pad on right
      if (state == ostream_type::goodbit) {
        for (; 0 < pad; --pad) {
          if (Traits::eq_int_type(Traits::eof(), os.rdbuf()->sputc(os.fill()))) {
            state |= ostream_type::badbit; // insertion failed, quit
            break;
          }
        }
      }

      os.width(0);
    } catch (...) {
      if ((os.exceptions() & ostream_type::badbit) != 0) {
        std::rethrow_exception(std::current_exception());
      }
    }
  }

  os.setstate(state);
  return os;
}

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
#if PREVIEW_CXX_VERSION >= 20
using u8string_view = basic_string_view<char8_t>;
#endif
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

inline namespace literals {
inline namespace string_view_literals {

constexpr string_view operator ""_sv(const char* str, std::size_t len) noexcept {
  return string_view{str, len};
}

#if PREVIEW_CXX_VERSION >= 20
constexpr u8string_view operator ""_sv(const char8_t* str, std::size_t len) noexcept {
  return u8string_view{str, len};
}
#endif

constexpr u16string_view operator ""_sv(const char16_t* str, std::size_t len) noexcept {
  return u16string_view{str, len};
}

constexpr u32string_view operator ""_sv(const char32_t* str, std::size_t len) noexcept {
  return u32string_view{str, len};
}

constexpr wstring_view operator ""_sv(const wchar_t* str, std::size_t len) noexcept {
  return wstring_view{str, len};
}

} // namespace string_view_literals
} // namespace literals

template<typename CharT, typename Traits >
struct ranges::enable_view<basic_string_view<CharT, Traits>> : std::true_type {};

#if PREVIEW_CXX_VERSION >= 17

template<typename It, typename End>
basic_string_view(It, End) -> basic_string_view<iter_value_t<It>>;

template<typename R>
basic_string_view(R&&) -> basic_string_view<ranges::range_value_t<R>>;

#endif

namespace detail {

template<typename CharT>
struct string_view_concatenator {
  const CharT* lhs_begin;
  const CharT* lhs_end;
  const CharT* rhs_begin;
  const CharT* rhs_end;

  struct iterator {
    using iterator_category = std::random_access_iterator_tag;
    using value_type = CharT;
    using difference_type = std::ptrdiff_t;
    using pointer = const CharT*;
    using reference = const value_type&;

    constexpr iterator() = default;

    constexpr iterator(string_view_concatenator* base, pointer ptr)
        : base_(base)
        , ptr_(ptr) {}

    constexpr iterator& operator++() {
      if (++ptr_ == base_->lhs_end) {
        ptr_ = base_->rhs_begin;
      }
      return *this;
    }
    constexpr iterator operator++(int) {
      iterator temp = *this;
      ++*this;
      return temp;
    }
    constexpr iterator& operator+=(difference_type n) {
      if (ptr_ < base_->lhs_end && base_->lhs_begin <= ptr_) {
        auto new_pos = ptr_ + n;
        if (new_pos < base_->lhs_end) {
          ptr_ = new_pos;
        } else {
          ptr_ = base_->rhs_begin + (new_pos - base_->lhs_end);
        }
      } else {
        // ptr exceeds the range
        assert(ptr_ + n < base_->rhs_end);
        ptr_ += n;
      }
      return *this;
    }
    friend constexpr iterator operator+(iterator x, difference_type n) {
      x += n;
      return x;
    }
    friend constexpr iterator operator+(difference_type n, iterator x) {
      x += n;
      return x;
    }

    constexpr iterator& operator--() {
      if (ptr_ == base_->rhs_begin) {
        ptr_ = base_->lhs_end - 1;
      } else {
        --ptr_;
      }
      return *this;
    }

    constexpr iterator operator--(int) {
      iterator temp = *this;
      --*this;
      return temp;
    }

    constexpr iterator& operator-=(difference_type n) {
      auto new_pos = ptr_ - n;
      if (base_->lhs_begin <= new_pos && new_pos < base_->lhs_end) {
        ptr_ = new_pos;
      } else {
        ptr_ = base_->rhs_begin + (new_pos - base_->lhs_end);

        // ptr_ exceeds the range
        assert(base_->rhs_begin <= ptr_ && ptr_ < base_->rhs_end);
      }
      return *this;
    }
    friend constexpr iterator operator-(iterator x, difference_type n) {
      x -= n;
      return x;
    }
    friend constexpr iterator operator-(difference_type n, iterator x) {
      x -= n;
      return x;
    }

    friend constexpr difference_type operator-(const iterator& x, const iterator& y) {
      assert(x.base_ == y.base_);

      if (x.is_left()) {
        if (y.is_left()) return x.ptr_ - y.ptr_; // both in lhs
        else return (x.ptr_ - x.base_->lhs_end) + (x.base_->rhs_begin - y.ptr_); // lhs - rhs
      } else {
        if (y.is_left()) return (x.ptr_ - x.base_->rhs_begin) + (x.base_->lhs_end - y.ptr_); // rhs - lhs
        else return x.ptr_ - y.ptr_; // both in rhs
      }
    }

    constexpr reference operator[](difference_type n) const {
      return *(*this + n);
    }
    constexpr reference operator*() const {
      return *ptr_;
    }
    constexpr pointer operator->() const {
      return ptr_;
    }

    friend constexpr bool operator==(const iterator& lhs, const iterator& rhs) {
      return lhs.ptr_ == rhs.ptr_;
    }
    friend constexpr bool operator!=(const iterator& lhs, const iterator& rhs) {
      return lhs.ptr_ != rhs.ptr_;
    }

    friend constexpr bool operator<(const iterator& x, const iterator& y) {
      if (x.is_left()) {
        if (y.is_left()) return x.ptr_ < y.ptr_;
        else return true;
      } else {
        if (y.is_left()) return false;
        else return x.ptr_ < y.ptr_;
      }
    }
    friend constexpr bool operator<=(const iterator& x, const iterator& y) {
      return !(y < x);
    }
    friend constexpr bool operator>(const iterator& x, const iterator& y) {
      return y < x;
    }
    friend constexpr bool operator>=(const iterator& x, const iterator& y) {
      return !(x < y);
    }

    constexpr bool is_left() const noexcept {
      return base_->lhs_begin <= ptr_ && ptr_ < base_->lhs_end;
    }

   private:
    string_view_concatenator* base_{};
    pointer ptr_{};
  };

  iterator begin() { return {this, lhs_begin}; }
  iterator end() { return {this, rhs_end}; }
};

} // namespace detail


// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2591r5

template<typename CharT, typename Traits, typename Allocator>
constexpr std::basic_string<CharT, Traits, Allocator>
operator+(const std::basic_string<CharT, Traits, Allocator>& lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) {
  // TODO: Benchmark this with modification though std::basic_string::data after C++17
  detail::string_view_concatenator<CharT> concatenator{lhs.data(), lhs.data() + lhs.size(), rhs.data(), rhs.data() + rhs.size()};
  std::basic_string<CharT, Traits, Allocator> result(concatenator.begin(), concatenator.end());
  return result;
}

template<typename CharT, typename Traits, typename Allocator>
constexpr std::basic_string<CharT, Traits, Allocator>
operator+(std::basic_string<CharT, Traits, Allocator>&& s, type_identity_t<basic_string_view<CharT, Traits>> sv) {
  s.append(sv.data(), sv.size());
  return std::move(s);
}

template<typename CharT, typename Traits, typename Allocator>
constexpr std::basic_string<CharT, Traits, Allocator>
operator+(type_identity_t<basic_string_view<CharT, Traits>> lhs, const std::basic_string<CharT, Traits, Allocator>& rhs) {
  detail::string_view_concatenator<CharT> concatenator{lhs.data(), lhs.data() + lhs.size(), rhs.data(), rhs.data() + rhs.size()};
  std::basic_string<CharT, Traits, Allocator> result(concatenator.begin(), concatenator.end());
  return result;
}

template<typename CharT, typename Traits, typename Allocator>
constexpr std::basic_string<CharT, Traits, Allocator>
operator+(type_identity_t<basic_string_view<CharT, Traits>> sv, std::basic_string<CharT, Traits, Allocator>&& s) {
  s.insert(0, sv.data(), sv.size());
  return std::move(s);
}

} // namespace preview

namespace std {

template<>
struct hash<preview::string_view> {
  std::size_t operator()(const preview::string_view& sv) const noexcept {
    return preview::hash_array(sv.data(), sv.size());
  }
};

template<>
struct hash<preview::wstring_view> {
  std::size_t operator()(const preview::wstring_view& sv) const noexcept {
    return preview::hash_array(sv.data(), sv.size());
  }
};

#if __cplusplus >= 202002L
template<>
struct hash<preview::u8string_view> {
  std::size_t operator()(const preview::u8string_view& sv) const noexcept {
    return preview::hash_array(sv.data(), sv.size());
  }
};
#endif

template<>
struct hash<preview::u16string_view> {
  std::size_t operator()(const preview::u16string_view& sv) const noexcept {
    return preview::hash_array(sv.data(), sv.size());
  }
};

template<>
struct hash<preview::u32string_view> {
  std::size_t operator()(const preview::u32string_view& sv) const noexcept {
    return preview::hash_array(sv.data(), sv.size());
  }
};

} // namespace std

template<typename CharT, typename Traits>
PREVIEW_SPECIALIZE_ENABLE_BORROWED_RANGE(preview::basic_string_view<CharT, Traits>) = true;

#endif // PREVIEW_STRING_VIEW_H_
