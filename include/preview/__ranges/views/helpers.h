//
// Created by yonggyulee on 14/04/2025
//

#ifndef PREVIEW_RANGES_VIEWS_HELPERS_H_
#define PREVIEW_RANGES_VIEWS_HELPERS_H_

#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/maybe_const.h"

namespace preview {
namespace ranges {
namespace detail {

template<bool Const, typename... Views>
using all_forward = conjunction<forward_range<maybe_const<Const, Views>>...>;

template<bool Const, typename... Views>
using all_bidirectional = conjunction<bidirectional_range<maybe_const<Const, Views>>...>;

template<bool Const, typename... Views>
using all_random_access = conjunction<random_access_range<maybe_const<Const, Views>>...>;

} // namespace detail
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_HELPERS_H_
