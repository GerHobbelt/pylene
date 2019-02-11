#include <mln/contrib/meanshift/meanshift.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/core/colors.hpp>

int main(int argc, const char** argv)
{
  if (argc < 5)
  {
    std::cerr << "Usage: " << argv[0] << " input.ppm hs hr output.hpp\n";
    std::exit(1);
  }



  mln::image2d<mln::rgb8> f;

  mln::io::imread(argv[1], f);

  float hs  = std::atof(argv[2]);
  float hr  = std::atof(argv[3]);
  auto  out = mln::contrib::meanshift(f, hs, hr);

  mln::io::imsave(out, argv[4]);
}
