#pragma once

#include <mln/core/algorithm/paste.hpp>
#include <mln/core/box.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/value/value_traits.hpp>

#include <mln/bp/tile.hpp>
#include <mln/bp/transpose.hpp>
#include <mln/core/extension/padding.hpp>

namespace mln
{



  class TileExecutorBase
  {
  public:
    virtual ~TileExecutorBase() = default;
    virtual void                            Execute(mln::bp::Tile2DView<void> in, mln::point2d i_anchor, mln::bp::Tile2DView<void> out, mln::point2d o_anchor) = 0;
    virtual mln::box2d                      ComputeInputRegion(mln::box2d roi) const noexcept                    = 0;
    virtual mln::box2d                      ComputeOutputRegion(mln::box2d roi) const noexcept                   = 0;
    virtual int                             GetElementSize() const noexcept                                      = 0;
    virtual bool                            NeedTranspose() const noexcept                                       = 0;
  };

  class SimpleFilter2D
  {
  public:
    using tile_loader_fn    = std::function<void(mln::point2d anchor, mln::bp::Tile2DView<void> out)>;
    using tile_writer_fn    = std::function<void(mln::point2d anchor, mln::bp::Tile2DView<void> out)>;
    using tile_generator_fn = std::function<mln::bp::Tile2D<void>(int width, int height)>;
    using tile_transpose_fn = std::function<void(mln::bp::Tile2DView<void> in, mln::bp::Tile2DView<void> out)>;

    SimpleFilter2D() = default;


    SimpleFilter2D(const SimpleFilter2D&) noexcept;
    SimpleFilter2D& operator=(const SimpleFilter2D&) noexcept;
    SimpleFilter2D(SimpleFilter2D&&) = default;
    SimpleFilter2D& operator=(SimpleFilter2D&&) = default;

    void SetLoader(tile_loader_fn load_fn) noexcept { m_load = std::move(load_fn); }
    void SetWriter(tile_writer_fn write_fn) noexcept { m_store = std::move(write_fn); }
    void SetTileGenerator(tile_generator_fn fn) noexcept { m_tile_create = std::move(fn); }
    void SetTranposeFunction(tile_transpose_fn fn) noexcept { m_transpose = fn; }
    void SetExecutor(TileExecutorBase* exec) noexcept { m_executor = exec; }



    /// Run execution for the given output \p roi
    /// \param tile_width The (maximal) width of the output tile
    /// \param tile_height The (maximal) height of the output tile
    void SetTileSize(int tile_width, int tile_height) noexcept;
    void Execute(mln::box2d roi) const;


  private:
    tile_loader_fn    m_load;
    tile_writer_fn    m_store;
    tile_generator_fn m_tile_create;
    tile_transpose_fn m_transpose;
    TileExecutorBase* m_executor;

    // Auxiliary data
    mln::bp::Tile2D<void>      m_tile_in;
    mln::bp::Tile2D<void>      m_tile_out;
    mln::bp::Tile2D<void>      m_aux_in;  // Used for transposition
    mln::bp::Tile2D<void>      m_aux_out; // Used for transposition
  };

  template <class V>
  class LocalFilter2D;

  template <>
  class LocalFilter2D<void>
  {
  public:
    void set_executor(TileExecutorBase* executor) noexcept  {m_filter.SetExecutor(executor); }

    // Execute
    void execute_parallel(mln::box2d roi, int tile_width, int tile_height);
    void execute_sequential(mln::box2d roi, int tile_width, int tile_height);
    void execute(mln::box2d roi, int tile_width, int tile_height, bool parallel);


  protected:
    SimpleFilter2D m_filter;
  };

  template <class V>
  class LocalFilter2D : public LocalFilter2D<void>
  {
  public:
    LocalFilter2D();

    template <class I>
    void set_input(I& input, e_padding_mode padding_mode, V padding_value);

    template <class J>
    void set_output(J& output);
  };

  class LocalChainFilter2D
  {
  public:
    LocalChainFilter2D() = default;

    // Execute
    void execute(mln::box2d roi, int tile_width, int tile_height, bool parallel);

    void set_writer(SimpleFilter2D::tile_writer_fn fn);
    void set_tile_generator(SimpleFilter2D::tile_generator_fn fn);
    void add_filter(TileExecutorBase* f);

  private:
    std::vector<TileExecutorBase*>    m_filters;
    SimpleFilter2D::tile_loader_fn    m_load;
    SimpleFilter2D::tile_writer_fn    m_store;
    SimpleFilter2D::tile_generator_fn m_tile_generator;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class V>
  LocalFilter2D<V>::LocalFilter2D()
  {
    auto fn_1 = [](int width, int height) { return mln::bp::Tile2D<V>(width, height); };
    m_filter.SetTileGenerator(fn_1);

    auto fn_2 = [](mln::bp::Tile2DView<void> in, mln::bp::Tile2DView<void> out) {
      mln::bp::transpose((V*)in.data(), (V*)out.data(), out.width(), out.height(), in.stride(), out.stride());
      return out;
    };
    m_filter.SetTranposeFunction(fn_2);
  }

  template <class V>
  template <class I>
  void LocalFilter2D<V>::set_input(I& input, e_padding_mode padding_mode, V padding_value)
  {
    auto fn = [&input, padding_mode, padding_value](mln::point2d anchor, mln::bp::Tile2DView<void> out_) {
      auto out = mln::bp::downcast<V>(out_);
      copy_pad(input, out, anchor, padding_mode, padding_value);
    };
    m_filter.SetLoader(fn);
  }

  template <class V>
  template <class J>
  void LocalFilter2D<V>::set_output(J& output)
  {
    auto fn = [&output](mln::point2d anchor, mln::bp::Tile2DView<void> in_) {
      auto in  = mln::bp::downcast<V>(in_);
      auto img = mln::image2d<V>::from_tile(in, anchor);
      paste(img, img.domain(), output);
    };
    m_filter.SetWriter(fn);
  }


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
