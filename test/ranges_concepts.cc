#include "preview/ranges.h"
#include "gtest.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <list>
#include <vector>

#include "preview/algorithm.h"
#include "preview/concepts.h"
#include "preview/span.h"
#include "preview/string_view.h"


TEST(VERSIONED(RangesConcepts), range) {
  // A minimum range
  struct SimpleRange
  {
    int* begin();
    int* end();
  };
  static_assert(preview::ranges::range<SimpleRange>::value, "");

  // not a range: no begin/end
  struct NotRange
  {
    int t {};
  };
  static_assert(!preview::ranges::range<NotRange>::value, "");

  // not a range: begin does not return an input_or_output_iterator
  struct NotRange2
  {
    void* begin();
    int* end();
  };
  static_assert(!preview::ranges::range<NotRange2>::value, "");
}
