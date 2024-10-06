//
// Created by yonggyulee on 2024. 10. 5.
//

#ifndef PREVIEW_ANY_BAD_ANY_CAST_H_
#define PREVIEW_ANY_BAD_ANY_CAST_H_

#include <typeinfo>

#include "preview/core.h"

#if PREVIEW_CXX_VERSION < 17

namespace preview {

class bad_any_cast : public std::bad_cast {
 public:
  bad_any_cast() noexcept = default;
  bad_any_cast(const bad_any_cast&) noexcept = default;

  const char* what() const noexcept override {
    return "preview::bad_any_cast";
  }
};

} // namespace preview

#else

#include <any>

namespace preview {

using std::bad_any_cast;

} // namespace preview

#endif

#endif // PREVIEW_ANY_BAD_ANY_CAST_H_
