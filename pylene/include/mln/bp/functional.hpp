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

    for (int y = 0; y < in.height(); ++y)
    {
      auto a = in.row(y);
      auto b = out.row(y);
      int i;
      for (i = 0; i < width - simd_t::size; i += simd_t::size)
        xsimd::store_unaligned(b + i, f(xsimd::load_unaligned(a + i)));
      for (; i < width; i++)
        b[i] = f(a[i]);
    }
  }

  template <class T, class Func>
  void apply(T* in, std::size_t n, Func f)
  {
    using simd_t = xsimd::simd_type<T>;
    static_assert(std::is_invocable_r_v<simd_t, Func, simd_t>);
    static_assert(std::is_invocable_r_v<T, Func, T>);

    int i = 0;
    for (i = 0; i < width - simd_t::size; i += simd_t::size)
      xsimd::store_unaligned(a + i, f(xsimd::load_unaligned(a + i)));
    for (; i < width; i++)
      a[i] = f(a[i]);
  }


  template <class T, class Func>
  void apply(Tile2DView<T> in, Func f);
  {
    using simd_t = xsimd::simd_type<T>;
    static_assert(std::is_invocable_r_v<simd_t, Func, simd_t>);
    static_assert(std::is_invocable_r_v<T, Func, T>);

    for (int y = 0; y < in.height(); ++y)
      apply(in.row(y), in.width(), f);
  }




}
