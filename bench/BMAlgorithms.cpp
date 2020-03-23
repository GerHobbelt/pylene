#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/count.hpp>
#include <mln/core/algorithm/count_if.hpp>
#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/generate.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/algorithm/parallel_pointwise_canvas.hpp>
#include <mln/core/algorithm/sort.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/concept/object.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <algorithm>
#include <numeric>


namespace baseline
{

  template <class V>
  void fill(mln::experimental::image2d<V>& ima, V value)
  {
    std::ptrdiff_t stride = ima.stride();
    V*             buffer = ima.buffer();
    int            height = ima.height();
    int            width  = ima.width();

    for (int i = 0; i < height; ++i, buffer += stride)
      std::fill(buffer, buffer + width, value);
  }

  template <class V>
  void copy(const mln::experimental::image2d<V>& in, mln::experimental::image2d<V>& out)
  {
    std::ptrdiff_t istride = in.stride();
    std::ptrdiff_t ostride = out.stride();
    const V*       ibuffer = in.buffer();
    V*             obuffer = out.buffer();
    int            height  = in.height();
    int            width   = in.width();

    assert(out.height() == height);
    assert(out.width() == width);

    for (int i = 0; i < height; ++i, ibuffer += istride, obuffer += ostride)
      std::copy(ibuffer, ibuffer + width, obuffer);
  }

  template <class V, class F>
  std::ptrdiff_t count_if(const mln::experimental::image2d<V>& in, F f)
  {
    std::ptrdiff_t istride = in.stride();
    const V*       ibuffer = in.buffer();
    int            height  = in.height();
    int            width   = in.width();

    std::ptrdiff_t k = 0;
    for (int i = 0; i < height; ++i, ibuffer += istride)
      k += std::count_if(ibuffer, ibuffer + width, f);

    return k;
  }

  template <class V>
  std::ptrdiff_t count(const mln::experimental::image2d<V>& in, V v)
  {
    std::ptrdiff_t istride = in.stride();
    const V*       ibuffer = in.buffer();
    int            height  = in.height();
    int            width   = in.width();

    std::ptrdiff_t k = 0;
    for (int i = 0; i < height; ++i, ibuffer += istride)
      k += std::count(ibuffer, ibuffer + width, v);

    return k;
  }

  template <class V>
  bool equal(const mln::experimental::image2d<V>& lhs, const mln::experimental::image2d<V>& rhs)
  {
    std::ptrdiff_t lhs_stride = lhs.stride();
    std::ptrdiff_t rhs_stride = rhs.stride();
    const V*       lhs_buffer = lhs.buffer();
    const V*       rhs_buffer = rhs.buffer();
    int            height     = lhs.height();
    int            width      = lhs.width();

    for (int i = 0; i < height; ++i, lhs_buffer += lhs_stride, rhs_buffer += rhs_stride)
      if (!std::equal(lhs_buffer, lhs_buffer + width, rhs_buffer))
        return false;

    return true;
  }

  template <class V>
  void paste(const mln::experimental::image2d<V>& in, mln::experimental::image2d<V>& out)
  {
    std::ptrdiff_t istride = in.stride();
    std::ptrdiff_t ostride = out.stride();
    const V*       ibuffer = in.buffer();
    V*             obuffer = out.buffer();
    int            height  = in.height();
    int            width   = in.width();

    assert(out.height() == height);
    assert(out.width() == width);

    for (int i = 0; i < height; ++i, ibuffer += istride, obuffer += ostride)
      std::copy(ibuffer, ibuffer + width, obuffer);
  }

  template <class F, class U, class V>
  void transform(const mln::experimental::image2d<U>& in, mln::experimental::image2d<V>& out, F f)
  {
    std::ptrdiff_t istride = in.stride();
    std::ptrdiff_t ostride = out.stride();
    const U*       ibuffer = in.buffer();
    V*             obuffer = out.buffer();
    int            height  = in.height();
    int            width   = in.width();

    assert(out.height() == height);
    assert(out.width() == width);

    for (int i = 0; i < height; ++i, ibuffer += istride, obuffer += ostride)
      std::transform(ibuffer, ibuffer + width, obuffer, f);
  }

