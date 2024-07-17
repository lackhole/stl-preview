//
// Created by yonggyulee on 2024. 7. 17.
//

#ifndef PREVIEW_FORMAT_FORMAT_ERROR_H_
#define PREVIEW_FORMAT_FORMAT_ERROR_H_

#include <stdexcept>
#include <string>

namespace preview {

class format_error : public std::runtime_error {
 public:
  using runtime_error::runtime_error;

  format_error(const format_error& other) noexcept = default;
  format_error& operator=(const format_error& other) noexcept = default;
};

} // namespace preview

#endif // PREVIEW_FORMAT_FORMAT_ERROR_H_
