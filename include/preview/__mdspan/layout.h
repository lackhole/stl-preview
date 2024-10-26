//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_LAYOUT_H_
#define PREVIEW_MDSPAN_LAYOUT_H_

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "preview/__concepts/same_as.h"
#include "preview/__core/std_version.h"
#include "preview/__mdspan/extents.h"
#include "preview/__mdspan/index_pair_like.h"
#include "preview/__mdspan/submdspan_extents.h"
#include "preview/__span/dynamic_extent.h"
#include "preview/__type_traits/bool_constant.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/detail/tag.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/negation.h"
#include "preview/__type_traits/void_t.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/in_place.h"

namespace preview {
namespace detail {

template<typename Layout, typename Mapping>
struct is_mapping_of : std::is_same<typename Layout::template mapping<Mapping::extents_type>, Mapping> {};

template<typename Mapping, typename = void>
struct is_layout_left_padded_mapping_of : std::false_type {};

template<typename Mapping, typename = void>
struct is_layout_right_padded_mapping_of : std::false_type {};

template<typename T>
struct is_extents : std::false_type {};
template<typename IndexType, std::size_t... Args>
struct is_extents<extents<IndexType, Args...>> : std::true_type {};

// TODO: Make concept

struct constexpr_is_always_strided_tester {
  template<typename M>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype( M::is_always_strided() ), M::is_always_strided()>{}, std::true_type{});
  template<typename M>
  static constexpr auto test(...) -> std::false_type;
};

template<typename M>
struct has_constexpr_static_fn_is_always_strided_impl
    : decltype(constexpr_is_always_strided_tester::test<std::remove_reference_t<M>>(0)) {};

template<typename M, bool = /* false */ has_constexpr_static_fn_is_always_strided_impl<M>::value>
struct has_constexpr_static_fn_is_always_strided : std::false_type {};
template<typename M>
struct has_constexpr_static_fn_is_always_strided<M, true> : same_as<decltype( M::is_always_strided() ), bool> {};

struct constexpr_is_always_exhaustive_tester {
  template<typename M>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype( M::is_always_exhaustive() ), M::is_always_exhaustive()>{}, std::true_type{});
  template<typename M>
  static constexpr auto test(...) -> std::false_type;
};

template<typename M>
struct has_constexpr_static_fn_is_always_exhaustive_impl
    : decltype(constexpr_is_always_exhaustive_tester::test<std::remove_reference_t<M>>(0)) {};

template<typename M, bool = /* false */ has_constexpr_static_fn_is_always_exhaustive_impl<M>::value>
struct has_constexpr_static_fn_is_always_exhaustive : std::false_type {};
template<typename M>
struct has_constexpr_static_fn_is_always_exhaustive<M, true> : same_as<decltype( M::is_always_exhaustive() ), bool> {};

struct constexpr_is_always_unique_tester {
  template<typename M>
  static constexpr auto test(int)
      -> decltype(std::integral_constant<decltype( M::is_always_unique() ), M::is_always_unique()>{}, std::true_type{});
  template<typename M>
  static constexpr auto test(...) -> std::false_type;
};

template<typename M>
struct has_constexpr_static_fn_is_always_unique_impl
    : decltype(constexpr_is_always_unique_tester::test<std::remove_reference_t<M>>(0)) {};

template<typename M, bool = /* false */ has_constexpr_static_fn_is_always_unique_impl<M>::value>
struct has_constexpr_static_fn_is_always_unique : std::false_type {};
template<typename M>
struct has_constexpr_static_fn_is_always_unique<M, true> : same_as<decltype( M::is_always_unique() ), bool> {};

template<typename M, typename = void>
struct has_typename_extents_type : std::false_type {};
template<typename M>
struct has_typename_extents_type<M, void_t<typename M::extents_type>> : std::true_type {};

template<typename M, bool = /* false */ conjunction<
    has_typename_extents_type<M>,
    has_constexpr_static_fn_is_always_strided<M>,
    has_constexpr_static_fn_is_always_exhaustive<M>,
    has_constexpr_static_fn_is_always_unique<M>
