#include <lockfree-queue/mpsc_pc.h>

#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <variant>

#include "flat-combining/flat-combining.h"

namespace common::lib {
class Mutex {
 public:
  auto is_locked() noexcept -> bool { return m_state.load(std::memory_order::acquire) == MtxState::Locked; }

  auto try_lock() noexcept -> bool {
    auto exp = MtxState::Unlocked;
    return m_state.compare_exchange_strong(exp, MtxState::Locked);
  }

  static void lock() noexcept {
    // Must not be called.
    std::abort();
  }

  void unlock() noexcept { m_state.store(MtxState::Unlocked, std::memory_order::release); }

 private:
  enum class MtxState { Locked, Unlocked };

  std::atomic<MtxState> m_state = MtxState::Unlocked;
};

struct PublicationSlot {
  void *publication_record = nullptr;
  Mutex *combiner_mtx = nullptr;
  std::atomic_bool is_processed = false;
};

class FlatCombiner::Impl {
 public:
  explicit Impl(unsigned publication_queue_length)
      // Workaround till lockfree::MPSCPCQueueAny CalculateSize/Initialize is fixed.
      : m_publication_queue(sizeof(void *) * publication_queue_length + sizeof(lockfree::MPSCPCQueueAny)) {
    m_publication_slots.reserve(publication_queue_length);
  }

  void publish_record(PublicationSlot *pub_slot) {
    pub_slot->combiner_mtx = &m_combiner_mtx;
    if (!m_publication_queue.TryPush(&pub_slot, sizeof(pub_slot)))  // NOLINT(bugprone-sizeof-expression)
      throw std::runtime_error("FlatCombiner: Insufficient publication queue capacity");
  }

  void run_combiner(TECombiner combiner) noexcept {
    PublicationSlot *pub_slot = nullptr;

    assert(combiner_lock);
    assert(m_publication_slots.empty());
    while (m_publication_queue.TryPop(&pub_slot)) {
      assert(pub_slot->is_processed.load(std::memory_order::acquire) == false);
      m_publication_slots.push_back(pub_slot);
    }
    assert(!m_publication_slots.empty());

    if (m_publication_slots.size() > 1) {
      for (size_t i = 0, end = m_publication_slots.size() - 1; i < end; i++) {
        combiner(m_publication_slots[i]->publication_record, false);
      }
    }
    combiner(m_publication_slots.back()->publication_record, true);

    for (auto &slot : m_publication_slots) {
      slot->is_processed.store(true, std::memory_order::release);
    }
    m_publication_slots.clear();
  }

 private:
  lockfree::thread::MPSCPCQueueAny m_publication_queue;
  std::vector<PublicationSlot *> m_publication_slots;
  Mutex m_combiner_mtx;
};

struct processing_done {};
struct run_combiner {
  std::unique_lock<Mutex> lock;
};
struct wait {};

using status_t = std::variant<processing_done, run_combiner, wait>;

static auto is_processing_done(PublicationSlot &pub_slot) noexcept -> bool {
  return pub_slot.is_processed.load(std::memory_order::acquire);
}

static auto publication_slot_status(PublicationSlot &pub_slot) noexcept -> status_t {
  assert(pub_slot.publication_record != nullptr);
  assert(pub_slot.combiner_mtx != nullptr);

  if (is_processing_done(pub_slot)) return processing_done();
  if (pub_slot.combiner_mtx->is_locked()) return wait();

  // Try acquiring Combiner mutex.
  std::unique_lock lock(*pub_slot.combiner_mtx, std::try_to_lock);
  if (lock) return run_combiner{std::move(lock)};
  return wait();
}

template <typename... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void FlatCombiner::execute(void *my_publication_record, TECombiner combiner, SpinYield yield) {
  PublicationSlot pub_slot = {my_publication_record};

  impl->publish_record(&pub_slot);

  bool done = false;
  while (!done) {
    auto status = publication_slot_status(pub_slot);
    done = std::visit(overloaded{[&](wait &) {
                                   yield();
                                   return false;
                                 },
                                 [](processing_done &) { return true; },
                                 [&](run_combiner &) {
                                   if (!is_processing_done(pub_slot)) impl->run_combiner(combiner);
                                   return true;
                                 }},
                      status);
  }
}

FlatCombiner::FlatCombiner(unsigned publication_queue_length)
    : impl(std::make_shared<Impl>(publication_queue_length)) {}
}  // namespace common::lib