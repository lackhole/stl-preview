//
// Created by yonggyulee on 2024. 8. 23.
//

#ifndef PREVIEW_FUNCTIONAL_FOLD_H_
#define PREVIEW_FUNCTIONAL_FOLD_H_

namespace preview {

template<typename F, typename Arg, typename... Args, std::enable_if_t<(sizeof...(Args) > 0)>>
constexpr auto fold_left1(F&& f, Arg&& arg, Args&&... args) {
  return f(std::forward<Arg>(arg))
}

template<typename F, typename Arg>
constexpr auto fold_left1(F&& f, Arg&& arg) {
  return f(std::forward<Arg>(arg));
}

} // namespace preview

#endif // PREVIEW_INCLUDE_PREVIEW___FUNCTIONAL_FOLD_H_
