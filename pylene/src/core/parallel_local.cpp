#include <mln/core/canvas/parallel_local.hpp>

#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>


namespace mln
{
  class ParallelLocalCanvas2DImpl
  {
  public:
    ParallelLocalCanvas2DImpl(const SimpleFilter2D& delegate)
      : m_delegate{delegate}
    {
    }

    void operator()(const tbb::blocked_range2d<int>&) const;


  private:
    SimpleFilter2D m_delegate;
};

  void ParallelLocalCanvas2DImpl::operator()(const tbb::blocked_range2d<int>& tile) const
  {
    int x = tile.cols().begin();
    int w = tile.cols().end() - tile.cols().begin();
    int y = tile.rows().begin();
    int h = tile.rows().end() - tile.rows().begin();

    mln::box2d roi(x, y, w, h);

    m_delegate.Execute(roi);
  }


  void SimpleFilter2D::SetTileSize(int width, int height) noexcept
  {
    mln::box2d roi = {width, height};
    mln::box2d iroi = m_executor->ComputeInputRegion(roi);
    m_tile_out = m_tile_create(width, height);
    m_tile_in  = m_tile_create(iroi.width(), iroi.height());

    if (m_executor->NeedTranspose())
    {
      m_aux_in  = m_tile_create(iroi.height(), iroi.width());
      m_aux_out = m_tile_create(height, width);
    }
  }

  SimpleFilter2D::SimpleFilter2D(const SimpleFilter2D& other) noexcept
    : m_load{other.m_load}
    , m_store{other.m_store}
    , m_tile_create{other.m_tile_create}
    , m_transpose{other.m_transpose}
    , m_executor{other.m_executor}
  {
    m_tile_out = m_tile_create(other.m_tile_out.width(), other.m_tile_out.height());
    m_tile_in  = m_tile_create(other.m_tile_in.width(), other.m_tile_in.height());

    if (m_executor->NeedTranspose())
    {
      m_aux_out = m_tile_create(other.m_aux_out.width(), other.m_aux_out.height());
      m_aux_in  = m_tile_create(other.m_aux_in.width(), other.m_aux_in.height());
    }
  }

  SimpleFilter2D& SimpleFilter2D::operator= (const SimpleFilter2D& other) noexcept
  {
    m_load        = other.m_load;
    m_store       = other.m_store;
    m_tile_create = other.m_tile_create;
    m_transpose   = other.m_transpose;
    m_executor    = other.m_executor;
    if (m_executor->NeedTranspose())
    {
      m_aux_out = m_tile_create(other.m_aux_out.width(), other.m_aux_out.height());
      m_aux_in  = m_tile_create(other.m_aux_in.width(), other.m_aux_in.height());
    }
    return *this;
  }


  void SimpleFilter2D::Execute(mln::box2d roi) const
  {
    auto iroi = m_executor->ComputeInputRegion(roi);

    auto in = m_tile_in.clip(iroi.width(), iroi.height());
    auto out = m_tile_out.clip(roi.width(), roi.height());

    // Laod the tile from the main image
    m_load(iroi.tl(), in);

    bool need_transpose = m_executor->NeedTranspose();

    mln::bp::Tile2DView<void> in_1  = (!need_transpose) ? in : m_aux_in.clip(in.height(), in.width());
    mln::bp::Tile2DView<void> out_1 = (!need_transpose) ? out : m_aux_out.clip(out.height(), out.width());

    mln::point2d anchor_in  = (!need_transpose) ? iroi.tl() : mln::point2d{iroi.y(), iroi.x()};
    mln::point2d anchor_out = (!need_transpose) ? roi.tl() : mln::point2d{roi.y(), roi.x()};

    if (need_transpose)
      m_transpose(in, in_1);

    // Execute on the tile
    m_executor->Execute(in_1, anchor_in, out_1, anchor_out);

    if (m_executor->NeedTranspose())
      m_transpose(out_1, out);

    // Write back the tile in the main image
    m_store(roi.tl(), out);
  }

  /*
  ** Caller for the TBB parallel_for
  ** Rationale being that every pointwise algorithm applies a function to every pixel,
  ** and each algorithm can take input image(s) as well as an output image, hence the variadInputImage
  ** We create a wrapper class to circumvent TBB not allowing abstract classes as parallel_for body
  */
  void LocalFilter2D<void>::execute_parallel(mln::box2d roi, int tile_width, int tile_height)
  {
    this->m_filter.SetTileSize(tile_width, tile_height);

    ParallelLocalCanvas2DImpl wrapper(m_filter);
    tbb::blocked_range2d<int> rng(roi.y(), roi.y() + roi.height(), tile_height, //
                                  roi.x(), roi.x() + roi.width(), tile_width);
    tbb::parallel_for(rng, wrapper, tbb::simple_partitioner());
  }


  void LocalFilter2D<void>::execute_sequential(mln::box2d roi, int tile_width, int tile_height)
  {
    const int x1 = roi.br().x();
    const int y1 = roi.br().y();

    this->m_filter.SetTileSize(tile_width, tile_height);

    for (int y = roi.y(); y < y1; y += tile_height)
      for (int x = roi.x(); x < x1; x += tile_width)
      {
        int w = std::min(tile_width, x1 - x);
        int h = std::min(tile_height, y1 - y);
        this->m_filter.Execute({x, y, w, h});
      }
  }

  void LocalFilter2D<void>::execute(mln::box2d roi, int tile_width, int tile_height, bool parallel)
  {
    if (parallel)
      this->execute_parallel(roi, tile_width, tile_height);
    else
      this->execute_sequential(roi, tile_width, tile_height);
  }

  void LocalChainFilter2D<void>::execute(mln::box2d roi, int tile_width, int tile_height, bool parallel)
  {
    int n = m_filters.size();
    if (n < 2)
      throw std::runtime_error("The number of filters in the chain must be >= 2.");

    std::vector<mln::box2d> regions(n);
    for (int i = n - 1; i >= 0; i--)
      regions[i] = std::exchange(roi, m_filters[i]->ComputeInputRegion(roi));


    // Allocate temporary images
    mln::ndbuffer_image tmp_1, tmp_2;
    if (n >= 2)
      tmp_1 = this->create_tmp_image(regions[0]);

    if (n > 2)
      tmp_2 = this->create_tmp_image(regions[1]);

    // Declare swap buffers
    mln::ndbuffer_image* tmp_in  = &tmp_2;
    mln::ndbuffer_image* tmp_out = &tmp_1;

    // First filter
    {
      auto* f = this->get_first_filter();
      f->set_output(tmp_out);
      f->set_executor(m_filters[0].get());
      f->execute(regions[0], tile_width, tile_height, parallel);
    }

    // Intermediate filters
    {
      for (int i = 1; i < n - 1; ++i)
      {
        auto* f =  this->get_middle_filter();
        std::swap(tmp_in, tmp_out);
        tmp_out->clip(regions[i]);
        f->set_input(tmp_in);
        f->set_output(tmp_out);
        f->set_executor(m_filters[i].get());
        f->execute(regions[i], tile_width, tile_height, parallel);
      }
    }
    // Last filter
    {
      auto* f = this->get_last_filter();
      f->set_input(tmp_out);
      f->set_executor(m_filters[n-1].get());
      f->execute(regions[n-1], tile_width, tile_height, parallel);
    }
  }

} // namespace mln
