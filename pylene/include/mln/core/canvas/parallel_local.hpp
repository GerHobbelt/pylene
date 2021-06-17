#pragma once

#include <mln/core/algorithm/paste.hpp>
#include <mln/core/box.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/value/value_traits.hpp>

namespace mln
{



  class TileExecutorBase
  {
  public:

    virtual ~TileExecutorBase();
    virtual void                            Execute(mln::bp::Tile2DView<void> in, mln::bp::Tile2DView<void> out) = 0;
    virtual mln::box2d                      ComputeInputRegion(mln::box2d roi) const noexcept         = 0;
    virtual mln::box2d                      ComputeOutputRegion(mln::box2d roi) const noexcept        = 0;
  };

  class SimpleFilter2D
  {
  public:
    using tile_loader_fn = std::function<void(mln::box2d roi, mln::bp::Tile2DView<void> out)>;
    using tile_writer_fn = std::function<void(mln::box2d roi, mln::bp::Tile2DView<void> out)>;
    using tile_alloc_fn  = std::function<Tile2D<void>(int width, int height)>;

    /// \param tile_width The width of the output tile
    /// \param tile_height The height of the output tile
    SimpleFilter2D(int tile_width, int tile_height);

    /// Run execution for the given output \p roi
    void Execute(mln::box2d roi);

  private:
    tile_loader_fn    m_load;
    tile_writer_fn    m_store;
    tile_generator_fn m_tile_create;
    TileExecutorBase* m_executor;

    // Auxiliary data
    Tile2D<void>      m_tile_in;
    Tile2D<void>      m_tile_out;
  };



  class LocalFilter2D
  {
  public:
    LocalFilter2D(SimpleFilter2D f);

    // Execute
    void execute_parallel(mln::box2d roi, int tile_width, int tile_height);
    void execute_sequential(mln::box2d roi, int tile_width, int tile_height);
    void execute(mln::box2d roi, int tile_width, int tile_height, bool parallel);


  private:
    SimpleFilter2D m_filter;
  };


  /*
  class LocalChainFilter2D
  {
  public:
    // Execute
    void execute_parallel(mln::box2d roi, int tile_width, int tile_height);
    void execute_sequential(mln::box2d roi, int tile_width, int tile_height);
    void execute(mln::box2d roi, int tile_width, int tile_height, bool parallel);

    void add_filter(SimpleFilter2D f);


  private:
    std::vector<SimpleFilter2D> m_filter;
  };



  
  class ParallelLocalCanvas2DBase
  {
  public:
    virtual ~ParallelLocalCanvas2DBase() = default;

    // Dynamic copy
    virtual std::unique_ptr<ParallelLocalCanvas2DBase> clone() const = 0;

  public:
    // Execute the tile
    virtual void ExecuteTile(mln::box2d out_roi) const = 0;


  public:
    // Execute
    void execute_parallel(mln::box2d roi, int tile_width, int tile_height);
    void execute_sequential(mln::box2d roi, int tile_width, int tile_height);
    void execute(mln::box2d roi, int tile_width, int tile_height, bool parallel);
  };


  class ParallelLocalCanvas2D : public ParallelLocalCanvas2DBase
  {
  public:
    int TILE_WIDTH  = 128;
    int TILE_HEIGHT = 128;

    virtual ~ParallelLocalCanvas2D() = default;


    virtual const TileLoaderBase*   GetTileLoader() const noexcept                = 0;
    virtual const TileWriterBase*   GetTileWriter() const noexcept                = 0;
    virtual const TileExecutorBase* GetTileExecutor() const noexcept              = 0;

    // Compute the input region required to compute the given output region
    virtual mln::box2d ComputeInputRegion(mln::box2d out_roi) const noexcept = 0;


    // Execute to compute this output roi
    virtual void ExecuteTile(mln::box2d out_roi) const final;
  };
  */
} // namespace mln
