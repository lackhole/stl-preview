# /*
#  * Created by YongGyu Lee on 2021/05/23.
#  */
#
# ifndef PREVIEW_OPTIONAL_NULLOPT_T_H_
# define PREVIEW_OPTIONAL_NULLOPT_T_H_

#include "preview/__core/cxx_version.h"

#if PREVIEW_CXX_VERSION >= 17
#include <optional>
#endif

namespace preview {

#if PREVIEW_CXX_VERSION >= 17

using nullopt_t = std::nullopt_t;
constexpr nullopt_t nullopt{std::nullopt};

#else

struct nullopt_t {
  struct ctor_tag {};
  constexpr explicit nullopt_t(ctor_tag, ctor_tag) {}
};

constexpr nullopt_t nullopt{nullopt_t::ctor_tag{}, nullopt_t::ctor_tag{}};

#endif


} // namespace preview

# endif // PREVIEW_OPTIONAL_NULLOPT_T_H_
