#pragma once

#include <bit>
#include <concepts>
#include <utility>

namespace common::lib {
template <typename Signature>
class FunctionPtr;

template <typename Res, typename... ArgTypes>
class FunctionPtr<Res(ArgTypes...)> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  FunctionPtr(std::invocable<ArgTypes...> auto &function)
      : m_function(std::bit_cast<void *>(&function)), m_invoker(invoker<std::decay_t<decltype(function)>>::invoke) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  FunctionPtr(Res (*function)(ArgTypes...)) : m_function(std::bit_cast<void *>(function)), m_invoker(simple_invoke) {}

  auto operator()(ArgTypes... args) const -> Res { return m_invoker(m_function, std::forward<ArgTypes>(args)...); }

 private:
  template <typename FunctorType>
  struct invoker {
    static auto invoke(void *functor, ArgTypes... args) -> Res {
      return (*std::bit_cast<FunctorType *>(functor))(std::forward<ArgTypes>(args)...);
    }
  };

  static auto simple_invoke(void *function, ArgTypes... args) -> Res {
    return (std::bit_cast<Res (*)(ArgTypes...)>(function))(std::forward<ArgTypes>(args)...);
  }

  void *m_function;
  Res (*m_invoker)(void *functor, ArgTypes...);
};

// C++17 Deduction guidelines: Adapted from libcxx source
template <typename Rp, typename... Ap>
FunctionPtr(Rp (*)(Ap...)) -> FunctionPtr<Rp(Ap...)>;

namespace detail {
template <typename Fp>
struct strip_signature;

template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...)> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) volatile> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const volatile> {
  using type = Rp(Ap...);
};

template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) &> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const &> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) volatile &> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const volatile &> {
  using type = Rp(Ap...);
};

template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) noexcept> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const noexcept> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) volatile noexcept> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const volatile noexcept> {
  using type = Rp(Ap...);
};

template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) &noexcept> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const &noexcept> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) volatile &noexcept> {
  using type = Rp(Ap...);
};
template <typename Rp, typename Gp, typename... Ap>
struct strip_signature<Rp (Gp::*)(Ap...) const volatile &noexcept> {
  using type = Rp(Ap...);
};
}  // namespace detail

template <typename Fp, typename Stripped = typename detail::strip_signature<decltype(&Fp::operator())>::type>
FunctionPtr(Fp) -> FunctionPtr<Stripped>;

}  // namespace common::lib