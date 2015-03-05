#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/se/ball.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/structural/gradient.hpp>
#include <mln/colors/literal.hpp>

#include <apps/tos/Kinterpolate.hpp>


int main(int argc, char** argv)
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << "detection.png original.png output.png\n"
      "Dessine les contours de la détection en superimosition de l'image d'origine.\n";
    std::exit(1);
  }

  using namespace mln;

  image2d<rgb8> f;
  image2d<rgb8> ori;

  io::imread(argv[1], f);
  io::imread(argv[2], ori);

  f = Kadjust_to(f, ori.domain());

  image2d<bool> mask = eval(red(f) > 127);

  auto ball = se::make_ball2d(10);
  auto m2 = morpho::structural::external_gradient(mask, ball);
  fill(ori | m2, colors::literal::red);
  io::imsave(ori, argv[3]);
}
