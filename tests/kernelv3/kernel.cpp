#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/kernelv3/kernel.hpp>

#define BOOST_TEST_MODULE Algorithm
#include <tests/test.hpp>

namespace ker = mln::kernel;

BOOST_AUTO_TEST_CASE(LinearKernel_SelfAssignment)
{
  mln::image2d<mln::uint8> f(10,10);
  mln::image2d<mln::uint8> g(10,10);
  mln::iota(f, 0);
  mln::iota(g, 0);

  {
    auto G = ker::make_image<0>(g);
    auto expr = (G(ker::p) = G(ker::p));
    ker::exec(expr);
  }
  imcheck_equal(g, f);
}

BOOST_AUTO_TEST_CASE(LinearKernel_Assignment)
{
  mln::image2d<mln::uint8> f(10,10);
  mln::image2d<mln::uint8> g(10,10);
  mln::iota(f, 0);

  {
    auto F = ker::make_image<0>(f);
    auto G = ker::make_image<1>(g);
    auto expr = (G(ker::p) = F(ker::p));
    ker::exec(expr);
  }
  imcheck_equal(g, f);
}

BOOST_AUTO_TEST_CASE(LinearKernel_ConstImage)
{
  mln::image2d<mln::uint8> f(10,10);
  mln::image2d<mln::uint8> g(10,10);
  mln::iota(f, 0);

  {
    const auto& const_f = f;
    auto F = ker::make_image<0>(const_f);
    auto G = ker::make_image<1>(g);
    auto expr = (G(ker::p) = F(ker::p));
    ker::exec(expr);
  }
  imcheck_equal(g, f);
}


BOOST_AUTO_TEST_CASE(LinearKernel_NoOverflow)
{
  mln::image2d<mln::uint8> f(10,10);
  mln::image2d<mln::uint8> g(10,10);
  mln::iota(f, 0);

  {
    auto F = ker::make_image<0>(f);
    auto G = ker::make_image<1>(g);
    auto expr = (G(ker::p) = F(ker::p) + F(ker::p));
    ker::exec(expr);
  }
  imcheck_equal(g, 2 * f);
}

BOOST_AUTO_TEST_CASE(LinearKernel_WithRhsMorpher)
{
  mln::image2d<mln::uint8> f(10,10);
  mln::image2d<mln::uint8> g(10,10);
  mln::iota(f, 0);

  auto double_f = 2 * f;
  {
    auto F = ker::make_image<0>(double_f);
    auto G = ker::make_image<1>(g);
    auto expr = (G(ker::p) = F(ker::p));
    ker::exec(expr);
  }
  imcheck_equal(g, double_f);
}

BOOST_AUTO_TEST_CASE(LinearKernel_WithLhsMorpher)
{
  mln::image2d<mln::uint8> f(10,10);
  mln::image2d<mln::uint8> g(10,10);
  mln::iota(f, 0);

  auto id = [](mln::uint8& x) -> mln::uint8& { return x; };
  auto id_g = imtransform(g, id);
  {
    auto F = ker::make_image<0>(f);
    auto G = ker::make_image<1>(id_g);
    auto expr = (G(ker::p) = F(ker::p));
    ker::exec(expr);
  }
  imcheck_equal(g, f);
}


