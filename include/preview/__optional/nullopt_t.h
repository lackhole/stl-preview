# /*
#  * Created by YongGyu Lee on 2021/05/23.
#  */
#
# ifndef PREVIEW_OPTIONAL_NULLOPT_T_H_
# define PREVIEW_OPTIONAL_NULLOPT_T_H_

namespace preview {

struct nullopt_t {
  constexpr explicit nullopt_t(int) {}
};

constexpr nullopt_t nullopt{0};

} // namespace preview

# endif // PREVIEW_OPTIONAL_NULLOPT_T_H_
