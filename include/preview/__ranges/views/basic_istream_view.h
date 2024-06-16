//
// Created by yonggyulee on 2/3/24.
//

#ifndef PREVIEW_RANGES_VIEWS_BASIC_ISTREAM_VIEW_H_
#define PREVIEW_RANGES_VIEWS_BASIC_ISTREAM_VIEW_H_

#include <cstddef>
#include <istream>
#include <string>
#include <type_traits>

#include "preview/__concepts/default_initializable.h"
#include "preview/__concepts/movable.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/default_sentinel_t.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename Val, typename CharT, typename Traits, typename = void>
struct stream_extractable : std::false_type {};

template<typename Val, typename CharT, typename Traits>
struct stream_extractable<Val, CharT, Traits,
    void_t<decltype( std::declval<std::basic_istream<CharT, Traits>&>() >> std::declval<Val&>() )>>
    : std::true_type {};

} //namespace detail

template<typename Val, typename CharT, typename Traits = std::char_traits<CharT>>
class basic_istream_view : public view_interface<basic_istream_view<Val, CharT, Traits>> {
 public:
  static_assert(movable<Val>::value, "Constraints not satisfied");
  static_assert(default_initializable<Val>::value, "Constraints not satisfied");
  static_assert(detail::stream_extractable<Val, CharT, Traits>::value, "Constraints not satisfied");

  struct iterator {
    using iterator_concept = input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Val;
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using iterator_category = iterator_ignore;
    using pointer = void;
    using reference = Val&;
#endif

    constexpr explicit iterator(basic_istream_view& parent)
        : parent_(preview::addressof(parent)) {}
    iterator(const iterator&) = delete;
    iterator(iterator&& other) noexcept
        : parent_(other.parent_) {
      other.parent_ = nullptr;
    }

    iterator& operator=(const iterator&) = delete;
    iterator& operator=(iterator&& other) noexcept {
      if (this != preview::addressof(other)) {
        parent_ = other.parent_;
        other.parent_ = nullptr;
      }
      return *this;
    }

    iterator& operator++() {
      parent_->read();
      return *this;
    }

    void operator++(int) {
      parent_->read();
    }

    Val& operator*() const {
      return parent_->value_;
    }

    friend bool operator==(const iterator& x, default_sentinel_t) {
      return x.parent_ == nullptr || x.fail();
    }

    friend bool operator!=(const iterator& x, default_sentinel_t) {
      return !(x == default_sentinel);
    }

    friend bool operator==(default_sentinel_t, const iterator& x) {
      return x == default_sentinel;
    }

    friend bool operator!=(default_sentinel_t, const iterator& x) {
      return !(x == default_sentinel);
    }

   private:
    bool fail() const {
      return parent_->stream_->fail();
    }

    basic_istream_view* parent_;
  };
  friend struct iterator;


  PREVIEW_CONSTEXPR_AFTER_CXX17 explicit basic_istream_view(std::basic_istream<CharT, Traits>& stream)
      : stream_(std::addressof(stream)), value_() {}

  constexpr auto begin() {
    read();
    return iterator{*this};
  }

  constexpr default_sentinel_t end() const noexcept {
    return default_sentinel;
  }

 private:
  void read() {
    *stream_ >> value_;
  }

  std::basic_istream<CharT, Traits>* stream_;
  Val value_;
};

template<typename Val> using istream_view = basic_istream_view<Val, char>;
template<typename Val> using wistream_view = basic_istream_view<Val, wchar_t>;

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_BASIC_ISTREAM_VIEW_H_
