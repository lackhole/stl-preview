//
// Created by lux on 2024. 7. 13..
//

#ifndef PREVIEW_RANGES_DETAIL_CONTAINER_APPENDER_H_
#define PREVIEW_RANGES_DETAIL_CONTAINER_APPENDER_H_

#include <type_traits>
#include <utility>

#include "preview/__ranges/range.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/void_t.h"

namespace preview {
namespace ranges {
namespace detail {

template<typename C, typename Ref, typename = void>
struct mem_fn_emplace_back_callable : std::false_type {};
template<typename C, typename Ref>
struct mem_fn_emplace_back_callable<C, Ref, void_t<decltype(
    std::declval<C&>().emplace_back(std::declval<Ref>())
    )>> : std::true_type {};

template<typename C, typename Ref, typename = void>
struct mem_fn_push_back_callable : std::false_type {};
template<typename C, typename Ref>
struct mem_fn_push_back_callable<C, Ref, void_t<decltype(
    std::declval<C&>().push_back(std::declval<Ref>())
    )>> : std::true_type {};

template<typename C, typename = void>
struct has_mem_fn_end : std::false_type {};
template<typename C>
struct has_mem_fn_end<C, void_t<decltype(std::declval<C&>().end())>>
    : is_referencable<decltype(std::declval<C&>().end())> {};

template<typename C, typename Ref, bool = has_mem_fn_end<C>::value /* false */, typename = void>
struct mem_fn_emplace_callable : std::false_type {};
template<typename C, typename Ref>
struct mem_fn_emplace_callable<C, Ref, true, void_t<decltype(
    std::declval<C&>().emplace(std::declval<C&>().end(), std::declval<Ref>())
    )>> : std::true_type {};

template<typename C, typename Ref, bool = has_mem_fn_end<C>::value /* false */, typename = void>
struct mem_fn_insert_callable : std::false_type {};
template<typename C, typename Ref>
struct mem_fn_insert_callable<C, Ref, true, void_t<decltype(
    std::declval<C&>().insert(std::declval<C&>().end(), std::declval<Ref>())
    )>> : std::true_type {};

template<typename Container>
class container_appender_object {
  template<typename Reference>
  using tag_t = preview::detail::conditional_tag<
      mem_fn_emplace_back_callable<Container, Reference>,
      mem_fn_push_back_callable<Container, Reference>,
      mem_fn_emplace_callable<Container, Reference>,
      mem_fn_insert_callable<Container, Reference>
  >;

 public:
  constexpr explicit container_appender_object(Container& container) noexcept
      : container_(container) {}

  template<typename Reference, std::enable_if_t<(tag_t<Reference>::value > 0), int> = 0>
  constexpr void operator()(Reference&& r) {
    call(std::forward<Reference>(r), tag_t<Reference>{});
  }

 private:
  using tag_1 = preview::detail::tag_1;
  using tag_2 = preview::detail::tag_2;
  using tag_3 = preview::detail::tag_3;
  using tag_4 = preview::detail::tag_4;

  template<typename Reference>
  void call(Reference&& r, tag_1) {
    container_.emplace_back(std::forward<Reference>(r));
  }

  template<typename Reference>
  void call(Reference&& r, tag_2) {
    container_.push_back(std::forward<Reference>(r));
  }

  template<typename Reference>
  void call(Reference&& r, tag_3) {
    container_.emplace(container_.end(), std::forward<Reference>(r));
  }

  template<typename Reference>
  void call(Reference&& r, tag_4) {
    container_.insert(container_.end(), std::forward<Reference>(r));
  }

  Container& container_;
};

} // namespace detail

template<typename Container, typename Reference>
struct container_appendable
    : is_invocable<detail::container_appender_object<Container>, Reference> {};

template<typename Container>
constexpr auto container_appender(Container& c) noexcept {
  return detail::container_appender_object<Container>{c};
}

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_DETAIL_CONTAINER_APPENDER_H_
