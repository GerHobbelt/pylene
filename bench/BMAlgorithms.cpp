#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/algorithm/generate.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/sort.hpp>
#include <algorithm>
#include <numeric>

namespace baseline
{

  template <class V>
  void fill(mln::image2d<V>& ima, V value)
  {
    std::ptrdiff_t stride = ima.index_strides()[0];
    V*             buffer = &ima.at(ima.domain().pmin);
    std::size_t    height = ima.nrows();
    std::size_t    width  = ima.ncols();

    for (std::size_t i = 0; i < height; ++i, buffer += stride)
      std::fill(buffer, buffer + width, value);
  }


  template <class V>
  void copy(const mln::image2d<V>& in, mln::image2d<V>& out)
  {
    std::ptrdiff_t istride = in.index_strides()[0];
    std::ptrdiff_t ostride = out.index_strides()[0];
    const V*       ibuffer = &in.at(in.domain().pmin);
    V*             obuffer = &out.at(out.domain().pmin);
    std::size_t    height  = in.nrows();
    std::size_t    width   = in.ncols();

    assert(out.nrows() == height);
    assert(out.ncols() == width);


    for (std::size_t i = 0; i < height; ++i, ibuffer += istride, obuffer += ostride)
      std::copy(ibuffer, ibuffer + width, obuffer);
  }


  template <class V>
  void paste(const mln::image2d<V>& in, mln::image2d<V>& out)
  {
    std::ptrdiff_t istride = in.index_strides()[0];
    std::ptrdiff_t ostride = out.index_strides()[0];
    const V*       ibuffer = &in.at(in.domain().pmin);
    V*             obuffer = &out.at(in.domain().pmin);
    std::size_t    height  = in.nrows();
    std::size_t    width   = in.ncols();

    assert(out.nrows() == height);
    assert(out.ncols() == width);

    for (std::size_t i = 0; i < height; ++i, ibuffer += istride, obuffer += ostride)
      std::copy(ibuffer, ibuffer + width, obuffer);
  }


  template <class F, class U, class V>
  void transform(const mln::image2d<U>& in, mln::image2d<V>& out, F f)
  {
    std::ptrdiff_t istride = in.index_strides()[0];
    std::ptrdiff_t ostride = out.index_strides()[0];
    const U*       ibuffer = &in.at(in.domain().pmin);
    V*             obuffer = &out.at(in.domain().pmin);
    std::size_t    height  = in.nrows();
    std::size_t    width   = in.ncols();

    assert(out.nrows() == height);
    assert(out.ncols() == width);

    for (std::size_t i = 0; i < height; ++i, ibuffer += istride, obuffer += ostride)
      std::transform(ibuffer, ibuffer + width, obuffer, f);
  }

  template <class F, class U>
  void for_each(mln::image2d<U>& in, F f)
  {
    std::ptrdiff_t istride = in.index_strides()[0];
    U*             ibuffer = &in.at(in.domain().pmin);
    std::size_t    height  = in.nrows();
    std::size_t    width   = in.ncols();

    for (std::size_t i = 0; i < height; ++i, ibuffer += istride)
      std::for_each(ibuffer, ibuffer + width, f);
  }


  template <class V, class Generator>
  void generate(mln::image2d<V>& ima, Generator g)
  {
    std::ptrdiff_t stride = ima.index_strides()[0];
    V*             buffer = &ima.at(ima.domain().pmin);
    std::size_t    height = ima.nrows();
    std::size_t    width  = ima.ncols();

    for (std::size_t i = 0; i < height; ++i, buffer += stride)
      std::generate(buffer, buffer + width, g);
  }

  template <class U, class V, class BinaryOperator>
  V accumulate(const mln::image2d<U>& ima, V init, BinaryOperator op)
  {
    std::ptrdiff_t stride = ima.index_strides()[0];
    const U*       buffer = &ima.at(ima.domain().pmin);
    std::size_t    height = ima.nrows();
    std::size_t    width  = ima.ncols();

    for (std::size_t i = 0; i < height; ++i, buffer += stride)
      init = op(init, std::accumulate(buffer, buffer + width, init, op));

    return init;
  }
}


void fill_baseline(mln::image2d<mln::uint8>& ima, mln::uint8 v) { baseline::fill(ima, v); }
void fill_baseline(mln::image2d<mln::rgb8>& ima, mln::rgb8 v) { baseline::fill(ima, v); }
void fill(mln::image2d<mln::uint8>& ima, mln::uint8 v) { mln::experimental::fill(ima, v); }
void fill(mln::image2d<mln::rgb8>& ima, mln::rgb8 v) { mln::experimental::fill(ima, v); }

void copy_baseline(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out) { return baseline::copy(in, out); }
void copy_baseline(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out){ return baseline::copy(in, out); }
void copy(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out) { return mln::experimental::copy(in, out); }
void copy(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out){ return mln::experimental::copy(in, out); }

void paste_baseline(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out) { return baseline::paste(in, out); }
void paste_baseline(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out){ return baseline::paste(in, out); }
void paste(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out) { return mln::experimental::paste(in, out); }
void paste(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out){ return mln::experimental::paste(in, out); }

namespace
{
  static auto plus_one = [](auto x) -> decltype(x) { return x + 1; };
  static auto plus_one_inplace = [](auto& x) { x += 1; };
}

void transform_baseline(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out) { return baseline::transform(in, out, plus_one); }
void transform_baseline(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out){ return baseline::transform(in, out, plus_one); }
void transform(const mln::image2d<mln::uint8>& in, mln::image2d<mln::uint8>& out) { return mln::experimental::transform(in, out, plus_one); }
void transform(const mln::image2d<mln::rgb8>& in, mln::image2d<mln::rgb8>& out){ return mln::experimental::transform(in, out, plus_one); }


void for_each_baseline(mln::image2d<mln::uint8>& in) { return baseline::for_each(in, plus_one_inplace); }
void for_each_baseline(mln::image2d<mln::rgb8>& in){ return baseline::for_each(in, plus_one_inplace); }
void for_each(mln::image2d<mln::uint8>& in) { return mln::experimental::for_each(in, plus_one_inplace); }
void for_each(mln::image2d<mln::rgb8>& in){ return mln::experimental::for_each(in, plus_one_inplace); }



namespace
{
  static int cpt = 0;
  static auto iota_generator = []() -> int { return cpt++; };
}


void generate_baseline(mln::image2d<mln::uint8>& ima) { baseline::generate(ima, iota_generator); }
void generate(mln::image2d<mln::uint8>& ima) { mln::experimental::generate(ima, iota_generator); }



#include <mln/accu/accumulators/sum.hpp>

int accumulate_baseline(const mln::image2d<mln::uint8>& ima) { return baseline::accumulate(ima, 0, std::plus<>{}); }
int accumulate(const mln::image2d<mln::uint8>& ima) { return mln::experimental::accumulate(ima, 0, std::plus<>{}); }
int accumulate_accu(const mln::image2d<mln::uint8>& ima) { return mln::experimental::accumulate(ima, mln::accu::features::sum<int>()); }



std::vector<mln::point2d> sort_points(const mln::image2d<mln::uint8>& ima) { return mln::experimental::sort_points(ima); }
std::vector<mln::point2d> sort_points(const mln::image2d<int>& ima) { return mln::experimental::sort_points(ima); }
std::vector<mln::point2d> sort_points(const mln::image2d<mln::rgb8>& ima) { return mln::experimental::sort_points(ima, mln::lexicographicalorder_less<mln::rgb8>()); }