>::value>
struct layout_mapping_alike : std::false_type {};

template<typename M>
struct layout_mapping_alike<M, true>
    : conjunction<
        is_extents<typename M::extents_type>,
        bool_constant<M::is_always_strided()>,
        bool_constant<M::is_always_exhaustive()>,
        bool_constant<M::is_always_unique()>
    > {};

} // namespace detail


struct layout_left {
  template<typename Extents>
  class mapping;
};

struct layout_right {
  template<typename Extents>
  class mapping;
};

struct layout_stride {
  template<typename Extents>
  class mapping;
};

template<typename Extents>
class layout_left::mapping {
 public:
  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_left;

  constexpr mapping() noexcept = default;
  constexpr mapping(const mapping&) noexcept = default;
  constexpr mapping(const extents_type& e) noexcept
      : extents_(e) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ negation<std::is_convertible<OtherExtents, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const mapping<OtherExtents>& other) noexcept
      : extents_(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>
  >::value, int> = 0>
  constexpr mapping(const mapping<OtherExtents>& other) noexcept
      : extents_(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() <= 1)>,
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ negation<std::is_convertible<OtherExtents, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const layout_right::mapping<OtherExtents>& other) noexcept;

  template<typename OtherExtents, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() <= 1)>,
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>
  >::value, int> = 0>
  constexpr mapping(const layout_right::mapping<OtherExtents>& other) noexcept;

  template<typename LayoutLeftPaddedMapping, std::enable_if_t<conjunction<
      detail::is_layout_left_padded_mapping_of<LayoutLeftPaddedMapping>,
      std::is_constructible<extents_type, typename LayoutLeftPaddedMapping::extents_type>,
      /* explicit(true) */ negation<std::is_convertible<typename LayoutLeftPaddedMapping::extents_type, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const LayoutLeftPaddedMapping& other) noexcept
      : extents_(other.extents()) {}

  template<typename LayoutLeftPaddedMapping, std::enable_if_t<conjunction<
      detail::is_layout_left_padded_mapping_of<LayoutLeftPaddedMapping>,
      std::is_constructible<extents_type, typename LayoutLeftPaddedMapping::extents_type>,
      /* explicit(false) */ std::is_convertible<typename LayoutLeftPaddedMapping::extents_type, extents_type>
  >::value, int> = 0>
  constexpr mapping(const LayoutLeftPaddedMapping& other) noexcept
      : extents_(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ bool_constant<(extents_type::rank() > 0)>
  >::value, int> = 0>
  constexpr explicit mapping(const layout_stride::mapping<OtherExtents>& other);

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ bool_constant<(extents_type::rank() <= 0)>
  >::value, int> = 0>
  constexpr mapping(const layout_stride::mapping<OtherExtents>& other);

  constexpr mapping& operator=(const mapping&) noexcept = default;

  // [mdspan.layout.left.obs], observers
  constexpr const extents_type& extents() const noexcept { return extents_; }

  constexpr index_type required_span_size() const noexcept {
    detail::extents_prod_helper::fwd_prod_of_extents(extents(), extents_type::rank());
  }

  template<typename... Indices, std::enable_if_t<conjunction<
      bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_constructible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >::value, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(std::index_sequence_for<Indices...>{}, std::move(i)...);
  }

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return true; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  static constexpr bool is_exhaustive() noexcept { return true; }
  static constexpr bool is_strided() noexcept { return true; }

  template<typename Dummy = void, std::enable_if_t<std::is_void<Dummy>::value && (extents_type::rank() > 0), int> = 0>
  constexpr index_type stride(rank_type i) const noexcept {
    return detail::extents_prod_helper::fwd_prod_of_extents(extents(), i);
  }

  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator==(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return x.extents() == y.extents();
  }



  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator!=(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return !(x == y);
  }

  // TODO
