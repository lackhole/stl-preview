//
// Created by yonggyulee on 2024. 7. 18.
//

#ifndef PREVIEW_ATOMIC_ATOMIC_H_
#define PREVIEW_ATOMIC_ATOMIC_H_

#include <atomic>
#include <type_traits>


#include "preview/__atomic/detail/atomic_floating_point.h"
#include "preview/__atomic/detail/atomic_generic.h"
#include "preview/__atomic/detail/atomic_integral.h"
#include "preview/__atomic/detail/atomic_pointer.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/conditional.h"

namespace preview {
namespace detail {

template<typename T, bool = disjunction<std::is_integral<T>, std::is_floating_point<T>>::value>
struct atomic_difference_type {};
template<typename T>
struct atomic_difference_type<T, true> { using type = T; };

template<typename T>
using atomic_base_t = conditional_t<
    std::is_integral<T>,       atomic_integral<T>,
    std::is_floating_point<T>, atomic_floating_point<T>,
    std::is_pointer<T>,        atomic_pointer<T>,
    atomic_generic<T>
>;

} // namespace detail

template<typename T>
class atomic : public detail::atomic_base_t<T> {
  using base = detail::atomic_base_t<T>;
public:
  using base::base;
};

} // namespace preview

#endif // PREVIEW_ATOMIC_ATOMIC_H_
