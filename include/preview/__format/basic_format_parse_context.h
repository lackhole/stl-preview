//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_FORMAT_BASIC_FORMAT_PARSE_CONTEXT_H_
#define PREVIEW_FORMAT_BASIC_FORMAT_PARSE_CONTEXT_H_

#include "preview/__format/format_error.h"
#include "preview/string_view.h"

namespace preview {

template<typename CharT>
class basic_format_parse_context {
 public:
  using char_type = CharT;
  using iterator = typename basic_string_view<CharT>::const_iterator;
  using const_iterator = typename basic_string_view<CharT>::const_iterator;

  constexpr explicit basic_format_parse_context(basic_string_view<CharT> fmt) noexcept
      : begin_(fmt.begin())
      , end_(fmt.end())
      , indexing_(unknown)
      , next_arg_id_(0)
      , num_args_(0) {}

  basic_format_parse_context(const basic_format_parse_context&) = delete;
  basic_format_parse_context& operator=(const basic_format_parse_context&) = delete;

  constexpr const_iterator begin() const noexcept {
    return begin_;
  }

  constexpr const_iterator end() const noexcept {
    return end_;
  }

  constexpr void advance_to(const_iterator it) {
    begin_ = it;
  }

  constexpr size_t next_arg_id() {
    if (indexing_ == manual)
      throw_mixing();
    else if (indexing_ == unknown)
      indexing_ = automatic;

    return next_arg_id_++;
  }

  constexpr void check_arg_id(std::size_t id) {
    if (indexing_ == automatic)
      throw_mixing();
    else if (indexing_ == unknown)
      indexing_ = manual;

  }

  template<typename... Ts>
  constexpr void check_dynamic_spec(std::size_t id) noexcept {

  }

  constexpr void check_dynamic_spec_integral(std::size_t id) noexcept {
    check_dynamic_spec<int, unsigned int, long long int, unsigned long long int>(id);
  }

  constexpr void check_dynamic_spec_string(std::size_t id) noexcept {
    check_dynamic_spec<const char_type*, basic_string_view<char_type>>(id);
  }

 private:
  constexpr void throw_mixing() const {
    throw format_error{"mixing of automatic and manual argument indexing"};
  }

  enum indexing {
    unknown,
    manual,
    automatic
  };

  iterator begin_;
  iterator end_;
  indexing indexing_;
  std::size_t next_arg_id_;
  std::size_t num_args_;
};

using format_parse_context = basic_format_parse_context<char>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;

} // namespace preview

#endif // PREVIEW_FORMAT_BASIC_FORMAT_PARSE_CONTEXT_H_
