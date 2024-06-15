//
// Created by YongGyu Lee on 4/11/24.
//

#ifndef PREVIEW_RANGES_VIEWS_ZIP_TRANSFORM_H_
#define PREVIEW_RANGES_VIEWS_ZIP_TRANSFORM_H_

#include <type_traits>

#include "preview/__concepts/copy_constructible.h"
#include "preview/__concepts/invocable.h"
#include "preview/__core/decay_copy.h"
#include "preview/__core/inline_variable.h"
#include "preview/__ranges/views/zip_transform_view.h"
#include "preview/__ranges/range_adaptor.h"
#include "preview/__ranges/range_adaptor_closure.h"
#include "preview/__ranges/views/empty_view.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_referenceable.h"

namespace preview {
namespace ranges {
namespace views {
namespace detail {

struct zip_transform_niebloid {
 private:
  template<typename FD, bool = conjunction<copy_constructible<FD>, is_referencable<FD>>::value /* false */>
  struct check : std::false_type {};

  template<typename FD>
  struct check<FD, true>
      : conjunction<
          regular_invocable<FD&>,
          std::is_object<invoke_result_t<FD&>>
      > {};

 public:
  template<typename F, std::enable_if_t<check<std::decay_t<F&&>>::value, int> = 0>
  constexpr auto operator()(F&& f) const {
    using FD = std::decay_t<decltype(f)>;

    return ((void)f, vccc_decay_copy(views::empty<std::decay_t<invoke_result_t<FD&>>>));
  }

  template<typename F, typename... Rs>
  constexpr auto operator()(F&& f, Rs&&... rs) const {
    return zip_transform_view<std::decay_t<F>, views::all_t<Rs>...>(std::forward<F>(f), std::forward<Rs>(rs)...);
  }
};

} // namespace detail

PREVIEW_INLINE_VARIABLE constexpr detail::zip_transform_niebloid zip_transform{};

} // namespace views
} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_ZIP_TRANSFORM_H_
