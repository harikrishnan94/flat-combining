#include <doctest/doctest.h>

#include <barrier>
#include <thread>
#include <vector>

#include "flat-combining/flat-combining.h"

using namespace common::lib;

TEST_SUITE_BEGIN("flat_combining");

TEST_CASE("basic test") {
  constexpr auto NumWorkers = 4;
  constexpr auto NumIncrementsPerWorker = 1'000'000;

  std::barrier barrier(NumWorkers);
  std::uint64_t counter = 0;
  FlatCombiner combiner(NumWorkers);

  {
    std::vector<std::jthread> workers;
    for (int i = 0; i < NumWorkers; i++) {
      workers.emplace_back([&] {
        barrier.arrive_and_wait();
        for (int i = 0; i < NumIncrementsPerWorker; i++) {
          combiner.Execute(counter, [](std::uint64_t &counter) { ++counter; });
        }
      });
    }
  }

  REQUIRE(counter == NumWorkers * NumIncrementsPerWorker);
}

TEST_SUITE_END();
