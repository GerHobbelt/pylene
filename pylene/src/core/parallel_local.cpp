#include <mln/core/algorithm/parallel_local.hpp>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>

namespace mln
{
  void ParallelLocal2d::operator()(const tbb::blocked_range2d<int>& tile) const
  {
    int x = tile.cols().begin();
    int w = tile.cols().end() - tile.cols().begin();
    int y = tile.rows().begin();
    int h = tile.rows().end() - tile.rows().begin();

    mln::experimental::box2d domain(x, y, w, h);
    this->ExecuteTile(domain);
  }


  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void parallel_execute2d(ParallelLocal2d& canvas)
  {
    struct ParallelForWrapper
    {
      ParallelLocal2d* canvas;
      ParallelForWrapper(ParallelLocal2d* ptr)
        : canvas{ptr}
      {}

      void operator()(const tbb::blocked_range2d<int>& tile) const
      {
        std::invoke(*canvas, tile);
      }
    };

    ParallelForWrapper wrapper(&canvas);
    mln::experimental::box2d domain = wrapper.canvas->GetDomain();
    tbb::parallel_for(tbb::blocked_range2d<int>(domain.y(), domain.y() + domain.height(), wrapper.canvas->TILE_HEIGHT, //
                                                domain.x(), domain.x() + domain.width(), wrapper.canvas->TILE_WIDTH),
                      wrapper);
  }
} // namespace mln
