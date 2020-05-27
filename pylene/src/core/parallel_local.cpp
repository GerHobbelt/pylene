#include <mln/core/canvas/parallel_local.hpp>

#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

namespace mln
{
  class ParallelLocalCanvas2DImpl
  {
  public:
    ParallelLocalCanvas2DImpl(ParallelLocalCanvas2D* delegate)
      : m_delegate{delegate}
    {
    }

    mln::box2d GetDomain() const;
    void operator()(const tbb::blocked_range2d<int>&) const;

    ParallelLocalCanvas2D* delegate() const { return m_delegate; }

  private:
    ParallelLocalCanvas2D* m_delegate = nullptr;
  };

  void ParallelLocalCanvas2DImpl::operator()(const tbb::blocked_range2d<int>& tile) const
  {
    int x = tile.cols().begin();
    int w = tile.cols().end() - tile.cols().begin();
    int y = tile.rows().begin();
    int h = tile.rows().end() - tile.rows().begin();

    mln::box2d domain(x, y, w, h);
    m_delegate->ExecuteTile(domain);
  }

  mln::box2d ParallelLocalCanvas2DImpl::GetDomain() const
  {
    return m_delegate->GetDomain();
  }

  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void parallel_execute_local2D(ParallelLocalCanvas2D& canvas)
  {
    tbb::task_scheduler_init init;
    ParallelLocalCanvas2DImpl wrapper(&canvas);
    mln::box2d domain = wrapper.GetDomain();

    tbb::blocked_range2d<int> rng(domain.y(), domain.y() + domain.height(), wrapper.delegate()->TILE_HEIGHT, //
                                  domain.x(), domain.x() + domain.width(), wrapper.delegate()->TILE_WIDTH);
    tbb::parallel_for(rng, wrapper);
  }
  
} // namespace mln
