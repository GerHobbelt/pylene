
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <bench/neighborhood.hpp>

#include <gtest/gtest.h>


class CoreNeighborhood : public ::testing::Test
{
  virtual void SetUp() override
  {
    std::string filename = fixtures::ImagePath::concat_with_filename("lena.pgm");

    mln::io::imread(filename, m_input);
    m_input.extension().fill(0);

    m_h       = m_input.height();
    m_w       = m_input.width();
    m_stride  = m_input.stride();
    m_ibuffer = m_input.buffer();
  }

protected:
  using fun1_t = std::function<void(const mln::image2d<uint8_t>&, mln::image2d<uint8_t>&)>;
  using fun2_t = std::function<void(const uint8_t*, uint8_t*, int, int, std::ptrdiff_t)>;

  mln::image2d<uint8_t> run_with(fun1_t f) const;
  mln::image2d<uint8_t> run_with(fun2_t f) const;

private:
  int                      m_w, m_h;
  std::ptrdiff_t           m_stride;
  uint8_t*              m_ibuffer;

  mln::image2d<uint8_t> m_input;
};

mln::image2d<uint8_t> CoreNeighborhood::run_with(fun1_t f) const
{
  mln::image2d<uint8_t> output;
  mln::resize(output, m_input);

  f(m_input, output);
  return output;
}


mln::image2d<uint8_t> CoreNeighborhood::run_with(fun2_t f) const
{
  mln::image2d<uint8_t> output;
  mln::resize(output, m_input);

  uint8_t* obuffer = output.buffer();
  f(m_ibuffer, obuffer, m_w, m_h, m_stride);
  return output;
}



TEST_F(CoreNeighborhood, Algorithm_Sum)
{
  auto ref  = this->run_with(Sum_C);
  auto ima = this->run_with(Sum);

  ASSERT_IMAGES_EQ_EXP(ima, ref);
}


TEST_F(CoreNeighborhood, Algorithm_Average)
{
  auto ref  = this->run_with(Average_C);
  auto ima = this->run_with(Average);

  ASSERT_IMAGES_EQ_EXP(ima, ref);
}


TEST_F(CoreNeighborhood, Algorithm_Erosion)
{
  auto ref  = this->run_with(Erosion_C);
  auto ima = this->run_with(Erosion);

  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST_F(CoreNeighborhood, Algorithm_Isotropic_Diffusion)
{
  auto ref = this->run_with(Isotropic_Diffusion_C);
  auto ima = this->run_with(Isotropic_Diffusion);

  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST_F(CoreNeighborhood, Algorithm_Anisotropic_Diffusion)
{
  auto ref  = this->run_with(Anisotropic_Diffusion_C);
  auto ima = this->run_with(Anisotropic_Diffusion);

  ASSERT_IMAGES_EQ_EXP(ima, ref);
}
