//
// Created by YongGyu Lee on 2/20/24.
//

#ifndef PREVIEW_TUPLE_TUPLE_TRANSFORM_H_
#define PREVIEW_TUPLE_TUPLE_TRANSFORM_H_

#include <tuple>
#include <utility>

#include "preview/__functional/invoke.h"
#include "preview/__tuple/apply.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {

template<typename Tuple, typename F>
constexpr auto tuple_transform(Tuple&& t, F&& f) {
  return preview::apply([&](auto&&... args)
  {
    return std::tuple<invoke_result_t<F&, decltype(args)>...>
        (preview::invoke(f, std::forward<decltype(args)>(args))...);
  }, std::forward<Tuple>(t));
}

} // namespace preview

#endif // PREVIEW_TUPLE_TUPLE_TRANSFORM_H_
