//
// Created by YongGyu Lee on 4/9/24.
//

#ifndef PREVIEW_RANGES_VIEWS_ZIP_TRANSFORM_VIEW_H_
#define PREVIEW_RANGES_VIEWS_ZIP_TRANSFORM_VIEW_H_

#include <tuple>
#include <type_traits>
#include <utility>

#include "preview/__compare/three_way_comparable.h"
#include "preview/__concepts/derived_from.h"
#include "preview/__concepts/equality_comparable.h"
#include "preview/__concepts/invocable.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__concepts/move_constructible.h"
#include "preview/__core/constexpr.h"
#include "preview/__core/std_version.h"
#include "preview/__iterator/detail/have_cxx20_iterator.h"
#include "preview/__iterator/iterator_tag.h"
#include "preview/__iterator/iterator_traits.h"
#include "preview/__iterator/sized_sentinel_for.h"
#include "preview/__memory/addressof.h"
#include "preview/__ranges/bidirectional_range.h"
#include "preview/__ranges/forward_range.h"
#include "preview/__ranges/input_range.h"
#include "preview/__ranges/iterator_t.h"
#include "preview/__ranges/movable_box.h"
#include "preview/__ranges/random_access_range.h"
#include "preview/__ranges/range.h"
#include "preview/__ranges/range_difference_t.h"
#include "preview/__ranges/range_reference_t.h"
#include "preview/__ranges/sentinel_t.h"
#include "preview/__ranges/view.h"
#include "preview/__ranges/view_interface.h"
#include "preview/__ranges/views/all.h"
#include "preview/__ranges/views/zip_view.h"
#include "preview/__type_traits/conditional.h"
#include "preview/__type_traits/conjunction.h"
#include "preview/__type_traits/is_invocable.h"
#include "preview/__type_traits/is_referenceable.h"
#include "preview/__type_traits/maybe_const.h"
#include "preview/__type_traits/negation.h"
#include "preview/__utility/cxx20_rel_ops.h"
#include "preview/__utility/type_sequence.h"

namespace preview {
namespace ranges {
namespace detail {

template<bool Const, typename Base, typename F, typename ViewPack, bool = forward_range<Base>::value /* false */>
struct zip_transform_view_iterator_category {
#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
  using iterator_category = iterator_ignore;
#endif
};

template<bool Const, typename Base, typename F, typename... Views>
struct zip_transform_view_iterator_category<Const, Base, F, type_sequence<Views...>, true> {
 private:
  using MF = maybe_const<Const, F>;
  template<typename View>
  using IC = typename iterator_traits<iterator_t<maybe_const<Const, View> >>::iterator_category;

  template<typename BaseC>
  using POT_derived_from = conjunction<derived_from<Views, BaseC>...>;

 public:
  using iterator_category =
      conditional_t<
          negation<is_referencable< invoke_result_t<MF&, range_reference_t<maybe_const<Const, Views>>...> >>, input_iterator_tag,
          POT_derived_from<random_access_iterator_tag>, random_access_iterator_tag,
          POT_derived_from<bidirectional_iterator_tag>, bidirectional_iterator_tag,
          POT_derived_from<forward_iterator_tag>, forward_iterator_tag,
          input_iterator_tag
      >;
};

} // namespace detail

template<typename F, typename... Views>
class zip_transform_view : public view_interface<zip_transform_view<Views...>> {
 public:
  static_assert(move_constructible<F>::value, "Constraints not satisfied");
  static_assert(conjunction<input_range<Views>...>::value, "Constraints not satisfied");
  static_assert(conjunction<view<Views>...>::value, "Constraints not satisfied");
  static_assert(sizeof...(Views) > 0, "Constraints not satisfied");
  static_assert(std::is_object<F>::value, "Constraints not satisfied");
  static_assert(regular_invocable<F&, range_reference_t<Views>...>::value, "Constraints not satisfied");
  static_assert(is_referencable<invoke_result_t<F&, range_reference_t<Views>...>>::value, "Constraints not satisfied");

  using tuple_index_sequence = std::index_sequence_for<Views...>;

  template<bool Const> class sentinel;
  template<bool Const> class iterator;

 private:
  using InnerView = zip_view<Views...>;
  template<bool Const> using ziperator = iterator_t<maybe_const<Const, InnerView>>;
  template<bool Const> using zentinel = sentinel_t<maybe_const<Const, InnerView>>;


