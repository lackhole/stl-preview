//
// Created by YongGyu Lee on 2024. 6. 15..
//

#ifndef PREVIEW_UTILITY_TO_UNDERLYING_H_
#define PREVIEW_UTILITY_TO_UNDERLYING_H_

namespace preview {

template<typename Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

} // namespace preview

#endif // PREVIEW_UTILITY_TO_UNDERLYING_H_
