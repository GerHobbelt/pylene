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
    using tile_generator_fn = mln::bp::Tile2D<void> (*)(int width, int height);
    using tile_transpose_fn = void (*)(mln::bp::Tile2DView<void> in, mln::bp::Tile2DView<void> out);

    SimpleFilter2D() = default;


    SimpleFilter2D(const SimpleFilter2D&) noexcept;
    SimpleFilter2D& operator=(const SimpleFilter2D&) noexcept;
    SimpleFilter2D(SimpleFilter2D&&) = default;
    SimpleFilter2D& operator=(SimpleFilter2D&&) = default;

    void SetLoader(tile_loader_fn load_fn) noexcept { m_load = std::move(load_fn); }
    void SetWriter(tile_writer_fn write_fn) noexcept { m_store = std::move(write_fn); }
    void SetTileGenerator(tile_generator_fn fn) noexcept { m_tile_create = fn; }
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

    virtual void set_output(mln::ndbuffer_image* out) = 0;
    virtual void set_input(mln::ndbuffer_image* out)  = 0;

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

    template <class I>
    void set_input(I& input);

    void set_input(mln::ndbuffer_image* in) final;


    template <class J>
    void set_output(J& output);

    void set_output(mln::ndbuffer_image* out) final;
  };

  template <class V>
  class LocalChainFilter2D;


  template <>
  class LocalChainFilter2D<void>
  {
  public:
    LocalChainFilter2D() = default;
    virtual ~LocalChainFilter2D() = default;

    void add_executor(std::unique_ptr<TileExecutorBase> f) { m_filters.push_back(std::move(f)); }
    void execute(mln::box2d roi, int tile_width, int tile_height, bool parallel);

  protected:
    virtual LocalFilter2D<void>* get_first_filter() noexcept = 0;
    virtual LocalFilter2D<void>* get_middle_filter() noexcept = 0;
    virtual LocalFilter2D<void>* get_last_filter() noexcept = 0;
    virtual mln::ndbuffer_image  create_tmp_image(mln::box2d domain) = 0;


    std::vector<std::unique_ptr<TileExecutorBase>>    m_filters;
  };


  template <class V>
  class LocalChainFilter2D : public LocalChainFilter2D<void>
  {
  public:
    LocalChainFilter2D() = default;

    template <class I>
    void set_input(I& input, e_padding_mode padding_mode, V padding_value)
    {
      m_first_filter.set_input(input, padding_mode, padding_value);
    }

    template <class J>
    void set_output(J& output)
    {
      m_last_filter.set_output(output);
    }

  protected:
    LocalFilter2D<void>* get_first_filter() noexcept final { return &m_first_filter; }
    LocalFilter2D<void>* get_middle_filter() noexcept final { return &m_middle_filter; }
    LocalFilter2D<void>* get_last_filter() noexcept final { return &m_last_filter; }
    mln::ndbuffer_image  create_tmp_image(mln::box2d domain) final { return mln::image2d<V>(domain); }


  private:
    LocalFilter2D<V> m_first_filter;
    LocalFilter2D<V> m_middle_filter;
    LocalFilter2D<V> m_last_filter;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class V>
  LocalFilter2D<V>::LocalFilter2D()
  {
    static auto fn_1 = [](int width, int height) -> mln::bp::Tile2D<void> { return mln::bp::Tile2D<V>(width, height); };
    m_filter.SetTileGenerator(fn_1);

    static auto fn_2 = [](mln::bp::Tile2DView<void> in, mln::bp::Tile2DView<void> out) {
      mln::bp::transpose((V*)in.data(), (V*)out.data(), out.width(), out.height(), in.stride(), out.stride());
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
  template <class I>
  void LocalFilter2D<V>::set_input(I& input)
  {
    auto fn = [&input](mln::point2d anchor, mln::bp::Tile2DView<void> out_) {
      auto out = mln::bp::downcast<V>(out_);
      auto roi = mln::box2d{anchor.x(), anchor.y(), out.width(), out.height()};
      mln::paste(input, roi, mln::image2d<V>::from_tile(out, anchor));
    };
    m_filter.SetLoader(fn);
  }

  template <class V>
  void LocalFilter2D<V>::set_input(mln::ndbuffer_image* in)
  {
    auto* input = static_cast<mln::image2d<V>*>(in);
    auto fn = [input](mln::point2d anchor, mln::bp::Tile2DView<void> out_) {
      auto out = mln::bp::downcast<V>(out_);
      auto roi = mln::box2d{anchor.x(), anchor.y(), out.width(), out.height()};
      mln::paste(*input, roi, mln::image2d<V>::from_tile(out, anchor));
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

  template <class V>
  void LocalFilter2D<V>::set_output(mln::ndbuffer_image* out)
  {
    auto* output = static_cast<mln::image2d<V>*>(out);
    auto  fn     = [output](mln::point2d anchor, mln::bp::Tile2DView<void> in_) {
      auto in  = mln::bp::downcast<V>(in_);
      auto img = mln::image2d<V>::from_tile(in, anchor);
      paste(img, img.domain(), *output);
    };
    m_filter.SetWriter(fn);
  }


} // namespace mln