//  template<typename... SliceSpecifiers, std::enable_if_t<(sizeof...(SliceSpecifiers) == extents_type::rank()), int> = 0>
//  friend constexpr auto submdspan_mapping(const mapping& src, SliceSpecifiers... slices) {
//
//
//    using tag = preview::detail::conditional_tag<
//        bool_constant<(Extents::rank() == 0)>,
//        bool_constant<(SubExtents::rank() == 0)>
//    >;
//
//    return src.submdspan_mapping_impl(slices...);
//  }

 private:
  template<std::size_t... P, typename... Indices>
  constexpr index_type call(std::index_sequence<P...>, Indices... i) const noexcept {
#if PREVIEW_CXX_VERSION < 17
    index_type result = 0;
    using parray = detail::static_array<std::size_t, P...>;
    index_type iarray[sizeof...(Indices)] = {static_cast<index_type>(i)...};
    for (std::size_t j = 0; j < parray::size; ++j) {
      result += iarray[j] * stride(parray::data[j]);
    }
    return result;
#else
    return ((static_cast<index_type>(i) * stride(P)) + ... + 0);
#endif
  }

  extents_type extents_{};                                               // exposition only

  // [mdspan.sub.map], submdspan mapping specialization
//  template<typename... SliceSpecifiers>
//  constexpr auto submdspan_mapping_impl(SliceSpecifiers... slices) const {
//    // TODO
//    return 0;
//  }
};

template<typename Extents>
class layout_right::mapping {
 public:
  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_right;

  constexpr mapping() noexcept = default;
  constexpr mapping(const mapping&) noexcept = default;
  constexpr mapping(const extents_type& e) noexcept
      : extents_(e) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ negation<std::is_convertible<OtherExtents, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const mapping<OtherExtents>& other) noexcept
      : extents_(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>
  >::value, int> = 0>
  constexpr mapping(const mapping<OtherExtents>& other) noexcept
      : extents_(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() <= 1)>,
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ negation<std::is_convertible<OtherExtents, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const layout_left::mapping<OtherExtents>& other) noexcept;

  template<typename OtherExtents, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() <= 1)>,
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ std::is_convertible<OtherExtents, extents_type>
  >::value, int> = 0>
  constexpr mapping(const layout_left::mapping<OtherExtents>& other) noexcept;

  template<typename LayoutRightPaddedMapping, std::enable_if_t<conjunction<
      detail::is_layout_right_padded_mapping_of<LayoutRightPaddedMapping>,
      std::is_constructible<extents_type, typename LayoutRightPaddedMapping::extents_type>,
      /* explicit(true) */ negation<std::is_convertible<typename LayoutRightPaddedMapping::extents_type, extents_type>>
  >::value, int> = 0>
  constexpr explicit mapping(const LayoutRightPaddedMapping& other) noexcept
      : extents_(other.extents()) {}

  template<typename LayoutRightPaddedMapping, std::enable_if_t<conjunction<
      detail::is_layout_right_padded_mapping_of<LayoutRightPaddedMapping>,
      std::is_constructible<extents_type, typename LayoutRightPaddedMapping::extents_type>,
      /* explicit(false) */ std::is_convertible<typename LayoutRightPaddedMapping::extents_type, extents_type>
  >::value, int> = 0>
  constexpr mapping(const LayoutRightPaddedMapping& other) noexcept
      : extents_(other.extents()) {}

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(true) */ bool_constant<(extents_type::rank() > 0)>
  >::value, int> = 0>
  constexpr explicit mapping(const layout_stride::mapping<OtherExtents>& other) noexcept;

  template<typename OtherExtents, std::enable_if_t<conjunction<
      std::is_constructible<extents_type, OtherExtents>,
      /* explicit(false) */ bool_constant<(extents_type::rank() <= 0)>
  >::value, int> = 0>
  constexpr mapping(const layout_stride::mapping<OtherExtents>& other) noexcept;

  constexpr mapping& operator=(const mapping&) noexcept = default;

  // [mdspan.layout.right.obs], observers
  constexpr const extents_type& extents() const noexcept { return extents_; }

  constexpr index_type required_span_size() const noexcept {
    return detail::extents_prod_helper::fwd_prod_of_extents(extents(), extents_type::rank());
  }

  template<typename... Indices, std::enable_if_t<conjunction<
      bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_constructible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >::value, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(std::index_sequence_for<Indices...>{}, std::move(i)...);
  }

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return true; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  static constexpr bool is_exhaustive() noexcept { return true; }
  static constexpr bool is_strided() noexcept { return true; }

  template<typename Dummy = void, std::enable_if_t<std::is_void<Dummy>::value && (extents_type::rank() > 0), int> = 0>
  constexpr index_type stride(rank_type i) const noexcept {
    return detail::extents_prod_helper::fwd_prod_of_extents(extents(), i);
  }

  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator==(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return x.extents() == y.extents();
  }

  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator!=(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return !(x == y);
  }

 private:
  template<std::size_t... P, typename... Indices>
  constexpr index_type call(std::index_sequence<P...>, Indices... i) const noexcept {
#if PREVIEW_CXX_VERSION < 17
    index_type result = 0;
    using parray = detail::static_array<std::size_t, P...>;
    index_type iarray[sizeof...(Indices)] = {static_cast<index_type>(i)...};
    for (std::size_t j = 0; j < parray::size; ++j) {
      result += iarray[j] * stride(parray::data[j]);
    }
    return result;
#else
    return ((static_cast<index_type>(i) * stride(P)) + ... + 0);
#endif
  }

  extents_type extents_{};

  // [mdspan.sub.map], submdspan mapping specialization
