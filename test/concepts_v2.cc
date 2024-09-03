#include <list>
#include <forward_list>

#include "preview/concepts_v2.h"

#include "gtest.h"


template<typename T, typename U, preview_require(preview::integral<T>)>
void foo(T, U) {}

// TODO: Write test
TEST(VERSIONED(ConceptsV2), basics) {
  using namespace preview;
  using concepts::constraints_not_satisfied;
  using concepts::at;
  using concepts::and_;
  using concepts::because;
  using concepts::is_false;


  auto a = preview::integral<float>; //  && std::is_signed<unsigned>{};
  auto b = preview::resolve_require(a);

  auto x = preview::signed_integral<unsigned>;
  auto y = preview::resolve_require(x);

  static_assert(std::is_same<
      decltype(y),
      constraints_not_satisfied<
          concepts::signed_integral<unsigned>, at<0, 1>, because<constraints_not_satisfied<
              std::is_signed<unsigned>, at<1, 2>
          >>>
  >::value, "");

  static_assert(std::is_same<
      decltype(preview::resolve_require(preview::integral<float>)),
      constraints_not_satisfied<concepts::integral<float>, at<0, 1>, because<std::is_integral<float>, is_false>>
  >::value, "");

//  foo(1.0, 2);
}
