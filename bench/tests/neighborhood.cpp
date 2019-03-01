#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <bench/neighborhood.hpp>

#include <gtest/gtest.h>


class CoreNeighborhood : public ::testing::Test
{
  virtual void SetUp() override
  {
    std::string             filename = fixtures::ImagePath::concat_with_filename("lena.ppm");
    mln::image2d<mln::rgb8> tmp(0, 0);
    mln::io::imread(filename, tmp);

    mln::resize(m_input, tmp);
    mln::copy(mln::red(tmp), m_input);
    mln::extension::fill(m_input, 0);

    m_h       = m_input.nrows();
    m_w       = m_input.ncols();
    m_stride  = m_input.strides()[0];
    m_ibuffer = &m_input.at(0, 0);
  }

protected:
  using fun1_t = std::function<void(const mln::image2d<mln::uint8>&, mln::image2d<mln::uint8>&)>;
  using fun2_t = std::function<void(const mln::uint8*, mln::uint8*, int, int, std::ptrdiff_t)>;

  mln::image2d<mln::uint8> run_with(fun1_t f) const;
  mln::image2d<mln::uint8> run_with(fun2_t f) const;

private:
  int                      m_w, m_h;
  std::ptrdiff_t           m_stride;
  mln::uint8*              m_ibuffer;
  mln::image2d<mln::uint8> m_input;
};

mln::image2d<mln::uint8> CoreNeighborhood::run_with(fun1_t f) const
{
  mln::image2d<mln::uint8> output;
  mln::resize(output, m_input);

  f(m_input, output);
  return output;
}

mln::image2d<mln::uint8> CoreNeighborhood::run_with(fun2_t f) const
{
  mln::image2d<mln::uint8> output;
  mln::resize(output, m_input);

  mln::uint8* obuffer = &output.at(0, 0);
  f(m_ibuffer, obuffer, m_w, m_h, m_stride);
  return output;
}

TEST_F(CoreNeighborhood, Algorithm_Sum)
{
  auto ref  = this->run_with(Sum_C);
  auto ima1 = this->run_with(Sum_New);
  auto ima2 = this->run_with(Sum);

  ASSERT_IMAGES_EQ(ima1, ref);
  ASSERT_IMAGES_EQ(ima2, ref);
}


TEST_F(CoreNeighborhood, Algorithm_Average)
{
  auto ref  = this->run_with(Average_C);
  auto ima1 = this->run_with(Average_New);
  auto ima2 = this->run_with(Average);

  ASSERT_IMAGES_EQ(ima1, ref);
  ASSERT_IMAGES_EQ(ima2, ref);
}


TEST_F(CoreNeighborhood, Algorithm_Erosion)
{
  auto ref  = this->run_with(Erosion_C);
  auto ima1 = this->run_with(Erosion_New);
  auto ima2 = this->run_with(Erosion);

  ASSERT_IMAGES_EQ(ima1, ref);
  ASSERT_IMAGES_EQ(ima2, ref);
}

TEST_F(CoreNeighborhood, Algorithm_Isotropic_Diffusion)
{
  auto ref  = this->run_with(Isotropic_Diffusion_C);
  auto ima1 = this->run_with(Isotropic_Diffusion_New);
  auto ima2 = this->run_with(Isotropic_Diffusion);

  ASSERT_IMAGES_EQ(ima1, ref);
  ASSERT_IMAGES_EQ(ima2, ref);
}

TEST_F(CoreNeighborhood, Algorithm_Anisotropic_Diffusion)
{
  auto ref  = this->run_with(Anisotropic_Diffusion_C);
  auto ima1 = this->run_with(Anisotropic_Diffusion_New);
  auto ima2 = this->run_with(Anisotropic_Diffusion);

  ASSERT_IMAGES_EQ(ima1, ref);
  ASSERT_IMAGES_EQ(ima2, ref);
}