//  template<typename... SliceSpecifiers>
//  constexpr auto submdspan_mapping_impl(SliceSpecifiers...) const
//  -> see below;
//
//  template<typename... SliceSpecifiers>
//  friend constexpr auto submdspan_mapping(
//      const mapping& src, SliceSpecifiers... slices) {
//    return src.submdspan_mapping_impl(slices...);
//  }
};

template<typename Extents>
class layout_stride::mapping {
 public:
  using extents_type = Extents;
  using index_type   = typename extents_type::index_type;
  using size_type    = typename extents_type::size_type;
  using rank_type    = typename extents_type::rank_type;
  using layout_type  = layout_stride;

 private:
  static constexpr rank_type rank_ = extents_type::rank();

  template<rank_type... d>
  constexpr explicit mapping(in_place_t, std::integer_sequence<rank_type, d...>)
      : extents_(extents_type())
      , strides_{layout_right::mapping<extents_type>().stride(d)...} {}

  template<typename SpanOrArray, rank_type... d>
  constexpr explicit mapping(in_place_t, const extents_type& e, SpanOrArray&& s, std::integer_sequence<rank_type, d...>)
      : extents_(e)
      , strides_{preview::as_const(s[d])...} {}

  template<typename StridedLayoutMapping, rank_type... d>
  constexpr explicit mapping(in_place_t, const StridedLayoutMapping& other, std::integer_sequence<rank_type, d...>)
      : extents_(other.extents())
      , strides_{} {}

 public:
  constexpr mapping() noexcept
      : mapping(in_place, std::make_integer_sequence<rank_type, rank_>{}) {}

  constexpr mapping(const mapping&) noexcept = default;

  template<typename OtherIndexType, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>
  >::value, int> = 0>
  constexpr mapping(const extents_type& e, span<OtherIndexType, rank_> s) noexcept
      : mapping(in_place, e, s, std::make_integer_sequence<rank_type, rank_>{}) {}

  template<typename OtherIndexType, std::enable_if_t<conjunction<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>
  >::value, int> = 0>
  constexpr mapping(const extents_type& e, const std::array<OtherIndexType, rank_>& s) noexcept
      : mapping(in_place, e, s, std::make_integer_sequence<rank_type, rank_>{}) {}

