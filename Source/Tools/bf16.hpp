#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <type_traits>

struct bf16 {
  uint16_t v; // raw 16-bit storage (1-8-7 layout: s eeeeeeee mmmmmmm)

  // --- constructors ---
  constexpr bf16() : v(0) {}                   // default
  constexpr explicit bf16(uint16_t raw, int) : v(raw) {} // raw-bits ctor tag (int to differentiate)
  bf16(const bf16&) = default;

  // factory for raw bits (constexpr)
  static constexpr bf16 from_bits(uint16_t raw) { return bf16(raw, 0); }
  // expose raw bits if you need them elsewhere (constexpr)
  constexpr uint16_t bits() const { return v; }

  // from float (round-to-nearest-even). Not constexpr by design.
  explicit bf16(float x) {
    // NaN: preserve top 16 payload bits, ensure mantissa nonzero
    if (std::isnan(x)) {
      uint32_t bits; std::memcpy(&bits, &x, 4);
      v = uint16_t((bits >> 16) | 0x0040);
      return;
    }
    uint32_t bits; std::memcpy(&bits, &x, 4);
    // add rounding bias: 0x7FFF + LSB of truncated field (ties to even)
    uint32_t lsb = (bits >> 16) & 1u;
    uint32_t rounding_bias = 0x7FFFu + lsb;
    bits += rounding_bias;
    v = uint16_t(bits >> 16);
  }

  // from double via float
  explicit bf16(double x) : bf16(static_cast<float>(x)) {}

  // to float (not constexpr)
  operator float() const {
    uint32_t bits = uint32_t(v) << 16;
    float out; std::memcpy(&out, &bits, 4);
    return out;
  }

  // arithmetic via promotion
  friend bf16 operator+(bf16 a, bf16 b){ return bf16(float(a)+float(b)); }
  friend bf16 operator-(bf16 a, bf16 b){ return bf16(float(a)-float(b)); }
  friend bf16 operator*(bf16 a, bf16 b){ return bf16(float(a)*float(b)); }
  friend bf16 operator/(bf16 a, bf16 b){ return bf16(float(a)/float(b)); }

  // comparisons via promotion
  friend bool operator<(bf16 a, bf16 b){ return float(a) <  float(b); }
  friend bool operator>(bf16 a, bf16 b){ return float(a) >  float(b); }
  friend bool operator<=(bf16 a, bf16 b){ return float(a) <= float(b); }
  friend bool operator>=(bf16 a, bf16 b){ return float(a) >= float(b); }
  friend bool operator==(bf16 a, bf16 b){ return float(a) == float(b); }
  friend bool operator!=(bf16 a, bf16 b){ return float(a) != float(b); }
};

// numeric_limits specialization
namespace std {
  template<> struct numeric_limits<bf16> {
    static constexpr bool is_specialized = true;
    static constexpr int  digits   = 8;   // p = 8 (1 implicit + 7 fraction)
    static constexpr int  digits10 = 2;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;

    static constexpr int  max_exponent = 128;   // like float32
    static constexpr int  min_exponent = -125;  // like float32

    // NOTE: return constants via raw bfloat16 bit patterns (constexpr)
    // Layout: [sign:1][exp:8][mant:7], bias=127

    // min positive normal: exp=1, mant=0 -> 0x0080
    static constexpr bf16 min() noexcept { return bf16::from_bits(0x0080); }

    // largest finite: exp=254, mant=all ones -> 0x7F7F
    static constexpr bf16 max() noexcept { return bf16::from_bits(0x7F7F); }

    // lowest (most negative finite): sign=1 + max finite -> 0xFF7F
    static constexpr bf16 lowest() noexcept { return bf16::from_bits(0xFF7F); }

    // epsilon (distance between 1 and next): 2^{-7} -> exp=120 -> 0x3C00
    static constexpr bf16 epsilon() noexcept { return bf16::from_bits(0x3C00); }

    // +infinity: exp=255, mant=0 -> 0x7F80
    static constexpr bf16 infinity() noexcept { return bf16::from_bits(0x7F80); }

    // quiet NaN: exp=255, mant msb=1 -> 0x7FC0
    static constexpr bf16 quiet_NaN() noexcept { return bf16::from_bits(0x7FC0); }

    // signaling NaN: exp=255, mant=least -> 0x7F81
    static constexpr bf16 signaling_NaN() noexcept { return bf16::from_bits(0x7F81); }
  };
} // namespace std
