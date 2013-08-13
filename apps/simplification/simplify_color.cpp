#include <iostream>


#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/colors.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>


#include <apps/tos/colorToSGrad.hpp>
#include <apps/tos/addborder.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/set_mean_on_nodes.hpp>
#include "simplify.hpp"


void usage(char** argv)
{
  std::cout << "Usage: " << argv[0] << " input(color) lambda output.tiff [grainsize [areafactor]]" << std::endl
	    << "Perform a simplification of the ToS by removing overlapping"
	    << "level lines in radius of lambda pixels" << std::endl
	    << "Param:" << std::endl
	    << "lambda: size of the dilation" << std::endl
    //      << "grainsize: minimal grain size (default: 0)" << std::endl
    //	    << "areafactor: avoid sur-simplification by limiting node desactivation (default: 0.0)" << std::endl
    //	    << "\t a shape S1 can only be desactived by a shape S2 if area(S1) * areafactor < area(S2)"
	    << std::endl
    ;
  std::terminate();
}

int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 4)
    usage(argv);

  int lambda = std::atoi(argv[2]);
  int grainsize = argc >= 5 ? std::atoi(argv[4]) : 0;
  float areafactor = argc >= 6 ? std::atof(argv[5]) : 0.0;

  image2d<rgb8> ima_, ima;
  io::imread(argv[1], ima_);


  image2d<unsigned> K;
  image2d<unsigned> parent;
  std::vector<unsigned> S;
  colorToSGrad(ima_, K, parent, S);


  ima = interpolate_k1(addborder(ima_, lexicographicalorder_less<rgb8>()));
  image2d<rgb8>  mean = set_mean_on_node2(immerse_k1(ima), K, S, parent, K1::is_face_2);
  image2d<rgb8> simp = simplify_top_down(unimmerse_k1(mean), K, parent, S, lambda); //, grainsize, areafactor);

  io::imsave(simp, argv[3]);
}
