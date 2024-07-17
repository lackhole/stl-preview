//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_FORMAT_BASIC_FORMAT_CONTEXT_H_
#define PREVIEW_FORMAT_BASIC_FORMAT_CONTEXT_H_

#include "preview/__iterator/output_iterator.h"

namespace preview {

template<typename OutputIt, typename CharT>
class basic_format_context {
 public:
  static_assert(output_iterator<OutputIt, const CharT&>::value, "Constraints not satisfied");

  using iterator = OutputIt;
  using char_type = CharT;

};

} // namespace preview

#endif // PREVIEW_FORMAT_BASIC_FORMAT_CONTEXT_H_
