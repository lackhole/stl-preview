//
// Created by yonggyulee on 2024. 10. 31.
//

#ifndef PREVIEW_BITSET_BITSET_H_
#define PREVIEW_BITSET_BITSET_H_

#include <cstddef>

#include "preview/__core/constexpr.h"

namespace preview {

template<std::size_t>
class bitset {
 public:
  class reference {
   public:

    constexpr reference& operator=(bool x) noexcept {
      return *this;
    }
    constexpr reference& operator=( const reference& x ) noexcept {

    }

    constexpr operator bool() const noexcept {
      return false;
    }

    constexpr bool operator~() const noexcept {
      return ~(this->operator bool());
    }

    constexpr reference& flip() noexcept {
      return *this = ~*this;
    }

   private:
    friend class bitset;
    reference() = default;
  };
};

} // namespace preview

#endif // PREVIEW_BITSET_BITSET_H_
