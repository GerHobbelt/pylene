#include "unhomo.hpp"
#include <mln/core/trace.hpp>

struct frgb
{
  float red, green, blue;

  frgb() {}
  frgb(float r, float g, float b) : red(r), green(g), blue(b) {}
  frgb(const mln::rgb8& c) : red(c[0]), green(c[1]), blue(c[2]) {}

  void set_inter(const frgb& c1, const frgb& c2, float delta)
  {
    red = c1.red + delta * (c2.red - c1.red);
    green = c1.green + delta * (c2.green - c1.green);
    blue = c1.blue + delta * (c2.blue - c1.blue);
  }

  operator mln::rgb8() const
  {
    return {mln::uint8(red+0.4999), mln::uint8(green+0.4999),
        mln::uint8(blue+0.4999)};
  }
};


template <unsigned i>
struct B_;

template <>
struct B_<0> {
  static float on(float u) { return (1.f - u) * (1.f - u) * (1.f - u); }
};

template <>
struct B_<1> {
  static float on(float u) { return 3.f * u * (1.f - u) * (1.f - u); }
};

template <>
struct B_<2> {
  static float on(float u) { return 3.f * u * u * (1.f - u); }
};

template <>
struct B_<3> {
  static float on(float u) { return u * u * u; }
};

struct bezier
{
  mln::point2df p[4][4]; // control points

  void interpolate_(mln::point2df start,
                    mln::point2df end,
                    mln::point2df& one_third,
                    mln::point2df& two_third)
  {
    mln::point2df diff = (end - start) / 3;
    one_third = start + diff;
    two_third = end - diff;
  }

  void init()
  {
    interpolate_(p[0][0], p[0][3], p[0][1], p[0][2]);
    interpolate_(p[0][0], p[3][0], p[1][0], p[2][0]);
    interpolate_(p[3][0], p[3][3], p[3][1], p[3][2]);
    interpolate_(p[0][3], p[3][3], p[1][3], p[2][3]);

    interpolate_(p[1][0], p[1][3], p[1][1], p[1][2]);
    interpolate_(p[2][0], p[2][3], p[2][1], p[2][2]);
  }

  mln::point2df Q(float u, float v) const
  {
    float
      B0u = B_<0>::on(u),
      B1u = B_<1>::on(u),
      B2u = B_<2>::on(u),
      B3u = B_<3>::on(u),
      B0v = B_<0>::on(v),
      B1v = B_<1>::on(v),
      B2v = B_<2>::on(v),
      B3v = B_<3>::on(v);

    return
      p[0][0] * B0u * B0v +
      p[0][1] * B0u * B1v +
      p[0][2] * B0u * B2v +
      p[0][3] * B0u * B3v +

      p[1][0] * B1u * B0v +
      p[1][1] * B1u * B1v +
      p[1][2] * B1u * B2v +
      p[1][3] * B1u * B3v +

      p[2][0] * B2u * B0v +
      p[2][1] * B2u * B1v +
      p[2][2] * B2u * B2v +
      p[2][3] * B2u * B3v +

      p[3][0] * B3u * B0v +
      p[3][1] * B3u * B1v +
      p[3][2] * B3u * B2v +
      p[3][3] * B3u * B3v;
  }

  mln::rgb8 bilinear(const mln::image2d<mln::rgb8>& input,
                     mln::point2df fp)
  {
    constexpr mln::point2d down = {1,0};
    constexpr mln::point2d right = {0,1};

    mln::point2d p = fp;
    mln::point2df delta = fp - p;
    float dr = delta[0], dc = delta[1];
    frgb top, bot, c;
    top.set_inter(input(p), input(p + right), dc);
    bot.set_inter(input(p + down), input(p + down + right), dc);
    c.set_inter(top, bot, dr);
    return c;
  }

  void fill(const mln::image2d<mln::rgb8>& input,
            mln::image2d<mln::rgb8>& output)
  {
    init();
    const unsigned
      nrows = output.nrows(),
      ncols = output.ncols();

    for (unsigned row = 0; row < nrows; ++row)
      for (unsigned col = 0; col < ncols; ++col)
        {
          float
            u = float(row) / float(nrows),
            v = float(col) / float(ncols);
          mln::point2df p = Q(u, v);
          output.at(row, col) = bilinear(input, p);
        }
  }

};



mln::image2d<mln::rgb8>
unhomo(const mln::image2d<mln::rgb8>& f,
       pquad_t points)
{
  mln_entering("unhomo");

  bezier b;

  b.p[0][0] = points[0];
  b.p[0][3] = points[1];
  b.p[3][0] = points[2];
  b.p[3][3] = points[3];

  mln::image2d<mln::rgb8> output(2970, 2100);
  b.fill(f, output);

  mln_exiting();
  return output;
}
