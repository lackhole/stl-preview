//
// Created by yonggyulee on 2024. 9. 27.
//

#ifndef PREVIEW_MDSPAN_MDSPAN_H_
#define PREVIEW_MDSPAN_MDSPAN_H_

#include <array>
#include <type_traits>
#include <utility>

#include "preview/__core/deprecated.h"
#include "preview/__mdspan/default_accessor.h"
#include "preview/__mdspan/extents.h"
#include "preview/span.h"
#include "preview/__utility/as_const.h"
#include "preview/__utility/compressed_pair.h"

namespace preview {
namespace detail {

template<typename AccessorType, typename MappingType, typename DataHandleType>
struct mdspan_storage_base : compressed_pair<AccessorType, MappingType> {
  using pair = compressed_pair<AccessorType, MappingType>;

  constexpr mdspan_storage_base()
#if defined(__clang__) && __clang_major__ <= 8 && __clang_minor__ < 1

#else
      noexcept(std::is_nothrow_constructible<pair>::value &&
               std::is_nothrow_constructible<DataHandleType>::value)
#endif
      = default;

  template<typename DataHandleArg, typename MappingArg>
  constexpr mdspan_storage_base(DataHandleArg&& ptr, MappingArg&& marg)
      : pair{AccessorType{}, MappingType{std::forward<MappingArg>(marg)}}
      , ptr_{std::forward<DataHandleArg>(ptr)} {}

  template<typename DataHandleArg, typename MappingArg>
  constexpr mdspan_storage_base(DataHandleArg&& ptr, MappingArg&& marg, const AccessorType& a)
      : pair{a, MappingType{std::forward<MappingArg>(marg)}}
      , ptr_{std::forward<DataHandleArg>(ptr)} {}

  template<typename DataHandleArg, typename MappingArg, typename OtherAccessorType>
  constexpr mdspan_storage_base(DataHandleArg&& ptr, MappingArg&& marg, const OtherAccessorType& a)
      : pair{a, MappingType{std::forward<MappingArg>(marg)}}
      , ptr_{std::forward<DataHandleArg>(ptr)} {}

  constexpr       AccessorType& accessor()       noexcept { return pair::first(); }
  constexpr const AccessorType& accessor() const noexcept { return pair::first(); }
  constexpr       MappingType&  mapping ()       noexcept { return pair::second(); }
  constexpr const MappingType&  mapping () const noexcept { return pair::second(); }

  DataHandleType ptr_{};
};

template<bool DefaultConstructible /* true */, typename AccessorType, typename MappingType, typename DataHandleType>
struct mdspan_storage : mdspan_storage_base<AccessorType, MappingType, DataHandleType> {
  using base = mdspan_storage_base<AccessorType, MappingType, DataHandleType>;
  using base::base;
  using base::accessor;
  using base::mapping;
  using base::ptr_;

  constexpr mdspan_storage() noexcept(std::is_nothrow_constructible<base>::value) = default;
};

template<typename AccessorType, typename MappingType, typename DataHandleType>
struct mdspan_storage<false, AccessorType, MappingType, DataHandleType>
    : mdspan_storage_base<AccessorType, MappingType, DataHandleType>
{
  using base = mdspan_storage_base<AccessorType, MappingType, DataHandleType>;
  using base::base;
  using base::accessor;
  using base::mapping;
  using base::ptr_;

  constexpr mdspan_storage() = delete;
};

} // namespace detail

template<
    typename ElementType,
    typename Extents,
    typename LayoutPolicy = layout_right,
    typename AccessorPolicy = default_accessor<ElementType>>
class mdspan {
 public:
  static_assert(is_complete<ElementType>::value, "Invalid type");
  static_assert(!std::is_abstract<ElementType>::value, "Invalid type");
  static_assert(!std::is_array<ElementType>::value, "Invalid type");
  static_assert(detail::is_extents<Extents>::value, "Invalid type");
  static_assert(std::is_same<ElementType, typename AccessorPolicy::element_type>::value, "Invalid type");

  using extents_type = Extents;
  using layout_type = LayoutPolicy;
  using accessor_type = AccessorPolicy;
  using mapping_type = typename layout_type::template mapping<extents_type>;
  using element_type = ElementType;
  using value_type = std::remove_cv_t<element_type>;
  using index_type = typename extents_type::index_type;
  using size_type = typename extents_type::size_type;
  using rank_type = typename extents_type::rank_type;
  using data_handle_type = typename accessor_type::data_handle_type;
  using reference = typename accessor_type::reference;

