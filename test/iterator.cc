#include "preview/iterator.h"
#include "gtest.h"


template<class T>
struct Holder {
  T t;
};

struct Incomplete;
using P = Holder<Incomplete>*;

TEST(VERSIONED(Iterator), projected) {
  EXPECT_TRUE_TYPE(preview::equality_comparable<P>); // OK
  EXPECT_TRUE_TYPE(preview::indirectly_comparable<P*, P*, std::equal_to<>>); // Error before C++26
  EXPECT_TRUE_TYPE(preview::sortable<P*>); // Error before C++26
}
