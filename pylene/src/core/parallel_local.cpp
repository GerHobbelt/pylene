#include <mln/core/canvas/parallel_local.hpp>

#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

#include <fmt/core.h>
//#include <mln/io/imprint.hpp>

namespace mln
{
  class ParallelLocalCanvas2DImpl
  {
  public:
    ParallelLocalCanvas2DImpl(ParallelLocalCanvas2DBase* delegate)
    {
      m_delegate = delegate->clone();
    }

    ParallelLocalCanvas2DImpl(const ParallelLocalCanvas2DImpl& other)
    {
      m_delegate = other.m_delegate->clone();
    }

    void operator()(const tbb::blocked_range2d<int>&) const;

  private:
    std::unique_ptr<ParallelLocalCanvas2DBase> m_delegate = nullptr;
  };

  void ParallelLocalCanvas2DImpl::operator()(const tbb::blocked_range2d<int>& tile) const
  {
    int x = tile.cols().begin();
    int w = tile.cols().end() - tile.cols().begin();
    int y = tile.rows().begin();
    int h = tile.rows().end() - tile.rows().begin();

    mln::box2d roi(x, y, w, h);

    //fmt::print("== Processing region=(x={},y={},w={},h={})==\n", roi.x(), roi.y(), roi.width(), roi.height());
    m_delegate->ExecuteTile(roi);
  }


  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void ParallelLocalCanvas2DBase::execute_parallel(mln::box2d roi, int tile_width, int tile_height)
  {
    ParallelLocalCanvas2DImpl wrapper(this);

    tbb::blocked_range2d<int> rng(roi.y(), roi.y() + roi.height(), tile_height, //
                                  roi.x(), roi.x() + roi.width(), tile_width);
    tbb::parallel_for(rng, wrapper, tbb::simple_partitioner());
  }


  void ParallelLocalCanvas2DBase::execute_sequential(mln::box2d roi, int tile_width, int tile_height)
  {
    const int x1 = roi.br().x();
    const int y1 = roi.br().y();


    for (int y = roi.y(); y < y1; y += tile_height)
      for (int x = roi.x(); x < x1; x += tile_width)
      {
        int w = std::min(tile_width, x1 - x);
        int h = std::min(tile_height, y1 - y);
        this->ExecuteTile({x, y, w, h});
      }
  }

  void ParallelLocalCanvas2DBase::execute(mln::box2d roi, int tile_width, int tile_height)
  {
    bool parallel = true;
    if (parallel)
      this->execute_parallel(roi, tile_width, tile_height);
    else
      this->execute_sequential(roi, tile_width, tile_height);
  }


  void ParallelLocalCanvas2D::ExecuteTile(mln::box2d roi) const
  {
    mln::box2d input_roi = this->ComputeInputRegion(roi);
    //fmt::print("Loading tile=(x={},y={},w={},h={})\n", input_roi.x(), input_roi.y(), input_roi.width(), input_roi.height());
    //fmt::print("Writing tile=(x={},y={},w={},h={})\n", roi.x(), roi.y(), roi.width(), roi.height());


    auto m_tile_l = this->GetTileLoader();
    auto m_tile_w = this->GetTileWriter();
    auto m_tile_e = this->GetTileExecutor();

    m_tile_l->load_tile(roi, input_roi);
    m_tile_e->execute(m_tile_l->get_tile(), m_tile_w->get_tile(roi));
    m_tile_w->write_tile(roi);
  }

} // namespace mln