  static constexpr rank_type rank() noexcept { return extents_type::rank(); }
  static constexpr rank_type rank_dynamic() noexcept { return extents_type::rank_dynamic(); }
  static constexpr size_t static_extent(rank_type r) noexcept { return extents_type::static_extent(r); }
  constexpr index_type extent(rank_type r) const noexcept { return extents().extent(r); }

//  template<bool B = conjunction_v<
//      bool_constant<(rank_dynamic() > 0)>,
//      std::is_default_constructible<data_handle_type>,
//      std::is_default_constructible<mapping_type>,
//      std::is_default_constructible<accessor_type>
//  >, std::enable_if_t<B, int> = 0>
  constexpr mdspan() = default;

  constexpr mdspan(const mdspan& rhs) = default;
  constexpr mdspan(mdspan&& rhs) = default;

  template<class... OtherIndexTypes, std::enable_if_t<conjunction_v<
      std::is_convertible<OtherIndexTypes, index_type>...,
      std::is_nothrow_constructible<index_type, OtherIndexTypes>...,
      bool_constant<((sizeof...(OtherIndexTypes) == rank()) || (sizeof...(OtherIndexTypes) == rank_dynamic()))>,
      std::is_constructible<mapping_type, extents_type>,
      std::is_default_constructible<accessor_type>
  >, int> = 0>
  constexpr explicit mdspan(data_handle_type ptr, OtherIndexTypes... exts)
      : storage_(std::move(ptr), extents_type(static_cast<index_type>(std::move(exts))...)) {}

  template<class OtherIndexType, std::size_t N, std::enable_if_t<conjunction_v<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank() || N == rank_dynamic())>,
      std::is_constructible<mapping_type, extents_type>,
      std::is_default_constructible<accessor_type>,
      // explicit (true)
      bool_constant<(N != rank_dynamic())>
  >, int> = 0>
  constexpr explicit mdspan(data_handle_type p, span<OtherIndexType, N> exts)
      : storage_(std::move(p), extents_type(exts)) {}

