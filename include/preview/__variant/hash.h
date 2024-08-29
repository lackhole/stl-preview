//
// Created by YongGyu Lee on 02/01/24.
//

#ifndef PREVIEW_VARIANT_HASH_H_
#define PREVIEW_VARIANT_HASH_H_

#include <functional>

#include "preview/__functional/hash_array.h"
#include "preview/__variant/variant.h"
#include "preview/__variant/variant_npos.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/remove_cvref.h"

namespace preview {
namespace detail {

struct variant_hash_visitor {
  template<typename T, std::size_t I>
  constexpr std::size_t operator()(const T& x, in_place_index_t<I>) const {
    return preview::FNV_1a(std::hash<T>{}(x), I);
  }

  template<typename T>
  constexpr std::size_t operator()(const T&, in_place_index_t<variant_npos>) const {
    return 0;
  }
};

template<typename Variant, bool Hashable /* false */>
struct variant_hash {};

template<typename Variant>
struct variant_hash<Variant, true> {
  std::size_t operator()(const Variant& var) const {
    return variant_raw_visit(var.index(), var._base().union_, variant_hash_visitor{});
  }
};

} // namespace detail
} // namespace preview

template<typename... Ts>
struct std::hash<preview::variant<Ts...>>
    : preview::detail::variant_hash<
        preview::variant<Ts...>,
        preview::conjunction<preview::is_invocable<std::hash<Ts>, const preview::remove_cvref_t<Ts>&>...>::value
      > {};

#endif // PREVIEW_VARIANT_HASH_H_
