#pragma once

#include <mln/core/box.hpp>

namespace mln
{
  struct ParallelCanvas2d
  {
    static constexpr int TILE_WIDTH  = 128;
    static constexpr int TILE_HEIGHT = 128;

    virtual mln::experimental::box2d GetDomain() const = 0;
    virtual void ExecuteTile(mln::experimental::box2d b) const = 0;
  };

  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void parallel_execute2d(ParallelCanvas2d&);
} // namespace mln