  template<class OtherIndexType, std::size_t N, std::enable_if_t<conjunction_v<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank() || N == rank_dynamic())>,
      std::is_constructible<mapping_type, extents_type>,
      std::is_default_constructible<accessor_type>,
      // explicit (false)
      bool_constant<(N == rank_dynamic())>
  >, int> = 0>
  constexpr mdspan(data_handle_type p, span<OtherIndexType, N> exts)
      : storage_(std::move(p), extents_type(exts)) {}

  template<class OtherIndexType, std::size_t N, std::enable_if_t<conjunction_v<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank() || N == rank_dynamic())>,
      std::is_constructible<mapping_type, extents_type>,
      std::is_default_constructible<accessor_type>,
      // explicit (true)
      bool_constant<(N != rank_dynamic())>
  >, int> = 0>
  constexpr explicit mdspan(data_handle_type p, const std::array<OtherIndexType, N>& exts)
      : storage_(std::move(p), extents_type(exts)) {}

  template<class OtherIndexType, std::size_t N, std::enable_if_t<conjunction_v<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>,
      bool_constant<(N == rank() || N == rank_dynamic())>,
      std::is_constructible<mapping_type, extents_type>,
      std::is_default_constructible<accessor_type>,
      // explicit (false)
      bool_constant<(N == rank_dynamic())>
  >, int> = 0>
  constexpr mdspan(data_handle_type p, const std::array<OtherIndexType, N>& exts)
      : storage_(std::move(p), extents_type(exts)) {}

  template<bool B = conjunction_v<
      std::is_constructible<mapping_type, const extents_type&>,
      std::is_default_constructible<accessor_type>
  >, std::enable_if_t<B, int> = 0>
  constexpr mdspan(data_handle_type p, const extents_type& ext)
      : storage_(std::move(p), ext) {}

  template<bool B = std::is_default_constructible<accessor_type>::value, std::enable_if_t<B, int> = 0>
  constexpr mdspan(data_handle_type p, const mapping_type& m)
      : storage_(std::move(p), m) {}

  constexpr mdspan(data_handle_type p, const mapping_type& m, const accessor_type& a)
      : storage_(std::move(p), m, a) {}

  template<typename OtherElementType, typename OtherExtents, typename OtherLayoutPolicy, typename OtherAccessorPolicy,
      std::enable_if_t<conjunction_v<
          std::is_constructible<mapping_type, const typename OtherLayoutPolicy::template mapping<OtherExtents>&>,
          std::is_constructible<accessor_type, const OtherAccessorPolicy&>,
          // explicit(true)
          bool_constant<(
              !std::is_convertible<const typename OtherLayoutPolicy::template mapping<OtherExtents>&, mapping_type>::value
              || !std::is_convertible<const OtherAccessorPolicy&, accessor_type>::value)>
  >, int> = 0>
  constexpr explicit mdspan(const mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessorPolicy>& other)
      : storage_(other.data_handle(), other.mapping(), other.accessor()) {}

  template<typename OtherElementType, typename OtherExtents, typename OtherLayoutPolicy, typename OtherAccessorPolicy,
      std::enable_if_t<conjunction_v<
          std::is_constructible<mapping_type, const typename OtherLayoutPolicy::template mapping<OtherExtents>&>,
          std::is_constructible<accessor_type, const OtherAccessorPolicy&>,
          // explicit(false)
          bool_constant<!(
              !std::is_convertible<const typename OtherLayoutPolicy::template mapping<OtherExtents>&, mapping_type>::value
              || !std::is_convertible<const OtherAccessorPolicy&, accessor_type>::value)>
  >, int> = 0>
  constexpr mdspan(const mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessorPolicy>& other)
      : storage_(other.data_handle(), other.mapping(), other.accessor()) {}

  constexpr mdspan& operator=(const mdspan& rhs) = default;
  constexpr mdspan& operator=(mdspan&& rhs) = default;

  template<typename... OtherIndexTypes, std::enable_if_t<conjunction_v<
      std::is_convertible<OtherIndexTypes, index_type>...,
      std::is_nothrow_constructible<index_type, OtherIndexTypes>...,
      bool_constant<(sizeof...(OtherIndexTypes) == rank())>
  >, int> = 0>
  PREVIEW_DEPRECATED_SINCE_CXX23("Use subscript operator instead")
  constexpr reference at(OtherIndexTypes... indices) const {
    return accessor().access(storage_.ptr_, storage_.mapping()(static_cast<index_type>(std::move(indices))...));
  }

  template<typename... OtherIndexTypes, std::enable_if_t<conjunction_v<
      std::is_convertible<OtherIndexTypes, index_type>...,
      std::is_nothrow_constructible<index_type, OtherIndexTypes>...,
      bool_constant<(sizeof...(OtherIndexTypes) == rank())>
  >, int> = 0>
  constexpr reference operator[](OtherIndexTypes... indices) const {
    return accessor().access(storage_.ptr_, storage_.mapping()(static_cast<index_type>(std::move(indices))...));
  }

  template<typename OtherIndexType, std::enable_if_t<conjunction_v<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>
  >, int> = 0>
  constexpr reference operator[](span<OtherIndexType, rank()> indices) const {
    return at_span_array(indices, std::make_index_sequence<rank()>{});
  }

  template<typename OtherIndexType, std::enable_if_t<conjunction_v<
      std::is_convertible<const OtherIndexType&, index_type>,
      std::is_nothrow_constructible<index_type, const OtherIndexType&>
  >, int> = 0>
  constexpr reference operator[](const std::array<OtherIndexType, rank()>& indices) const {
    return at_span_array(indices, std::make_index_sequence<rank()>{});
  }

  constexpr size_type size() const noexcept {
    return static_cast<size_type>(detail::extents_helper::fwd_prod_of_extents(extents(), rank()));
  }
  constexpr bool empty() const noexcept {
    return size() == 0;
  }

  friend constexpr void swap(mdspan& x, mdspan& y) noexcept {
    using std::swap;
    swap(x.storage_.ptr_, y.storage_.ptr_);
    swap(x.storage_.mapping(), y.storage_.mapping());
    swap(x.storage_.accessor(), y.storage_.accessor());
  }

  constexpr const extents_type& extents() const noexcept { return mapping().extents(); }
  constexpr const data_handle_type& data_handle() const noexcept { return storage_.ptr_; }
  constexpr const mapping_type& mapping() const noexcept { return storage_.mapping(); }
  constexpr const accessor_type& accessor() const noexcept { return storage_.accessor(); }

  // TODO:Verify LWG-4021
  static constexpr bool is_always_unique() noexcept { return mapping_type::is_always_unique(); }
  static constexpr bool is_always_exhaustive() noexcept { return mapping_type::is_always_exhaustive(); }
  static constexpr bool is_always_strided() noexcept { return mapping_type::is_always_strided(); }

  constexpr bool is_unique() const { return mapping().is_unique(); }
  constexpr bool is_exhaustive() const { return mapping().is_exhaustive(); }
  constexpr bool is_strided() const { return mapping().is_strided(); }
  constexpr index_type stride(rank_type r) const { return mapping().stride(r); }

 private:
  template<typename... OtherIndexTypes>
  constexpr reference at_impl(OtherIndexTypes... indices) const {
    return accessor().access(storage_.ptr_, storage_.mapping()(static_cast<index_type>(std::move(indices))...));
  }

  template<typename SpanOrArray, std::size_t... P>
  constexpr reference at_span_array(SpanOrArray&& indices, std::index_sequence<P...>) const {
    return at_impl(detail::extents_helper::index_cast<extents_type>(preview::as_const(indices[P]))...);
  }

  using default_constructible = conjunction<
      bool_constant<(rank_dynamic() > 0)>,
      std::is_default_constructible<data_handle_type>,
      std::is_default_constructible<mapping_type>,
      std::is_default_constructible<accessor_type>
  >;

  detail::mdspan_storage<default_constructible::value, accessor_type, mapping_type, data_handle_type> storage_;
};

