//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_FORMAT_BASIC_FORMAT_STRING_H_
#define PREVIEW_FORMAT_BASIC_FORMAT_STRING_H_

#include "preview/core.h"

#include <string>
#if PREVIEW_CXX_VERSION >= 17
#include <string_view>
#endif

#include "preview/string_view.h"

namespace preview {

template<class CharT, class... Args>
struct basic_format_string {
 public:
  template<std::size_t N>
  PREVIEW_CONSTEVAL basic_format_string(const CharT (&s)[N])
      : str(s, N - 1) {}

#if PREVIEW_CXX_VERSION < 17
  basic_format_string(std::basic_string<CharT> s) noexcept
      : str(s.str) {}
#else
  basic_format_string(std::basic_string_view<CharT> s) noexcept
      : str(s.data(), s.size()) {}
#endif

  basic_format_string(basic_string_view<CharT> s) noexcept
      : str(s) {}

  constexpr basic_string_view<CharT> get() const noexcept { return str; }

 private:
  basic_string_view<CharT> str;
};

} // namespace preview

#endif // PREVIEW_FORMAT_BASIC_FORMAT_STRING_H_
