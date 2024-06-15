//
// Created by yonggyulee on 2023/12/24.
//

#ifndef PREVIEW_RANGES_BORROWED_RANGE_H_
#define PREVIEW_RANGES_BORROWED_RANGE_H_

#include <type_traits>

#include "preview/__ranges/__forward_declare.h"
#include "preview/__ranges/enable_borrowed_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace ranges {

template<typename R>
struct borrowed_range
    : conjunction<
        range<R>,
        disjunction<
          std::is_lvalue_reference<R>,
          enable_borrowed_range<remove_cvref_t<R>>
        >
      > {};

} // namespace preview
} // namespace ranges

#endif // PREVIEW_RANGES_BORROWED_RANGE_H_