 public:
  template<bool Const>
  class iterator
     : public detail::zip_transform_view_iterator_category<
        Const,
        InnerView,
        F,
        type_sequence<Views...>>
  {
    using Parent = maybe_const<Const, zip_transform_view>;
    using Base = maybe_const<Const, InnerView>;

    friend class iterator<false>;
    friend class sentinel<true>;
    friend class sentinel<false>;
    friend class zip_transform_view;

    struct deref_fn {
     private:
      struct impl {
        template<typename Tuple, std::size_t... I>
        decltype(auto) operator()(const F& f, const Tuple& t, std::index_sequence<I...>)
            noexcept(noexcept(preview::invoke(f, std::get<I>(t)...)))
        {
          return preview::invoke(f, std::get<I>(t)...);
        }
      };

     public:
      template<typename Tuple>
      decltype(auto) operator()(const F& f, const Tuple& t)
          noexcept(noexcept(preview::invoke(impl{}, f, t, tuple_index_sequence{})))
      {
        return impl{}(f, t, tuple_index_sequence{});
      }

    };

   public:

    using iterator_concept = typename ziperator<Const>::iterator_concept;
    using value_type = std::conditional_t<Const,
        remove_cvref_t<invoke_result_t<const F&, range_reference_t<const Views>...>>,
        remove_cvref_t<invoke_result_t<F&, range_reference_t<Views>...>>>;
    using difference_type = range_difference_t<Base>;

#if !PREVIEW_STD_HAVE_CXX20_ITERATOR
    using pointer = void;
    using reference = invoke_result_t<deref_fn, const F&, decltype(*std::declval<ziperator<Const>&>())>;
#endif

    iterator() = default;

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<ziperator<false>, ziperator<Const>>
    >::value, int> = 0>
    constexpr iterator(iterator<!Const> i)
        : parent_(std::move(i.parent_)), inner_(std::move(i.inner_)) {}

    constexpr decltype(auto) operator*() const
        noexcept(noexcept(preview::invoke(deref_fn{},
                                          *std::declval<const iterator&>().parent_->fun_,
                                          *std::declval<const iterator&>().inner_)))
    {
      return deref_fn{}(*parent_->fun_, *inner_);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr decltype(auto) operator[](difference_type n) const {
      return preview::apply(
          [&](const auto&... iters) -> decltype(auto) {
            return preview::invoke(*parent_->fun_, iters[iter_difference_t<remove_cvref_t<decltype(iters)>>(n)]...);
          }
      );
    }

    constexpr iterator& operator++() {
      ++inner_;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<!forward_range<B>::value, int> = 0>
    constexpr void operator++(int) {
      ++*this;
    }

    template<typename B = Base, std::enable_if_t<forward_range<B>::value, int> = 0>
    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    template<typename B = Base, std::enable_if_t<bidirectional_range<B>::value, int> = 0>
    constexpr iterator& operator--() {
      --inner_;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<bidirectional_range<B>::value, int> = 0>
    constexpr iterator operator--(int) {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr iterator& operator+=(difference_type n) {
      inner_ += n;
      return *this;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    constexpr iterator& operator-=(difference_type n) {
      inner_ -= n;
      return *this;
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<equality_comparable<Z>::value, int> = 0>
    friend constexpr bool operator==(const iterator& x, const iterator& y) {
      return x.inner_ == y.inner_;
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<equality_comparable<Z>::value, int> = 0>
    friend constexpr bool operator!=(const iterator& x, const iterator& y) {
      return !(x.inner_ == y.inner_);
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<conjunction<
        random_access_range<Z>,
        unstable_three_way_comparable<Z>
    >::value, int> = 0>
    friend constexpr bool operator<(const iterator& x, const iterator& y) {
      return x.inner_ < y.inner_;
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<conjunction<
        random_access_range<Z>,
        unstable_three_way_comparable<Z>
    >::value, int> = 0>
    friend constexpr bool operator<=(const iterator& x, const iterator& y) {
      using namespace preview::rel_ops;
      return x <= y;
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<conjunction<
        random_access_range<Z>,
        unstable_three_way_comparable<Z>
    >::value, int> = 0>
    friend constexpr bool operator>(const iterator& x, const iterator& y) {
      using namespace preview::rel_ops;
      return x > y;
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<conjunction<
        random_access_range<Z>,
        unstable_three_way_comparable<Z>
    >::value, int> = 0>
    friend constexpr bool operator>=(const iterator& x, const iterator& y) {
      using namespace preview::rel_ops;
      return x >= y;
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator+(const iterator& i, difference_type n) {
      return iterator(*i.parent_, i.inner_ + n);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator+(difference_type n, const iterator& i) {
      return iterator(*i.parent_, i.inner_ + n);
    }

    template<typename B = Base, std::enable_if_t<random_access_range<B>::value, int> = 0>
    friend constexpr iterator operator-(const iterator& i, difference_type n) {
      return iterator(*i.parent_, i.inner_ - n);
    }

    template<typename Z = ziperator<Const>, std::enable_if_t<sized_sentinel_for<Z, Z>::value, int> = 0>
    friend constexpr difference_type operator-(const iterator& i, const iterator& j) {
      return i.inner_ - j.inner_;
    }

   private:
    PREVIEW_CONSTEXPR_AFTER_CXX17 iterator(Parent& parent, ziperator<Const> inner)
        : parent_(preview::addressof(parent)), inner_(std::move(inner)) {}

    Parent* parent_{};
    ziperator<Const> inner_{};
  };

  template<bool Const>
  class sentinel {
    friend class sentinel<false>;
    friend class zip_transform_view;

   public:
    sentinel() = default;

    template<bool AntiConst, std::enable_if_t<conjunction<
        bool_constant<((Const != AntiConst) && Const)>,
        convertible_to<zentinel<false>, zentinel<Const>>
    >::value, int> = 0>
    constexpr sentinel(sentinel<AntiConst> i)
        : inner_(std::move(i.inner_)) {}

    template<bool OtherConst, std::enable_if_t<sentinel_for<zentinel<Const>, ziperator<OtherConst>>::value, int> = 0>
    friend constexpr bool operator==(const iterator<OtherConst>& x, const sentinel& y) {
      return y.equal_to(x);
    }

    template<bool OtherConst, std::enable_if_t<sentinel_for<zentinel<Const>, ziperator<OtherConst>>::value, int> = 0>
    friend constexpr bool operator==(const sentinel& y, const iterator<OtherConst>& x) {
      return x == y;
    }

    template<bool OtherConst, std::enable_if_t<sentinel_for<zentinel<Const>, ziperator<OtherConst>>::value, int> = 0>
    friend constexpr bool operator!=(const iterator<OtherConst>& x, const sentinel& y) {
      return !(x == y);
    }

    template<bool OtherConst, std::enable_if_t<sentinel_for<zentinel<Const>, ziperator<OtherConst>>::value, int> = 0>
    friend constexpr bool operator!=(const sentinel& y, const iterator<OtherConst>& x) {
      return !(x == y);
    }

    template<bool OtherConst, std::enable_if_t<sized_sentinel_for<zentinel<Const>, ziperator<OtherConst>>::value, int> = 0>
    friend constexpr range_difference_t<maybe_const<OtherConst, InnerView>>
    operator-(const iterator<OtherConst>& x, const sentinel& y) {
      return x.inner_ - y.inner_;
    }

    template<bool OtherConst, std::enable_if_t<sized_sentinel_for<zentinel<Const>, ziperator<OtherConst>>::value, int> = 0>
    friend constexpr range_difference_t<maybe_const<OtherConst, InnerView>>
    operator-(const sentinel& y, const iterator<OtherConst>& x) {
      return y.inner_ - x.inner_;
    }

   private:
    constexpr explicit sentinel(zentinel<Const> inner)
        : inner_(std::move(inner)) {}

    template<bool OtherConst>
    constexpr bool equal_to(const iterator<OtherConst>& x) const {
      return x.inner_ == inner_;
    }

    zentinel<Const> inner_{};
  };

  zip_transform_view() = default;

  constexpr zip_transform_view(F fun, Views... views)
      : fun_(std::move(fun)), zip_(std::move(views)...) {}

  constexpr iterator<false> begin() {
    return iterator<false>(*this, zip_.begin());
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      range<const zip_view<Views...>>
  >::value, int> = 0>
  constexpr iterator<true> begin() const {
    return iterator<true>(*this, zip_.begin());
  }

  constexpr auto end() {
    return end_impl<false>(*this, common_range<InnerView>{});
  }

  constexpr auto end() const {
    return end_impl<true>(*this, common_range<const InnerView>{});
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<InnerView>
  >::value, int> = 0>
  constexpr auto size() {
    return zip_.size();
  }

  template<typename Dummy = void, std::enable_if_t<conjunction<std::is_void<Dummy>,
      sized_range<const InnerView>
  >::value, int> = 0>
  constexpr auto size() const {
    return zip_.size();
  }

 private:
  template<bool Const, typename Self>
  static constexpr iterator<Const> end_impl(Self&& self, std::true_type /* common_range */) {
    return iterator<Const>(self, self.zip_.end());
  }

  template<bool Const, typename Self>
  static constexpr sentinel<Const> end_impl(Self&& self, std::false_type /* common_range */) {
    return sentinel<Const>(self.zip_.end());
  }

  movable_box<F> fun_{};
  InnerView zip_{};
};

#if __cplusplus >= 201703L

template<typename F, typename... Rs>
zip_transform_view(F, Rs&&...) -> zip_transform_view<F, views::all_t<Rs>...>;

#endif

} // namespace ranges
} // namespace preview

#endif // PREVIEW_RANGES_VIEWS_ZIP_TRANSFORM_VIEW_H_