#if PREVIEW_CXX_VERSION >= 17
#if !PREVIEW_CONFORM_CXX20_STANDARD
template<typename CArray>
mdspan(CArray&)
    -> mdspan<
        std::enable_if_t<(std::is_array<CArray>::value && std::rank<CArray>::value == 1),
                         std::remove_all_extents_t<CArray>>,
        extents<std::size_t, std::extent<CArray, 0>::value>>;

template<typename Pointer>
mdspan(Pointer&&)
    -> mdspan<
        std::enable_if_t<std::is_pointer<std::remove_reference_t<Pointer>>::value,
                         std::remove_pointer_t<std::remove_reference_t<Pointer>>>,
        extents<std::size_t>>;

template<typename ElementType, typename... Integrals>
explicit mdspan(ElementType*, Integrals...)
    -> mdspan<
        std::enable_if_t<conjunction_v<
                             std::is_convertible<Integrals, std::size_t>...,
                             bool_constant<(sizeof...(Integrals) > 0)>>,
                         ElementType>,
        extents<std::size_t, detail::maybe_static_ext<Integrals>::value...>>;
#else
template<typename CArray>
requires (std::is_array_v<CArray> && std::rank_v<CArray> == 1)
mdspan(CArray&)
    -> mdspan<std::remove_all_extents_t<CArray>, extents<std::size_t, std::extent_v<CArray, 0>>>;

template<typename Pointer>
requires (std::is_pointer_v<std::remove_reference_t<Pointer>>)
mdspan(Pointer&&)
    -> mdspan<std::remove_pointer_t<std::remove_reference_t<Pointer>>, extents<std::size_t>>;

template<typename ElementType, typename... Integrals>
requires ((std::is_convertible_v<Integrals, std::size_t> && ...) && sizeof...(Integrals) > 0)
explicit mdspan(ElementType*, Integrals...)
    -> mdspan<ElementType, extents<std::size_t, detail::maybe_static_ext<Integrals>::value...>>;
#endif

template<class ElementType, class OtherIndexType, size_t N>
mdspan(ElementType*, span<OtherIndexType, N>)
    -> mdspan<ElementType, dextents<size_t, N>>;

template<class ElementType, class OtherIndexType, size_t N>
mdspan(ElementType*, const std::array<OtherIndexType, N>&)
    -> mdspan<ElementType, dextents<size_t, N>>;

template<class ElementType, class IndexType, size_t... ExtentsPack>
mdspan(ElementType*, const extents<IndexType, ExtentsPack...>&)
    -> mdspan<ElementType, extents<IndexType, ExtentsPack...>>;

template<class ElementType, class MappingType>
mdspan(ElementType*, const MappingType&)
    -> mdspan<
        ElementType,
        typename MappingType::extents_type,
        typename MappingType::layout_type>;

template<class MappingType, class AccessorType>
mdspan(const typename AccessorType::data_handle_type&, const MappingType&, const AccessorType&)
    -> mdspan<
        typename AccessorType::element_type,
        typename MappingType::extents_type,
        typename MappingType::layout_type, AccessorType>;

#endif

} // namespace preview

#endif // PREVIEW_MDSPAN_MDSPAN_H_
