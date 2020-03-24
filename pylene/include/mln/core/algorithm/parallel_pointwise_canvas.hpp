#pragma once

#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "mln/core/box.hpp"
#include "mln/core/concept/object.hpp"
#include <mln/core/image/image.hpp>
#include <range/v3/functional/concepts.hpp>
#include <mln/core/algorithm/for_each.hpp>

namespace mln
{

  class ParallelCanvas2d
  {
    constexpr int TILE_WIDTH  = 128;
    constexpr int TILE_HEIGHT = 128;


    void operator()(const tbb::blocked_range2d<int>& tile) const
    {
      int x = tile.cols().begin();
      int w = tile.cols().end() - tile.cols().begin();
      int y = tile.rows().begin();
      int h = tile.rows().end() - tile.rows().begin();

      mln::experimental::box2d domain(x, y, w, h);
      this->ExecuteTile(domain);
    }

    virtual mln::experimental::box2d GetDomain() const = 0;
    virtual ExecuteTile(mln::experimental::box2d b) const = 0;
  };


  template <class Function, class InputImage>
  class ForEachPointwise
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::invocable<Function, image_reference_t<InputImage>>);

    ApplyPointwise(InputImage input, Function fun);

    mln::experimental::box2d GetDomain() const final { return input.domain(); }

    void ExecuteTile(mln::experimental::box2d b) const final
    {
      auto subimage = input.clip(domain);
      mln::for_each(subimage, fun);
    }
  };



  template <class Function, class InputImage, class O>
  class TransformPointwise
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::invocable<Function, image_reference_t<InputImage>>);

    ApplyPointwise(InputImage input, Function fun);

    mln::experimental::box2d GetDomain() const final { return input.domain(); }

    void ExecuteTile(mln::experimental::box2d b) const final
    {
      auto subimage_in = input.clip(domain);
      auto subimage_out = input.clip(domain);
      mln::transform(subimage, fun);
    }
  };


  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  */
  void parallel_execute2d(ParallelCanvas2d* canvas)
  {
    mln::experimental::box2d domain = canvas->GetDomain();
    tbb::parallel_for(tbb::blocked_range2d<int>(domain.y(), domain.y() + domain.height(), canvas->TILE_HEIGHT, //
                                                domain.x(), domain.x() + domain.width(), canvas->TILE_WIDTH),
                      *canvas);
  }


} // namespace mln
