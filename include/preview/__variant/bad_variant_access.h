//
// Created by YongGyu Lee on 11/3/23.
//

#ifndef PREVIEW_VARIANT_BAD_VARIANT_ACCESS_H_
#define PREVIEW_VARIANT_BAD_VARIANT_ACCESS_H_

#include <exception>

namespace preview {

class bad_variant_access : public std::exception {
 public:
  bad_variant_access() noexcept = default;
  bad_variant_access(const bad_variant_access&) noexcept = default;

  const char* what() const noexcept override {
    return "preview::variant: Bad variant access";
  }
};

} // namespace preview

#endif // PREVIEW_VARIANT_BAD_VARIANT_ACCESS_H_
