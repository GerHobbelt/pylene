#include <mln/core/canvas/parallel_pointwise.hpp>

#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

namespace mln
{
  class ParallelCanvas2dImpl
  {
  public:
    ParallelCanvas2dImpl(ParallelCanvas2d* delegate)
      : m_delegate{delegate}
    {
    }

    mln::experimental::box2d GetDomain() const;
    void operator()(const tbb::blocked_range2d<int>&) const;

    ParallelCanvas2d* delegate() const { return m_delegate; }

  private:
    ParallelCanvas2d* m_delegate = nullptr;
  };

  void ParallelCanvas2dImpl::operator()(const tbb::blocked_range2d<int>& tile) const
  {
    int x = tile.cols().begin();
    int w = tile.cols().end() - tile.cols().begin();
    int y = tile.rows().begin();
    int h = tile.rows().end() - tile.rows().begin();

    mln::experimental::box2d domain(x, y, w, h);
    m_delegate->ExecuteTile(domain);
  }

  mln::experimental::box2d ParallelCanvas2dImpl::GetDomain() const
  {
    return m_delegate->GetDomain();
  }


  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void parallel_execute2d(ParallelCanvas2d& canvas)
  {
    ParallelCanvas2dImpl wrapper(&canvas);
    mln::experimental::box2d domain = wrapper.GetDomain();

    tbb::blocked_range2d<int> rng(domain.y(), domain.y() + domain.height(), wrapper.delegate()->TILE_HEIGHT, //
                                  domain.x(), domain.x() + domain.width(), wrapper.delegate()->TILE_WIDTH);
    tbb::parallel_for(rng, wrapper);
  }
} // namespace mln
