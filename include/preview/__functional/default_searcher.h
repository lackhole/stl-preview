//
// Created by YongGyu Lee on 2024. 7. 9.
//

#ifndef PREVIEW_FUNCTIONAL_DEFAULT_SEARCHER_H_
#define PREVIEW_FUNCTIONAL_DEFAULT_SEARCHER_H_

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace preview {

template<typename ForwardIterator1, typename BinaryPredicate = std::equal_to<>>
class default_searcher {
 public:
  constexpr default_searcher(
      ForwardIterator1 pat_first, ForwardIterator1 pat_last, BinaryPredicate pred = BinaryPredicate())
      : pat_first_(pat_first)
      , pat_last_(pat_last)
      , pred_(pred) {}

  template<typename ForwardIterator2>
  constexpr std::pair<ForwardIterator2, ForwardIterator2> operator()(ForwardIterator2 first, ForwardIterator2 last) const {
    auto i = std::search(first, last, pat_first_, pat_last_);
    if (i == last)
      return {i, last};
    return {i, std::next(i, std::distance(pat_first_, pat_last_))};
  }

 private:
  ForwardIterator1 pat_first_;
  ForwardIterator1 pat_last_;
  BinaryPredicate pred_;
};

template<typename ForwardIterator1, typename BinaryPredicate = std::equal_to<>>
constexpr default_searcher<ForwardIterator1, BinaryPredicate>
make_default_searcher(ForwardIterator1 pat_first, ForwardIterator1 pat_last, BinaryPredicate pred = BinaryPredicate()) {
  return default_searcher<ForwardIterator1, BinaryPredicate>(std::move(pat_first), std::move(pat_last), std::move(pred));
}

} // namespace preview

#endif // PREVIEW_FUNCTIONAL_DEFAULT_SEARCHER_H_
