#include <cmath>
#include <cstdlib>
#include <string>

#include "preview/algorithm.h"
#include "preview/expected.h"
#include "preview/optional.h"
#include "preview/ranges.h"
#include "preview/string_view.h"

#include "gtest.h"

using namespace std::literals;

enum class parse_error {
  invalid_input,
  overflow
};

auto parse_number(preview::string_view& str) -> preview::expected<double, parse_error> {
  const char* begin = str.data();
  char* end;
  double retval = std::strtod(begin, &end);

  if (begin == end)
    return preview::unexpected<parse_error>{parse_error::invalid_input};
  else if (std::isinf(retval))
    return preview::unexpected<parse_error>{parse_error::overflow};

  str.remove_prefix(end - begin);
  return retval;
}

TEST(VERSIONED(Expected), basics) {
  auto process = [](preview::string_view str) -> preview::expected<int, std::string> {
    return parse_number(str)
        .transform([](double v) { return static_cast<int>(v); })
        .transform_error([](parse_error e) {
          return e == parse_error::invalid_input ? "invalid input" : "overflow";
        });
  };

  EXPECT_EQ(process("42"), 42);
  EXPECT_EQ(process("42abc"), 42);
  EXPECT_EQ(process("meow").error_or(""), "invalid input");
  EXPECT_EQ(process("inf").error_or(""), "overflow");
}
