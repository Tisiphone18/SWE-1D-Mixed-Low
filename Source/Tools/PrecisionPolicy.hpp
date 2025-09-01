


#pragma once

namespace Precision {

  struct MixedASafe {
    using Store = _Float16;   // global state
    using Work  = double;  // arithmetic
    using Accum = double;  // accumulators

    static constexpr Work G      = 9.81;
    static constexpr Work H_MIN  = 1e-6;
    static constexpr double CFL  = 0.9;
    static constexpr bool use_kahan = false;
    static constexpr bool keep_bathymetry_in_f32 = true;
    static constexpr const char* name = "Mixed A (safe)";
  };

  struct MixedBAggressive {
    using Store = __bf16;
    using Work  = float;
    using Accum = double;

    static constexpr Work G      = 9.81f;
    static constexpr Work H_MIN  = 5e-4f;
    static constexpr double CFL  = 0.8;
    static constexpr bool use_kahan = true;
    static constexpr bool keep_bathymetry_in_f32 = true;
    static constexpr const char* name = "Mixed B (aggressive)";
  };

  struct MixedC {
    using Store = float;
    using Work  = double;
    using Accum = double;

    static constexpr Work G      = 9.81f;
    static constexpr Work H_MIN  = 1e-6;
    static constexpr double CFL  = 0.8;
    static constexpr bool use_kahan = true;
    static constexpr bool keep_bathymetry_in_f32 = true;
    static constexpr const char* name = "Mixed C";
  };

} // namespace Precision
