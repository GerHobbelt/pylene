#pragma once

#include <mln/bp/tile.hpp>
#include <xsimd/xsimd.hpp>

#include <concepts>

namespace mln::bp
{

  template <class T, class Func>
  void transform(Tile2DView<T> in, Tile2DView<T> out, Func f);


  template <class T, class Func>
  void apply(Tile2DView<T> in, Func f);

  template <class T, class Func>
  void apply(T* in, std::size_t n, Func f);



  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T, class Func>
  void transform(Tile2DView<T> in, Tile2DView<T> out, Func f)
  {
    using simd_t = xsimd::simd_type<T>;
    static_assert(std::is_invocable_r_v<simd_t, Func, simd_t>);
    static_assert(std::is_invocable_r_v<T, Func, T>);

    assert(in.width() == out.width());
    assert(in.height() == out.height());

    int w = in.width();
    for (int y = 0; y < in.height(); ++y)
    {
      auto a = in.row(y);
      auto b = out.row(y);
      int i;
      for (i = 0; (i + simd_t::size) < w; i += simd_t::size)
        xsimd::store_unaligned(b + i, f(xsimd::load_unaligned(a + i)));
      for (; i < w; i++)
        b[i] = f(a[i]);
    }
  }

  template <class T, class Func>
  void apply(T* in, std::size_t n, Func f)
  {
    using simd_t = xsimd::simd_type<T>;
    static_assert(std::is_invocable_r_v<simd_t, Func, simd_t>);
    static_assert(std::is_invocable_r_v<T, Func, T>);

    std::size_t i = 0;
    for (i = 0; (i + simd_t::size) < n; i += simd_t::size)
      xsimd::store_unaligned(in + i, f(xsimd::load_unaligned(in + i)));
    for (; i < n; i++)
      in[i] = f(in[i]);
  }


  template <class T, class Func>
  void apply(Tile2DView<T> in, Func f)
  {
    using simd_t = xsimd::simd_type<T>;
    static_assert(std::is_invocable_r_v<simd_t, Func, simd_t>);
    static_assert(std::is_invocable_r_v<T, Func, T>);

    for (int y = 0; y < in.height(); ++y)
      apply(in.row(y), in.width(), f);
  }




}
