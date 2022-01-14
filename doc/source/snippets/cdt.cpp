#include <mln/core/image/ndimage.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/data/stretch.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/chamfer_distance_transform.hpp>


#include <cstdint>




mln::rgb8 heat_lut(float x)
{
  assert(0 <= x && x <= 1);
  float x0 = 1.f / 4.f;
  float x1 = 2.f / 4.f;
  float x2 = 3.f / 4.f;

  if (x < x0)
  {
    auto g = static_cast<uint8_t>(x / x0 * 255);
    return mln::rgb8{0, g, 255};
  }
  else if (x < x1)
  {
    auto b = static_cast<uint8_t>((x1 - x) / x0 * 255);
    return mln::rgb8{0, 255, b};
  }
  else if (x < x2)
  {
    auto r = static_cast<uint8_t>((x - x1) / x0 * 255);
    return mln::rgb8{r, 255, 0};
  }
  else
  {
    auto b = static_cast<uint8_t>((1.f - x) / x0 * 255);
    return mln::rgb8{255, b, 0};
  }
}

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cout << "Usage:" << argv[0] << " distance_type input output\n"
      "chamfer distance transform with disttype:\n"
      "1: 4-connexion\n"
      "2: 8-connexion\n"
      "3: 2-3 weights\n"
      "4: 5-7-11 weights\n";
    std::abort();
  }

  mln::image2d<bool> input;
  mln::io::imread(argv[2], input);

  // BEGIN
  // (1) Compute the distance transform
  mln::se::wmask2d<int> weights;

  switch (std::atoi(argv[1]))
  {
  case 1:
    weights = mln::se::wmask2d({{0, 1, 0}, //
                                              {1, 0, 1},
                                              {0, 1, 0}});
    break;
  case 2:
    weights = mln::se::wmask2d({{1, 1, 1}, //
                                              {1, 0, 1},
                                              {1, 1, 1}});
    break;
  case 3:
    weights = mln::se::wmask2d({{3, 2, 3}, //
                                              {2, 0, 2},
                                              {3, 2, 3}});
    break;
  case 4:
    weights = mln::se::wmask2d({{+0, 11, +0, 11, +0}, //
                                              {11, +7, +5, +7, 11},
                                              {+0, +5, +0, +5, +0},
                                              {11, +7, +5, +7, 11},
                                              {+0, 11, +0, 11, +0}});
    break;
  default:
    std::cerr << "Bad type.\n";
    std::abort();
  }


  auto d = mln::labeling::chamfer_distance_transform<int16_t>(input, weights, true);
  auto d_stretched = mln::data::stretch<float>(d);
  mln::io::imsave(mln::transform(d_stretched, heat_lut), argv[3]);
}
