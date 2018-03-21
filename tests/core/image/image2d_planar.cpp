#include <boost/concept/assert.hpp>
// redefine concept assert, suppress the warning etc.
#undef BOOST_CONCEPT_ASSERT
#define BOOST_CONCEPT_ASSERT(Model)

#include <mln/core/image/image2d_planar.hpp>
#include <mln/core/colors.hpp>
#include <mln/io/imprint.hpp>

#include <numeric>
#include <gtest/gtest.h>

TEST(UTImage2DPlanar, From_Buffer)
{
  int buffer[12];
  std::iota(buffer, buffer + 12, 1);

  void* buffers[1] = {static_cast<void*>(buffer)};

  mln::box2d domain = {{0,0}, {3, 4}};
  auto ima = mln::image2d_planar<int>::from_buffer(buffers, domain);
  mln::io::imprint(ima);

  //FIXME: real test
}

TEST(UTImage2DPlanar, From_Buffer_strided)
{
  int buffer[18];
  std::iota(buffer, buffer + 18, 1);

  void* buffers[1] = {static_cast<void*>(buffer)};
  std::size_t strides[2] = {6 * sizeof(int), sizeof(int)};

  mln::box2d domain = {{0,0}, {3, 4}};
  auto ima = mln::image2d_planar<int>::from_buffer(buffers, domain, strides);
  mln::io::imprint(ima);

  //FIXME: real test
}


TEST(UTImage2DPlanar, From_Buffer_RGB)
{
  mln::uint8 bufferR[12];
  mln::uint8 bufferG[12];
  mln::uint8 bufferB[12];
  std::iota(bufferR, bufferR + 12, 10);
  std::iota(bufferG, bufferG + 12, 20);
  std::iota(bufferB, bufferB + 12, 30);

  void* buffers[3] = {static_cast<void*>(bufferR),
                     static_cast<void*>(bufferG),
                     static_cast<void*>(bufferB)};

  mln::box2d domain = {{0,0}, {3, 4}};
  auto ima = mln::image2d_planar<mln::rgb8>::from_buffer(buffers, domain);
  mln::io::imprint(ima);

  //FIXME: real test
}

TEST(UTImage2DPlanar, From_Buffer_RGB_strided)
{
  mln::uint8 bufferR[18];
  mln::uint8 bufferG[18];
  mln::uint8 bufferB[18];
  std::iota(bufferR, bufferR + 18, 10);
  std::iota(bufferG, bufferG + 18, 20);
  std::iota(bufferB, bufferB + 18, 30);

  void* buffers[3] = {static_cast<void*>(bufferR),
                     static_cast<void*>(bufferG),
                     static_cast<void*>(bufferB)};
  std::size_t strides[2] = {6 * sizeof(mln::uint8), sizeof(mln::uint8)};

  mln::box2d domain = {{0,0}, {3, 4}};
  auto ima = mln::image2d_planar<mln::rgb8>::from_buffer(buffers, domain, strides);
  mln::io::imprint(ima);

  //FIXME: real test
}

TEST(UTImage2DPlanar, channel)
{
  mln::uint8 bufferR[12];
  mln::uint8 bufferG[12];
  mln::uint8 bufferB[12];
  std::iota(bufferR, bufferR + 12, 10);
  std::iota(bufferG, bufferG + 12, 20);
  std::iota(bufferB, bufferB + 12, 30);

  void* buffers[3] = {static_cast<void*>(bufferR),
                     static_cast<void*>(bufferG),
                     static_cast<void*>(bufferB)};

  mln::box2d domain = {{0,0}, {3, 4}};
  auto ima = mln::image2d_planar<mln::rgb8>::from_buffer(buffers, domain);


  auto green = ima.channel(1);
  mln::io::imprint(green);
}

