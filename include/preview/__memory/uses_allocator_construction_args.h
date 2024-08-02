//
// Created by yonggyulee on 2024. 8. 2.
//

#ifndef PREVIEW_MEMORY_USES_ALLOCATOR_CONSTRUCTION_ARGS_H_
#define PREVIEW_MEMORY_USES_ALLOCATOR_CONSTRUCTION_ARGS_H_

#include <memory>
#include <type_traits>
#include <tuple>
#include <utility>

#include "preview/__ranges/subrange.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/disjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_specialization.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/remove_cvref.h"
#include "preview/__tuple/apply.h"
#include "preview/__tuple/make_from_tuple.h"
#include "preview/__tuple/tuple_like.h"

namespace preview {
namespace detail {

template<typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_nonpair(tag_1, const Alloc&, Args&&... args) noexcept {
  return std::forward_as_tuple(std::forward<Args>(args)...);
}

template<typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_nonpair(tag_2, const Alloc& alloc, Args&&... args) noexcept {
  return std::tuple<std::allocator_arg_t, const Alloc&, Args&&...>(std::allocator_arg, alloc, std::forward<Args>(args)...);
}

template<typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_nonpair(tag_3, const Alloc& alloc, Args&&... args) noexcept {
  return std::forward_as_tuple(std::forward<Args>(args)..., alloc);
}

struct as_pair_niebloid {
  template<typename T, typename U>
  constexpr decltype(auto) operator()(const std::pair<T, U>& p) const { return p; }
};

} // namespace detail

template<typename T, typename Alloc, typename... Args>
constexpr T make_obj_using_allocator(const Alloc& alloc, Args&&... args);

template<typename T, typename Alloc, typename... Args, std::enable_if_t<conjunction<
    negation<is_specialization<std::remove_cv_t<T>, std::pair>>,
    disjunction<
        conjunction<
            negation<std::uses_allocator<std::remove_cv_t<T>, Alloc>>,
            std::is_constructible<T, Args...>
        >,
        conjunction<
            std::uses_allocator<std::remove_cv_t<T>, Alloc>,
            std::is_constructible<T, std::allocator_arg_t, const Alloc&, Args...>
        >,
        conjunction<
            std::uses_allocator<std::remove_cv_t<T>, Alloc>,
            std::is_constructible<T, Args..., const Alloc&>
        >
    >
>::value, int> = 0>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, Args&&... args) noexcept {
  using tag = detail::conditional_tag<
      conjunction<
          negation<std::uses_allocator<std::remove_cv_t<T>, Alloc>>,
          std::is_constructible<T, Args...>
      >,
      conjunction<
          std::uses_allocator<std::remove_cv_t<T>, Alloc>,
          std::is_constructible<T, std::allocator_arg_t, const Alloc&, Args...>
      >,
      conjunction<
          std::uses_allocator<std::remove_cv_t<T>, Alloc>,
          std::is_constructible<T, Args..., const Alloc&>
      >
  >;

  return preview::detail::uses_allocator_construction_args_nonpair<T>(tag{}, alloc, std::forward<Args>(args)...);
}

template<typename T, typename Alloc, typename Tuple1, typename Tuple2, std::enable_if_t<
    is_specialization<std::remove_cv_t<T>, std::pair>
::value, int> = 0>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, std::piecewise_construct_t, Tuple1&& x, Tuple2&& y) noexcept {
  using T1 = typename std::remove_cv_t<T>::first_type;
  using T2 = typename std::remove_cv_t<T>::second_type;

  return std::make_tuple(
      std::piecewise_construct,
      preview::apply(
          [&alloc](auto&&... args1) {
            return preview::uses_allocator_construction_args<T1>(
                alloc,
                std::forward<decltype(args1)>(args1)...
            );
          },
          std::forward<Tuple1>(x)
      ),
      preview::apply(
          [&alloc](auto&&... args2) {
            return preview::uses_allocator_construction_args<T2>(
                alloc,
                std::forward<decltype(args2)>(args2)...
            );
          },
          std::forward<Tuple2>(y)
      )
  );
}

template<typename T, typename Alloc, std::enable_if_t<
    is_specialization<std::remove_cv_t<T>, std::pair>
::value, int> = 0>
constexpr auto uses_allocator_construction_args(const Alloc& alloc) noexcept {
  return preview::uses_allocator_construction_args<T>(alloc, std::piecewise_construct, std::tuple<>{}, std::tuple<>{});
}

template<typename T, typename Alloc, typename U, typename V, std::enable_if_t<
    is_specialization<std::remove_cv_t<T>, std::pair>
::value, int> = 0>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, U&& u, V&& v) noexcept {
  return preview::uses_allocator_construction_args<T>(
      alloc,
      std::piecewise_construct,
      std::forward_as_tuple(std::forward<U>(u)),
      std::forward_as_tuple(std::forward<V>(v))
  );
}

template<typename T, typename Alloc, typename PairLike, std::enable_if_t<conjunction<
    is_specialization<std::remove_cv_t<T>, std::pair>,
    pair_like<PairLike>,
    negation<ranges::detail::is_subrange<remove_cvref_t<PairLike>>>
>::value, int> = 0>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, PairLike&& p) noexcept {
  return preview::uses_allocator_construction_args<T>(
      alloc,
      std::piecewise_construct,
      std::forward_as_tuple(std::get<0>(std::forward<PairLike>(p))),
      std::forward_as_tuple(std::get<1>(std::forward<PairLike>(p)))
  );
}

namespace detail {

template<typename T, typename Alloc, typename U>
class pair_constructor {
  using pair_type = std::remove_cv_t<T>;

  constexpr auto do_construct(const pair_type& p) const {
    return preview::make_obj_using_allocator<pair_type>(alloc_, p);
  }
  constexpr auto do_construct(pair_type&& p) const {
    return preview::make_obj_using_allocator<pair_type>(alloc_, std::move(p));
  }

  const Alloc& alloc_;
  U& u_;

 public:
  constexpr pair_constructor(const Alloc& alloc, U& u)
      : alloc_(alloc)
      , u_(u) {}

  constexpr operator pair_type() const {
    return do_construct(std::forward<U>(u_));
  }
};

} // namespace detail

template<typename T, typename Alloc, typename U, std::enable_if_t<conjunction<
    is_specialization<std::remove_cv_t<T>, std::pair>,
    disjunction<
        ranges::detail::is_subrange<remove_cvref_t<U>>,
        conjunction<
            negation<pair_like<U>>,
            negation<is_invocable<detail::as_pair_niebloid, U>>
        >
    >
>::value, int> = 0>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, U&& u) noexcept {
  return std::make_tuple(detail::pair_constructor<T, Alloc, U>{alloc, u});
}


template<typename T, typename Alloc, typename... Args>
constexpr T make_obj_using_allocator(const Alloc& alloc, Args&&... args) {
  return preview::make_from_tuple<T>(
      preview::uses_allocator_construction_args<T>(alloc, std::forward<Args>(args)...)
  );
}

} // namespace preview

#endif // PREVIEW_MEMORY_USES_ALLOCATOR_CONSTRUCTION_ARGS_H_
