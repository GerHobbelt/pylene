#pragma once

#include <mln/core/canvas/parallel_local.hpp>
#include <mln/core/extension/padding.hpp>

namespace mln::details
{

  template <class I>
  class TileLoader2D : public TileLoaderBase
  {
    using V = image_value_t<I>;

  public:
    /// \param width the extended width of the tile (width of the tile + width of the border)
    /// \param height the extended height of the tile (height of the tile + height of the border)
    TileLoader2D(I input, int width, int height, e_padding_mode padding_mode, V padding_value)
      : m_in{input}
      , m_tile{width, height}
      , m_padding_mode{padding_mode}
      , m_padding_value{padding_value}
        //, m_transpose{transpose}
    {
    }

    TileLoader2D(const TileLoader2D& other)
      : m_in{other.m_in}
      , m_tile{other.m_tile.width(), other.m_tile.height()}
      , m_padding_mode(other.m_padding_mode)
      , m_padding_value{other.m_padding_value}
    {
    }

    TileLoader2D& operator=(const TileLoader2D&) = delete;

    /// \param roi The tile region
    /// \param input_roi The extended roi required to compute the tile region
    void load_tile(mln::box2d /* roi */, mln::box2d input_roi) const final
    {
      assert(m_tile.width() >= input_roi.width() && "Tile width mismatches");
      assert(m_tile.height() >= input_roi.height() && "Tile height mismatches");

      m_tile.set_domain_topleft(input_roi.tl());
      assert(m_tile.domain().includes(input_roi));
      assert(m_tile.domain().tl() == input_roi.tl());

      auto dst = m_tile.clip(input_roi);
      // if (m_transpose)
      //   transpose_pad(m_in, dst, padding_method, padding_value);
      // else
      copy_pad(m_in, dst, m_padding_mode, m_padding_value);
      // mln::io::imprint(m_tile);
    }

    mln::ndbuffer_image get_tile() const final { return m_tile; }

  private:
    I                       m_in;
    mutable mln::image2d<V> m_tile;
    e_padding_mode          m_padding_mode;
    V                       m_padding_value;
    //bool                    m_transpose;
  };


  template <class InputImage, typename V>
  struct DirectTileLoader2D
  {
    void operator()(mln::ndbuffer_image& out_) const
    {
      auto& out = out_.__cast<V, 2>();
      copy_pad(*m_in, out, m_padding_mode, m_padding_value);
    }

    void operator()(mln::ndbuffer_image&& out_) const
    {
      auto& out = out_.__cast<V, 2>();
      copy_pad(*m_in, out, m_padding_mode, m_padding_value);
    }


    InputImage*             m_in;
    e_padding_mode          m_padding_mode;
    V                       m_padding_value;

  };

  template <class OutputImage, typename V>
  struct DirectTileWriter2D
  {
    void operator()(mln::ndbuffer_image& in_) const
    {
      auto& in  = in_.__cast<V, 2>();
      mln::paste(in, in.domain(), *m_out);
    }

    void operator()(mln::ndbuffer_image&& in_) const
    {
      auto& in  = in_.__cast<V, 2>();
      mln::paste(in, in.domain(), *m_out);
    }


    OutputImage* m_out;
  };




}
