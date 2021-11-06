#pragma once

#include <immintrin.h>

#include <memory>
#include <span>

#include "flat-combining/function_ptr.h"

namespace common::lib {
class FlatCombiner {
 public:
  explicit FlatCombiner(unsigned publication_queue_length);

  // must not throw
  using SpinYield = FunctionPtr<void()>;

  // Combiner must not throw
  template <typename PublicationRecordType>
  void Execute(PublicationRecordType &my_publication_record,
               std::invocable<PublicationRecordType &, bool> auto &&combiner,
               SpinYield yield = mm_pause) {
    auto te_combiner = [=, combiner = std::forward<std::decay_t<decltype(combiner)>>(combiner)](void *pub_record,
                                                                                                bool is_last) {
      combiner(*static_cast<PublicationRecordType *>(pub_record), is_last);
    };
    execute(&my_publication_record, te_combiner, yield);
  }

  // Combiner must not throw
  template <typename PublicationRecordType>
  void Execute(PublicationRecordType &my_publication_record,
               std::invocable<PublicationRecordType &> auto &&combiner,
               SpinYield yield = mm_pause) {
    auto te_combiner = [=, combiner = std::forward<std::decay_t<decltype(combiner)>>(combiner)](void *pub_record,
                                                                                                bool /*is_last*/) {
      combiner(*static_cast<PublicationRecordType *>(pub_record));
    };
    execute(&my_publication_record, te_combiner, yield);
  }

 private:
  class Impl;

  using TECombiner = FunctionPtr<void(void *, bool is_last)>;

  static constexpr auto mm_pause = [] { _mm_pause(); };

  void execute(void *my_publication_record, TECombiner combiner, SpinYield yield);

  std::shared_ptr<Impl> impl;
};
}  // namespace common::lib