  template <class F, class U>
  void for_each(mln::experimental::image2d<U>& in, F f)
  {
    std::ptrdiff_t istride = in.stride();
    U*             ibuffer = in.buffer();
    int            height  = in.height();
    int            width   = in.width();

    for (int i = 0; i < height; ++i, ibuffer += istride)
      std::for_each(ibuffer, ibuffer + width, f);
  }

  template <class V, class Generator>
  void generate(mln::experimental::image2d<V>& ima, Generator g)
  {
    std::ptrdiff_t stride = ima.stride();
    V*             buffer = ima.buffer();
    int            height = ima.height();
    int            width  = ima.width();

    for (int i = 0; i < height; ++i, buffer += stride)
      std::generate(buffer, buffer + width, g);
  }

  template <class U, class V, class BinaryOperator>
  V accumulate(const mln::experimental::image2d<U>& ima, V init, BinaryOperator op)
  {
    std::ptrdiff_t stride = ima.stride();
    const U*       buffer = ima.buffer();
    int            height = ima.height();
    int            width  = ima.width();

    for (int i = 0; i < height; ++i, buffer += stride)
      init = op(init, std::accumulate(buffer, buffer + width, init, op));

    return init;
  }
} // namespace baseline


void fill_baseline(mln::experimental::image2d<uint8_t>& ima, uint8_t v)
{
  baseline::fill(ima, v);
}
void fill_baseline(mln::experimental::image2d<mln::rgb8>& ima, mln::rgb8 v)
{
  baseline::fill(ima, v);
}
void fill(mln::experimental::image2d<uint8_t>& ima, uint8_t v)
{
  mln::fill(ima, v);
}
void fill(mln::experimental::image2d<mln::rgb8>& ima, mln::rgb8 v)
{
  mln::fill(ima, v);
}


void copy_baseline(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out)
{
  baseline::copy(in, out);
}
void copy_baseline(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out)
{
  baseline::copy(in, out);
}
void copy(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out)
{
  mln::experimental::copy(in, out);
}
void copy(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out)
{
  mln::experimental::copy(in, out);
}


namespace
{
  static auto is_uint8_15   = [](uint8_t v) { return v == 15; };
  static auto is_rgb8_black = [](mln::rgb8 v) { return v == mln::rgb8{0, 0, 0}; };
} // namespace

std::ptrdiff_t count_if_baseline(const mln::experimental::image2d<uint8_t>& in)
{
  return baseline::count_if(in, is_uint8_15);
}
std::ptrdiff_t count_if_baseline(const mln::experimental::image2d<mln::rgb8>& in)
{
  return baseline::count_if(in, is_rgb8_black);
}
std::ptrdiff_t count_if(const mln::experimental::image2d<uint8_t>& in)
{
  return mln::count_if(in, is_uint8_15);
}
std::ptrdiff_t count_if(const mln::experimental::image2d<mln::rgb8>& in)
{
  return mln::count_if(in, is_rgb8_black);
}


std::ptrdiff_t count_baseline(const mln::experimental::image2d<uint8_t>& in)
{
  return baseline::count(in, uint8_t(15));
}
std::ptrdiff_t count_baseline(const mln::experimental::image2d<mln::rgb8>& in)
{
  return baseline::count(in, mln::rgb8{0, 0, 0});
}
std::ptrdiff_t count(const mln::experimental::image2d<uint8_t>& in)
{
  return mln::count(in, uint8_t(15));
}
std::ptrdiff_t count(const mln::experimental::image2d<mln::rgb8>& in)
{
  return mln::count(in, mln::rgb8{0, 0, 0});
}


