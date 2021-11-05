#include <doctest/doctest.h>

#include <functional>

#include "flat-combining/function_ptr.h"

using namespace common::lib;

TEST_SUITE_BEGIN("function_ptr");

static auto inc(int a) -> int { return a + 1; }

TEST_CASE("function pointer test") {
  FunctionPtr inc_ptr = inc;

  REQUIRE(inc_ptr(1) == inc(1));
}

TEST_CASE("lamdba test") {
  auto inc = [val = 2](int a) { return a + val; };
  FunctionPtr inc_ptr = inc;

  REQUIRE(inc_ptr(1) == inc(1));
}

TEST_CASE("functor test") {
  struct inc_t {
    int val;  // NOLINT

    auto operator()(int a) const noexcept -> int { return a + val; }
  };

  inc_t inc = {2};
  FunctionPtr inc_ptr = inc;

  REQUIRE(inc_ptr(1) == inc(1));
}

TEST_SUITE_END();
