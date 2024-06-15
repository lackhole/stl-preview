# /*
#  * Created by YongGyu Lee on 2021/05/30.
#  */
#
#
# ifndef PREVIEW_OPTIONAL_INTERNAL_CHECK_OVERLOAD_H_
# define PREVIEW_OPTIONAL_INTERNAL_CHECK_OVERLOAD_H_
#
# include <type_traits>


namespace preview {
namespace internal {
namespace optional {

template<typename T, bool v = std::is_copy_constructible<T>::value>
struct check_copy_constructible {};
template<typename T>
struct check_copy_constructible<T, false> {
  check_copy_constructible() = default;
  check_copy_constructible(check_copy_constructible const&) = delete;
  check_copy_constructible(check_copy_constructible &&) = default;
  check_copy_constructible& operator=(check_copy_constructible const&) = default;
  check_copy_constructible& operator=(check_copy_constructible &&) = default;
};

template<typename T, bool v = std::is_move_constructible<T>::value>
struct check_move_constructible {};
template<typename T>
struct check_move_constructible<T, false> {
  check_move_constructible() = default;
  check_move_constructible(check_move_constructible const&) = default;
  check_move_constructible(check_move_constructible &&) = delete;
  check_move_constructible& operator=(check_move_constructible const&) = default;
  check_move_constructible& operator=(check_move_constructible &&) = default;
};

template<typename T, bool v = std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value>
struct check_copy_assignable {};
template<typename T>
struct check_copy_assignable<T, false> {
  check_copy_assignable() = default;
  check_copy_assignable(check_copy_assignable const&) = default;
  check_copy_assignable(check_copy_assignable &&) = default;
  check_copy_assignable& operator=(check_copy_assignable const&) = delete;
  check_copy_assignable& operator=(check_copy_assignable &&) = default;
};

template<typename T, bool v = std::is_move_constructible<T>::value && std::is_move_assignable<T>::value>
struct check_move_assignable {};
template<typename T>
struct check_move_assignable<T, false> {
  check_move_assignable() = default;
  check_move_assignable(check_move_assignable const&) = default;
  check_move_assignable(check_move_assignable &&) = default;
  check_move_assignable& operator=(check_move_assignable const&) = default;
  check_move_assignable& operator=(check_move_assignable &&) = delete;
};



} // namespace optional
} // namespace internal
} // namespace preview

# endif // PREVIEW_OPTIONAL_INTERNAL_CHECK_OVERLOAD_H_
