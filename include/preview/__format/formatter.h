//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_FORMAT_FORMATTER_H_
#define PREVIEW_FORMAT_FORMATTER_H_

#include "preview/__format/basic_format_parse_context.h"
#include "preview/__format/basic_format_context.h"
#include "preview/__format/format_spec.h"

namespace preview {
namespace detail {

template<typename T, typename CharT = char>
class formatter_base {
 public:

  constexpr typename basic_format_parse_context<CharT>::iterator
  parse(basic_format_parse_context<CharT>& parse_ctx) {
    return {};
  }

  template<typename OutputIt>
  constexpr typename basic_format_context<OutputIt, CharT>::iterator
  format(const T& arg, basic_format_context<OutputIt, CharT>& fmt_ctx) const {
    return {};
  }
};

}

template<typename T, typename CharT = char>
struct formatter;



} // namespace preview

#endif // PREVIEW_FORMAT_FORMATTER_H_
