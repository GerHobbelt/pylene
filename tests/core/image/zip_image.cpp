#include <mln/core/image/zip_image.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/io/imprint.hpp>

#include <boost/tuple/tuple_io.hpp>


int main()
{
  using namespace mln;


  image2d<int> ima(5,5);
  image2d<uint16> ima2(5,5);

  iota(ima2, 4);
  iota(ima, 3);


  uint16 x;
  int y;
  for (auto w:imzip(ima, ima2).values() ) {
    std::cout << w << std::endl;
    boost::tie(x, y) = w;
    std::cout << ((int) x++) << "," << y++ << std::endl;
    boost::get<0>(w) = x++;
    boost::get<1>(w) = y++;
  }

  for (auto w : imzip(ima, ima2).pixels()) {
    boost::tie(x, y) = w.val();
    std::cout << ((int) x++) << "," << y++ << std::endl;
    boost::get<0>(w.val()) = x++;
    boost::get<1>(w.val()) = y++;
  }


  io::imprint(imzip(ima, ima2));

}

