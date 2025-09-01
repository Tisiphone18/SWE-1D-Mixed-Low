#pragma once
#include <cmath>
#include <type_traits>
#include <sstream>
#include <iostream>

#include "RealType.hpp"   // defines RealType, ComputeType, bf16 (if used)

/* -----------------------------------------------------------------------
   Helper: promote to a safe math type for the call, then cast back.
   - For half/bfloat16: compute in float
   - Otherwise: compute in double (or keep native where exact overload exists)
   ----------------------------------------------------------------------- */

namespace detail {

  template <class T>
  struct safe_compute_for_arg {
    using type = std::conditional_t<
      std::is_same_v<T, _Float16> || std::is_same_v<T, __bf16>,
      float,
      double>;
  };

  template <class T, class U>
  struct safe_compute_for_pair {
    static constexpr bool has_half =
      std::is_same_v<T, _Float16> || std::is_same_v<T, bf16> ||
      std::is_same_v<U, _Float16> || std::is_same_v<U, bf16>;
    using type = std::conditional_t<has_half, float, double>;
  }

  ; // end struct

  // bring std overload sets into ADL scope once
  inline void import_std_math() {
    using std::sqrt; using std::pow;
    using std::fabs; using std::fma;
    using std::fmax; using std::fmin;
  }

} // namespace detail

/* =========================
   RealType versions
   ========================= */

// sqrt
inline RealType sqrt_real(RealType x) {
  detail::import_std_math();
  using C = typename detail::safe_compute_for_arg<RealType>::type;
  using std::sqrt;
  const C r = sqrt(static_cast<C>(x));
  return static_cast<RealType>(r);
}

// pow
inline RealType pow_real(RealType base, RealType exp) {
  detail::import_std_math();
  using C = typename detail::safe_compute_for_pair<RealType, RealType>::type;
  using std::pow;
  const C r = pow(static_cast<C>(base), static_cast<C>(exp));
  return static_cast<RealType>(r);
}

// abs
inline RealType abs_real(RealType x) {
  using std::fabs;
  using C = typename detail::safe_compute_for_arg<RealType>::type;
  const C r = fabs(static_cast<C>(x));
  return static_cast<RealType>(r);
}

// fma
inline RealType fma_real(RealType a, RealType b, RealType c) {
  using std::fma;
  using C = typename detail::safe_compute_for_pair<RealType, RealType>::type;
  const C r = fma(static_cast<C>(a), static_cast<C>(b), static_cast<C>(c));
  return static_cast<RealType>(r);
}

// fmax / fmin
inline RealType fmax_real(RealType a, RealType b) {
  using std::fmax;
  using C = typename detail::safe_compute_for_pair<RealType, RealType>::type;
  const C r = fmax(static_cast<C>(a), static_cast<C>(b));
  return static_cast<RealType>(r);
}

inline RealType fmin_real(RealType a, RealType b) {
  using std::fmin;
  using C = typename detail::safe_compute_for_pair<RealType, RealType>::type;
  const C r = fmin(static_cast<C>(a), static_cast<C>(b));
  return static_cast<RealType>(r);
}

// max/min (C++ versions; preserve sign rules of std::max/min)
inline RealType max_real(RealType a, RealType b){
  return (a < b) ? b : a;
}
inline RealType min_real(RealType a, RealType b){
  return (b < a) ? b : a;
}

/* IO helpers for RealType */
inline std::istringstream& read_real(std::istringstream& ss, RealType& value) {
#if defined(ENABLE_HALF_PRECISION) || defined(ENABLE_FLOAT16_MIXED)
  float tmp; ss >> tmp; value = static_cast<_Float16>(tmp);
#elif defined(ENABLE_BFLOAT16_MIXED) || defined(ENABLE_HALFB_PRECISION)
  float tmp; ss >> tmp; value = static_cast<__bf16>(tmp);
#else
  ss >> value;
#endif
  return ss;
}

inline void print_real(const RealType& value, std::ostream& os = std::cout) {
#if defined(ENABLE_HALF_PRECISION) || defined(ENABLE_FLOAT16_MIXED) || defined(ENABLE_HALFB_PRECISION)
  os << static_cast<float>(value);
#elif defined(ENABLE_BFLOAT16_MIXED)
  os << static_cast<float>(value);
#else
  os << value;
#endif
}

/* =========================
   ComputeType versions
   ========================= */

// sqrt
inline ComputeType sqrt_compute(ComputeType x) {
  detail::import_std_math();
  using C = typename detail::safe_compute_for_arg<ComputeType>::type;
  using std::sqrt;
  const C r = sqrt(static_cast<C>(x));
  return static_cast<ComputeType>(r);
}

// pow
inline ComputeType pow_compute(ComputeType base, ComputeType exp) {
  detail::import_std_math();
  using C = typename detail::safe_compute_for_pair<ComputeType, ComputeType>::type;
  using std::pow;
  const C r = pow(static_cast<C>(base), static_cast<C>(exp));
  return static_cast<ComputeType>(r);
}

// abs
inline ComputeType abs_compute(ComputeType x) {
  using std::fabs;
  using C = typename detail::safe_compute_for_arg<ComputeType>::type;
  const C r = fabs(static_cast<C>(x));
  return static_cast<ComputeType>(r);
}

// fma
inline ComputeType fma_compute(ComputeType a, ComputeType b, ComputeType c) {
  using std::fma;
  using C = typename detail::safe_compute_for_pair<ComputeType, ComputeType>::type;
  const C r = fma(static_cast<C>(a), static_cast<C>(b), static_cast<C>(c));
  return static_cast<ComputeType>(r);
}

// fmax / fmin
inline ComputeType fmax_compute(ComputeType a, ComputeType b) {
  using std::fmax;
  using C = typename detail::safe_compute_for_pair<ComputeType, ComputeType>::type;
  const C r = fmax(static_cast<C>(a), static_cast<C>(b));
  return static_cast<ComputeType>(r);
}
inline ComputeType fmin_compute(ComputeType a, ComputeType b) {
  using std::fmin;
  using C = typename detail::safe_compute_for_pair<ComputeType, ComputeType>::type;
  const C r = fmin(static_cast<C>(a), static_cast<C>(b));
  return static_cast<ComputeType>(r);
}

// max/min for ComputeType
inline ComputeType max_compute(ComputeType a, ComputeType b){
  return (a < b) ? b : a;
}
inline ComputeType min_compute(ComputeType a, ComputeType b){
  return (b < a) ? b : a;
}
