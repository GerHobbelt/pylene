#include <mln/core/canvas/parallel_local.hpp>

#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

namespace mln
{
  class ParallelLocalCanvas2DImpl
  {
  public:
    ParallelLocalCanvas2DImpl(ParallelLocalCanvas2D* delegate)
    {
      m_delegate = delegate->clone();
    }

    ParallelLocalCanvas2DImpl(const ParallelLocalCanvas2DImpl& other)
    {
      m_delegate = other.m_delegate->clone();
    }


    mln::box2d GetOutputRegion() const;
    void operator()(const tbb::blocked_range2d<int>&) const;

    ParallelLocalCanvas2D* delegate() const { return m_delegate.get(); }

  private:
    std::unique_ptr<ParallelLocalCanvas2D> m_delegate = nullptr;
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

  mln::box2d ParallelLocalCanvas2DImpl::GetOutputRegion() const
  {
    return m_delegate->GetOutputRegion();
  }

  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void parallel_execute_local2D(ParallelLocalCanvas2D& canvas)
  {
    ParallelLocalCanvas2DImpl wrapper(&canvas);
    mln::box2d domain = wrapper.GetOutputRegion();

    tbb::blocked_range2d<int> rng(domain.y(), domain.y() + domain.height(), wrapper.delegate()->TILE_HEIGHT, //
                                  domain.x(), domain.x() + domain.width(), wrapper.delegate()->TILE_WIDTH);
    tbb::parallel_for(rng, wrapper, tbb::simple_partitioner());
  }

  void ParallelLocalCanvas2D::ExecuteTile(mln::box2d roi) const
  {
    mln::box2d input_roi = this->ComputeInputRegion(roi);

    auto m_tile_l = this->GetTileLoader();
    auto m_tile_w = this->GetTileWriter();
    auto m_tile_e = this->GetTileExecutor();

    m_tile_l->load_tile(roi, input_roi);
    m_tile_e->execute(m_tile_l->get_tile(), m_tile_w->get_tile(roi));
    m_tile_w->write_tile(roi);
  }

} // namespace mln
