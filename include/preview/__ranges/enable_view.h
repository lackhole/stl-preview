//
// Created by yonggyulee on 2023/12/30.
//

#ifndef PREVIEW_RANGES_ENABLE_VIEW_H_
#define PREVIEW_RANGES_ENABLE_VIEW_H_

#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/derived_from_single_crtp.h"
#include "preview/__ranges/__forward_declare.h"
#include "preview/__ranges/view_base.h"
#include "preview/__type_traits/disjunction.h"

namespace preview {
namespace ranges {

template<typename T>
struct enable_view : disjunction<
          derived_from<T, view_base>,
          derived_from_single_crtp<T, view_interface>
      > {};

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_ENABLE_VIEW_H_
