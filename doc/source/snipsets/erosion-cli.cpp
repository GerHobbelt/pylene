#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/morpho/structural/erode.hpp>
#include <mln/morpho/structural/dilate.hpp>
#include <mln/morpho/structural/opening.hpp>
#include <mln/morpho/structural/closing.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <cctype>
#include <string>
#include <algorithm>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

enum se_type
{
  kSquare,
  kDisc,
  kDiamond
};


std::istream& operator>> (std::istream& in, se_type& se)
{
  std::string token;
  in >> token;
  std::transform(token.begin(), token.end(), token.begin(), static_cast<int(*)(int)>(std::tolower));
  if (token == "square")
    se = kSquare;
  else if (token == "disc")
    se = kDisc;
  else if (token == "diamond")
    se = kDiamond;
  else
    throw po::invalid_option_value ("Invalid SE");
  return in;
}

enum morpho_op_type
{
  kErosion,
  kDilation,
  kOpening,
  kClosing
};

std::istream& operator>> (std::istream& in, morpho_op_type& se)
{
  std::string token;
  in >> token;
  std::transform(token.begin(), token.end(), token.begin(), static_cast<int(*)(int)>(std::tolower));
  if (token == "erosion")
    se = kErosion;
  else if (token == "dilation")
    se = kDilation;
  else if (token == "opening")
    se = kOpening;
  else if (token == "closing")
    se = kClosing;
  else
    throw po::invalid_option_value ("Invalid Operator");
  return in;
}


struct exec_params_type
{
  se_type se;
  morpho_op_type op;
  int size;
};


int main(int argc, char** argv)
{
  po::options_description desc("Allowed options");
  po::positional_options_description p;
  p.add("operator", 1)
    .add("se", 1)
    .add("size", 1)
    .add("input", 1)
    .add("output", 1);

  desc.add_options()
    ("help", "produce help message")
    ("operator", po::value<morpho_op_type>()->required(), "")
    ("se", po::value<se_type>()->required(), "")
    ("size", po::value<int>()->required(), "Size of the SE.")
    ("input", po::value<std::string>()->required(), "Input image (8u or rgb8)")
    ("output", po::value<std::string>()->required(), "Output image (8u or rgb8)")
    ;

  po::variables_map vm;
  int size;
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
    std::cout << "Usage: " << argv[0] << " [-h,--help] OPERATOR SE size input output\n"
      "OPERATOR\t Morphological operation to perform [erosion | dilation]\n"
      "SE\t Structuring element to use [square | disc | diamond]\n"
      "size\t Size of the SE\n"
      "input\t Input image (u8)\n"
      "output\t Output image (u8)\n";
    return 1;
  }

  mln::image2d<mln::uint8> input, output;
  mln::io::imread(vm["input"].as<std::string>(), input);



  auto nbh = mln::make_rectangle2d(size, size);

  switch (vm["operator"].as<morpho_op_type>())
  {
  case kErosion:
    output = mln::morpho::structural::erode(input, nbh);
    break;
  case kDilation:
    output = mln::morpho::structural::dilate(input, nbh);
    break;
  case kOpening:
    output = mln::morpho::structural::opening(input, nbh);
    break;
  case kClosing:
    output = mln::morpho::structural::closing(input, nbh);
    break;
  }

  mln::io::imsave(output, vm["output"].as<std::string>());
}