bool equal_baseline(const mln::experimental::image2d<uint8_t>& lhs, const mln::experimental::image2d<uint8_t>& rhs)
{
  return baseline::equal(lhs, rhs);
}
bool equal_baseline(const mln::experimental::image2d<mln::rgb8>& lhs, const mln::experimental::image2d<mln::rgb8>& rhs)
{
  return baseline::equal(lhs, rhs);
}
bool equal(const mln::experimental::image2d<uint8_t>& lhs, const mln::experimental::image2d<uint8_t>& rhs)
{
  return mln::equal(lhs, rhs);
}
bool equal(const mln::experimental::image2d<mln::rgb8>& lhs, const mln::experimental::image2d<mln::rgb8>& rhs)
{
  return mln::equal(lhs, rhs);
}


void paste_baseline(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out)
{
  baseline::paste(in, out);
}
void paste_baseline(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out)
{
  baseline::paste(in, out);
}
void paste(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out)
{
  mln::paste(in, out);
}
void paste(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out)
{
  mln::paste(in, out);
}


namespace
{
  static auto plus_one         = [](auto x) -> decltype(x) { return x + 1; };
  static auto plus_one_inplace = [](auto& x) { x += 1; };
} // namespace

void transform_baseline(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out)
{
  baseline::transform(in, out, plus_one);
}
void transform_baseline(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out)
{
  baseline::transform(in, out, plus_one);
}
void transform(const mln::experimental::image2d<uint8_t>& in, mln::experimental::image2d<uint8_t>& out)
{
  mln::transform(in, out, plus_one);
}
void transform(const mln::experimental::image2d<mln::rgb8>& in, mln::experimental::image2d<mln::rgb8>& out)
{
  mln::transform(in, out, plus_one);
}


void for_each_baseline(mln::experimental::image2d<uint8_t>& in)
{
  baseline::for_each(in, plus_one_inplace);
}
void for_each_baseline(mln::experimental::image2d<mln::rgb8>& in)
{
  baseline::for_each(in, plus_one_inplace);
}
void for_each(mln::experimental::image2d<uint8_t>& in)
{
  mln::for_each(in, plus_one_inplace);
}
void for_each(mln::experimental::image2d<mln::rgb8>& in)
{
  mln::for_each(in, plus_one_inplace);
}
void parallel_for_each(mln::experimental::image2d<uint8_t>& in)
{
  mln::ApplyPointwise(in, plus_one_inplace);
}


namespace
{
  static int  cpt            = 0;
  static auto iota_generator = []() -> int { return cpt++; };
} // namespace


void generate_baseline(mln::experimental::image2d<uint8_t>& ima)
{
  baseline::generate(ima, iota_generator);
}
void generate(mln::experimental::image2d<uint8_t>& ima)
{
  mln::generate(ima, iota_generator);
}


#include <mln/accu/accumulators/sum.hpp>

int accumulate_baseline(const mln::experimental::image2d<uint8_t>& ima)
{
  return baseline::accumulate(ima, 0, std::plus<>{});
}
int accumulate(const mln::experimental::image2d<uint8_t>& ima)
{
  return mln::accumulate(ima, 0, std::plus<>{});
}
int accumulate_accu(const mln::experimental::image2d<uint8_t>& ima)
{
  return mln::accumulate(ima, mln::accu::features::sum<int>());
}

using point_t = mln::image_point_t<mln::experimental::image2d<uint8_t>>;

std::vector<point_t> sort_points(const mln::experimental::image2d<uint8_t>& ima)
{
  return mln::experimental::sort_points(ima);
}
std::vector<point_t> sort_points(const mln::experimental::image2d<int>& ima)
{
  return mln::experimental::sort_points(ima);
}
std::vector<point_t> sort_points(const mln::experimental::image2d<mln::rgb8>& ima)
{
  return mln::experimental::sort_points(ima, mln::lexicographicalorder_less<mln::rgb8>());
}
