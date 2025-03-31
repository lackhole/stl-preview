//
// Created by yonggyulee on 2024. 10. 27.
//

#ifndef PREVIEW_MDSPAN_LAYOUT_RIGHT_H_
#define PREVIEW_MDSPAN_LAYOUT_RIGHT_H_

#include "preview/__array/to_array.h"
#include "preview/__mdspan/extents.h"
#include "preview/__mdspan/layout_forward_declare.h"

namespace preview {

template<typename Extents>
class layout_right::mapping {
  static_assert(detail::is_extents<Extents>::value, "Extents must be a specialization of extents");
  static_assert(detail::extents_helper::index_space_is_representable<Extents>(),
                "index space Extents() must be representable as a value of type typename Extents::index_type");
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
      /* explicit(false) */ bool_constant<!(extents_type::rank() > 0)>
  >::value, int> = 0>
  constexpr mapping(const layout_stride::mapping<OtherExtents>& other) noexcept;

  constexpr mapping& operator=(const mapping&) noexcept = default;

  // [mdspan.layout.right.obs], observers
  constexpr const extents_type& extents() const noexcept { return extents_; }

  constexpr index_type required_span_size() const noexcept {
    return detail::extents_helper::fwd_prod_of_extents(extents(), extents_type::rank());
  }

#if PREVIEW_CXX_VERSION < 17
  template<bool B = (extents_type::rank() == 0), std::enable_if_t<B, int> = 0>
  constexpr index_type operator()() const noexcept {
    return 0;
  }

  template<typename... Indices, std::enable_if_t<conjunction<
      bool_constant<(extents_type::rank() != 0)>,
      bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_constructible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >::value, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(preview::to_array({static_cast<index_type>(std::move(i))...}));
  }
#else
  template<typename... Indices, std::enable_if_t<conjunction<
      bool_constant<(sizeof...(Indices) == extents_type::rank())>,
      std::is_constructible<Indices, index_type>...,
      std::is_nothrow_constructible<index_type, Indices>...
  >::value, int> = 0>
  constexpr index_type operator()(Indices... i) const noexcept {
    return call(std::index_sequence_for<Indices...>{}, std::move(i)...);
  }
#endif

  static constexpr bool is_always_unique() noexcept { return true; }
  static constexpr bool is_always_exhaustive() noexcept { return true; }
  static constexpr bool is_always_strided() noexcept { return true; }

  static constexpr bool is_unique() noexcept { return true; }
  static constexpr bool is_exhaustive() noexcept { return true; }
  static constexpr bool is_strided() noexcept { return true; }

  template<typename Dummy = void, std::enable_if_t<std::is_void<Dummy>::value && (extents_type::rank() > 0), int> = 0>
  constexpr index_type stride(rank_type i) const noexcept {
    return detail::extents_helper::rev_prod_of_extents(extents(), i);
  }

  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator==(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return x.extents() == y.extents();
  }

#if !PREVIEW_CONFORM_CXX20_STANDARD
  template<typename OtherExtents, std::enable_if_t<(extents_type::rank() == OtherExtents::rank()), int> = 0>
  friend constexpr bool operator!=(const mapping& x, const mapping<OtherExtents>& y) noexcept {
    return !(x == y);
  }
#endif

 private:
#if PREVIEW_CXX_VERSION < 17
  template<std::size_t Rank>
  constexpr index_type call(const std::array<index_type, Rank>& indices) const noexcept {
    index_type result = 0;
    for (std::size_t r = 0; r < Rank; ++r) {
      result += indices[r] * stride(r);
    }
    return 0;
  }
  constexpr index_type call(const std::array<index_type, 1>& indices) const noexcept {
    return indices[0] * stride(0);
  }
  constexpr index_type call(const std::array<index_type, 2>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1);
  }
  constexpr index_type call(const std::array<index_type, 3>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1) + indices[2] * stride(2);
  }
  constexpr index_type call(const std::array<index_type, 4>& indices) const noexcept {
    return indices[0] * stride(0) + indices[1] * stride(1) + indices[2] * stride(2) + indices[3] * stride(3);
  }
#else
  template<std::size_t... P, typename... Indices>
  constexpr index_type call(std::index_sequence<P...>, Indices... i) const noexcept {
    return ((static_cast<index_type>(i) * stride(P)) + ... + 0);
  }
#endif

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

} // namespace preview

#endif // PREVIEW_MDSPAN_LAYOUT_RIGHT_H_
