#include <mln/core/image/ndimage.hpp>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <mln/morpho/closing.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>
#include <mln/morpho/gradient.hpp>
#include <mln/morpho/median_filter.hpp>
#include <mln/morpho/opening.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

enum class morpho_op_type
{
  kErosion = 0,
  kDilation,
  kOpening,
  kClosing,
  kMedian,
  kGradient,
  kInternalGradient,
  kExternalGradient
};

enum class se_type
{
  kSquare = 0,
  kDisc
};

morpho_op_type get_morpho_op(char* arg)
{
  const std::string str(arg);
  if (str == "erosion")
    return morpho_op_type::kErosion;
  else if (str == "dilation")
    return morpho_op_type::kDilation;
  else if (str == "opening")
    return morpho_op_type::kOpening;
  else if (str == "closing")
    return morpho_op_type::kClosing;
  else if (str == "median")
    return morpho_op_type::kMedian;
  else if (str == "gradient")
    return morpho_op_type::kGradient;
  else if (str == "int_gradient")
    return morpho_op_type::kInternalGradient;
  else if (str == "ext_gradient")
    return morpho_op_type::kExternalGradient;
  else
    throw std::invalid_argument("Invalid input morphological operation");
}

se_type get_se(char* arg)
{
  const std::string str(arg);
  if (str == "square")
    return se_type::kSquare;
  else if (str == "disk")
    return se_type::kDisc;
  else
    throw std::invalid_argument("Invalid input structuring element");
}

template <typename SE>
mln::image2d<std::uint8_t> process(mln::image2d<std::uint8_t> img, morpho_op_type op, SE se)
{
  using namespace mln::morpho;
  switch (op)
  {
  case morpho_op_type::kErosion:
    return erosion(img, se);
    break;
  case morpho_op_type::kDilation:
    return dilation(img, se);
    break;
  case morpho_op_type::kOpening:
    return opening(img, se);
    break;
  case morpho_op_type::kClosing:
    return closing(img, se);
    break;
  case morpho_op_type::kMedian:
    return median_filter(img, se, mln::extension::bm::mirror{});
    break;
  case morpho_op_type::kGradient:
    return gradient(img, se);
    break;
  case morpho_op_type::kInternalGradient:
    return internal_gradient(img, se);
    break;
  case morpho_op_type::kExternalGradient:
    return external_gradient(img, se);
    break;
  }
  return mln::image2d<std::uint8_t>();
}

int main(int argc, char* argv[])
{
  using namespace mln;

  if (argc != 6)
  {
    std::cout << "Usage: " << argv[0]
              << " OPERATOR SE size input output\n"
                 "OPERATOR\t Morphological operation to perform [erosion | dilation | opening | closing | median | "
                 "gradient | ext_gradient | int_gradient]\n"
                 "SE\t Structuring element to use [square | disc]\n"
                 "size\t Size of the SE\n"
                 "input\t Input image (u8)\n"
                 "output\t Output image (u8)\n";
    return 1;
  }

  const auto input_op = get_morpho_op(argv[1]);
  const auto input_se = get_se(argv[2]);
  const auto size     = std::atoi(argv[3]);

  mln::image2d<std::uint8_t> img;
  mln::io::imread(argv[4], img);

  if (size < 1 || size % 2 == 0)
    throw std::invalid_argument("Structuring element size must be positive and odd");

  mln::image2d<std::uint8_t> out;
  if (input_se == se_type::kDisc)
    out = process(img, input_op, se::disc(size));
  else
    out = process(img, input_op, se::rect2d(size, size));

  mln::io::imsave(out, argv[5]);
  return 0;
}