//
// Created by yonggyulee on 28/04/2025
//

#ifndef PREVIEW_RANGES_VIEWS_AS_RVALUE_H_
#define PREVIEW_RANGES_VIEWS_AS_RVALUE_H_

#include <type_traits>
#include <utility>

#include "preview/__concepts/same_as.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/range_rvalue_reference_t.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/as_rvalue_view.h"
#include "preview/__type_traits/conjunction.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct as_rvalue_adaptor_object : range_adaptor_closure<as_rvalue_adaptor_object> {
 private:
  template<typename R>
  constexpr auto call(R&& r, std::true_type /* input_range and already rvalue */) const {
    return views::all(std::forward<R>(r));
  }
  template<typename R>
  constexpr auto call(R&& r, std::false_type /* input_range and already rvalue */) const {
    return as_rvalue_view<all_t<R>>(std::forward<R>(r));
  }

 public:
  template<typename R, std::enable_if_t<viewable_range<R>::value, int> = 0>
  constexpr auto operator()(R&& r) const {
    return call(
        std::forward<R>(r),
        conjunction<
            input_range<R>,
            same_as<range_rvalue_reference_t<R>, range_reference_t<R>>
        >{}
    );
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::as_rvalue_adaptor_object as_rvalue{};

} // namespace views
} // namespace ranges
} // namespace preview

} // namespace views
} // namespace ranges
} // namespace preview 

#endif // PREVIEW_RANGES_VIEWS_AS_RVALUE_H_
