#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/colors/lsh.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>



int main(int argc, const char** argv)
{
  using namespace mln;

  std::string filename = argv[1];


  image2d<rgb8> ima;
  io::imread(filename, ima);

  image2d<lsh8> f = transform(ima, rgb2lsh<uint8> );

  image2d<uint8> mask = transform(f, [] (lsh8 x) { return (uint8) ((x[1] < 45) * 255); });


  io::imsave(mask, "mask.tiff");
  io::imsave(transform(f, [] (lsh8 x) -> uint8 { return x[1] < 45 ? x[0] : 0; }), "lum.tiff");
  io::imsave(transform(f, [] (lsh8 x) -> uint8 { return x[1] > 45 ? x[2] : 0; }), "hue.tiff");

  // Compute TOS on cc[L]
  {
    auto L = transform(f, [] (lsh8 x) -> uint8 { return x[0]; });

    image2d<uint8> lbl;
    unsigned nlabel;
    std::tie(lbl, nlabel) = labeling::blobs<uint8>(L);

    std::vector< std::pair< FIXME, FIXME >

    for (int i = 1; i <= nlabel; ++i)
      {
	auto x = (lbl == i);
	auto subima = (L | rng::filter(ima.domain(), [x](point2d p) { return x(p); }));
	auto ima2 = addborder(subima);
	morpho::tos(ima2)
      }

  }

}