  template<typename StridedLayoutMapping, std::enable_if_t<conjunction<
      detail::layout_mapping_alike<StridedLayoutMapping>,
      std::is_constructible<extents_type, typename StridedLayoutMapping::extents_type>,
      // explicit(true)
      negation<
          conjunction<
              std::is_convertible<typename StridedLayoutMapping::extents_type, extents_type>,
              disjunction<
                  detail::is_mapping_of<layout_left, StridedLayoutMapping>,
                  detail::is_mapping_of<layout_right, StridedLayoutMapping>,
                  detail::is_layout_left_padded_mapping_of<StridedLayoutMapping>,
                  detail::is_layout_right_padded_mapping_of<StridedLayoutMapping>,
                  detail::is_mapping_of<layout_stride, StridedLayoutMapping>
              >
          >
      >
  >::value, int> = 0>
  constexpr explicit mapping(const StridedLayoutMapping& other) noexcept
      : mapping(in_place, other, std::make_integer_sequence<rank_type, rank_>{}) {}

  constexpr mapping& operator=(const mapping&) noexcept = default;

  constexpr const extents_type& extents() const noexcept { return extents_; }
  constexpr std::array<index_type, rank_> strides() const noexcept { return strides_; }

  constexpr index_type required_span_size() const noexcept;

  template<typename... Indices>
  constexpr index_type operator()(Indices...) const noexcept;

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return false; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  constexpr bool is_exhaustive() const noexcept;
  static constexpr bool is_strided() noexcept { return true; }

  constexpr index_type stride(rank_type i) const noexcept { return strides_[i]; }

  template<class OtherMapping>
  friend constexpr bool operator==(const mapping&, const OtherMapping&) noexcept;

 private:
  extents_type extents_{};
  std::array<index_type, rank_> strides_{};

//  template<class... SliceSpecifiers>
//  constexpr auto submdspan-mapping-impl(SliceSpecifiers...) const
//  -> see below;
//
//  template<class... SliceSpecifiers>
//  friend constexpr auto submdspan_mapping(
//      const mapping& src, SliceSpecifiers... slices) {
//    return src.submdspan-mapping-impl(slices...);
//  }
};

template<std::size_t PaddingValue = dynamic_extent>
struct layout_left_padded {
  template<typename Extents>
  class mapping {
    using _padding_outer = layout_left_padded;

  };
};

template<std::size_t PaddingValue = dynamic_extent>
struct layout_right_padded {
  template<typename Extents>
  class mapping {
    using _padding_outer = layout_right_padded;

  };
};

// ---------- out-of-line definitions ----------

//      ----- layout_left::mapping -----
template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ negation<std::is_convertible<OtherExtents, Extents>>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_right::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ std::is_convertible<OtherExtents, Extents>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_right::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ bool_constant<(Extents::rank() > 0)>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other)
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ bool_constant<(Extents::rank() <= 0)>
>::value, int>>
constexpr layout_left::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other)
    : extents_(other.extents()) {}


//      ----- layout_right::mapping -----
template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ negation<std::is_convertible<OtherExtents, Extents>>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_left::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    bool_constant<(Extents::rank() <= 1)>,
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ std::is_convertible<OtherExtents, Extents>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_left::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(true) */ bool_constant<(Extents::rank() > 0)>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

template<typename Extents>
template<typename OtherExtents, std::enable_if_t<conjunction<
    std::is_constructible<Extents, OtherExtents>,
    /* explicit(false) */ bool_constant<(Extents::rank() <= 0)>
>::value, int>>
constexpr layout_right::mapping<Extents>::mapping(const layout_stride::mapping<OtherExtents>& other) noexcept
    : extents_(other.extents()) {}

namespace detail {

template<typename T>
struct is_layout_left_padded : std::false_type {};
template<std::size_t S>
struct is_layout_left_padded<layout_left_padded<S>> : std::true_type {};
template<typename T>
struct is_layout_right_padded : std::false_type {};
template<std::size_t S>
struct is_layout_right_padded<layout_right_padded<S>> : std::true_type {};

template<typename Mapping>
struct is_layout_left_padded_mapping_of<Mapping, void_t<typename Mapping::_padding_outer>>
    : is_layout_left_padded<typename Mapping::_padding_outer> {};

template<typename Mapping>
struct is_layout_right_padded_mapping_of<Mapping, void_t<typename Mapping::_padding_outer>>
    : is_layout_right_padded<typename Mapping::_padding_outer> {};

} // namespace detail
} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_H_
