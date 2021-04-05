//
// Created by alexandre on 05/04/2021.
//

#include <iostream>
#include "mln/contrib/segdet/linearregression.hpp"
#include "mln/contrib/segdet/segdet.hpp"

int main()
{
  using namespace mln::contrib::segdet;

  mln::image2d<uint8_t> input;
  mln::io::imread("./complex_straightpre_processed.png", input);

  auto output = detect_line(input, 3, 0);

  printf("lol");
  (void) output;
  return 0;
}
