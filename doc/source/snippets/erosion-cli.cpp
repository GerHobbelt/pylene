#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/se/rect2d.hpp>

#include <mln/morpho/experimental/closing.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/experimental/opening.hpp>
#include <mln/morpho/experimental/median_filter.hpp>

#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>


#include <algorithm>
#include <boost/program_options.hpp>
#include <cctype>
#include <iostream>
#include <string>

namespace po = boost::program_options;

enum se_type
{
  kSquare,
  kDisc,
  kDiamond
};


int tolower_safe(int c)
{
  return std::tolower(static_cast<unsigned char>(c));
}

std::istream& operator>>(std::istream& in, se_type& se)
{
  std::string token;
  in >> token;
  std::transform(token.begin(), token.end(), token.begin(), tolower_safe);
  if (token == "square")
    se = kSquare;
  else if (token == "disc")
    se = kDisc;
  else if (token == "diamond")
    se = kDiamond;
  else
    throw po::invalid_option_value("Invalid SE");
  return in;
}

enum morpho_op_type
{
  kErosion,
  kDilation,
  kOpening,
  kClosing,
  kMedian,
};

std::istream& operator>>(std::istream& in, morpho_op_type& se)
{
  std::string token;
  in >> token;
  std::transform(token.begin(), token.end(), token.begin(), tolower_safe);
  if (token == "erosion")
    se = kErosion;
  else if (token == "dilation")
    se = kDilation;
  else if (token == "opening")
    se = kOpening;
  else if (token == "closing")
    se = kClosing;
  else if (token == "median")
    se = kMedian;
  else
    throw po::invalid_option_value("Invalid Operator");
  return in;
}


struct exec_params_type
{
  se_type        se;
  morpho_op_type op;
  int            size;
};


int main(int argc, char** argv)
{
  po::options_description            desc("Allowed options");
  po::positional_options_description p;
  p.add("operator", 1).add("se", 1).add("size", 1).add("input", 1).add("output", 1);

  desc.add_options()("help", "produce help message")("operator", po::value<morpho_op_type>()->required(), "")(
      "se", po::value<se_type>()->required(), "")("size", po::value<int>()->required(), "Size of the SE.")(
      "input", po::value<std::string>()->required(),
      "Input image (8u or rgb8)")("output", po::value<std::string>()->required(), "Output image (8u or rgb8)");

  po::variables_map vm;
  int               size;
  try
  {
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);
    size = vm["size"].as<int>();
    if (size < 1 || size % 2 == 0)
    {
      std::cerr << "Size must be positive and odd.\n";
      return 1;
    }
  }
  catch (...)
  {
    std::cout << "Usage: " << argv[0]
              << " [-h,--help] OPERATOR SE size input output\n"
                 "OPERATOR\t Morphological operation to perform [erosion | dilation]\n"
                 "SE\t Structuring element to use [square | disc | diamond]\n"
                 "size\t Size of the SE\n"
                 "input\t Input image (u8)\n"
                 "output\t Output image (u8)\n";
    return 1;
  }

  mln::experimental::image2d<mln::uint8> input, output;
  mln::io::experimental::imread(vm["input"].as<std::string>(), input);


  mln::experimental::se::rect2d nbh(size, size);

  switch (vm["operator"].as<morpho_op_type>())
  {
  case kErosion:
    output = mln::morpho::experimental::erosion(input, nbh);
    break;
  case kDilation:
    output = mln::morpho::experimental::dilation(input, nbh);
    break;
  case kOpening:
    output = mln::morpho::experimental::opening(input, nbh);
    break;
  case kClosing:
    output = mln::morpho::experimental::closing(input, nbh);
    break;
  case kMedian:
    output = mln::morpho::experimental::median_filter(input, nbh, mln::extension::bm::mirror{});
    break;
  }

  mln::io::experimental::imsave(output, vm["output"].as<std::string>());
}
