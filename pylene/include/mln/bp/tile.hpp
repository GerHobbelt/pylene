#pragma once

#include <mln/bp/alloc.hpp>

namespace mln::bp
{

  template <class T>
  class Tile2DView;

  template <>
  class Tile2DView<void>
  {
  public:
    Tile2DView() = default;
    Tile2DView(void* ptr, int width, int height, std::ptrdiff_t pitch) noexcept
      : m_ptr{ptr}
      , m_width{width}
      , m_height{height}
      , m_stride{pitch}
    {
    }

    /// \brief Return a pointer to the beginning of the row
    void*          row(int y) const noexcept { return ptr_offset(m_ptr, y * m_stride); }


    /// \brief Return a raw to pointer to the buffer
    void*          data() const noexcept { return m_ptr; }

    /// \brief Get the stride (in **bytes**)
    std::ptrdiff_t stride() const noexcept { return m_stride; }

    /// \Brief Get the number of cols
    int width() const noexcept { return m_width; }

    /// \brief Get the number of rows
    int height() const noexcept { return m_height; }

    /// \brief Return true if the buffer aligned with the required size
    /// e.g., ``is_aligned(16)`` to check that the buffer is 16-bytes aligned
    bool is_aligned(int width = 16) { return m_ptr & (intptr_t)(width - 1) == 0; }


  protected:
    void*          m_ptr = nullptr;
    int            m_width;
    int            m_height;
    std::ptrdiff_t m_stride;
  };


  template <class T>
  class Tile2DView : public Tile2DView<void>
  {
  public:
    Tile2DView() = default;
    Tile2DView(T* ptr, int width, int height, std::ptrdiff_t pitch) noexcept
      : base(ptr, width, height, pitch)
    {
    }

    T* row(int y) const noexcept { return (T*)(base::row(y)); }
    T& at(int x, int y) const noexcept { return *((T*)(base::row(y)) + x); }
    T& operator()(int x, int y) const noexcept { return *((T*)(base::row(y)) + x); }

    /// \brief Return a sub-area of the tile
    Tile2DView<T> clip(int x, int y, int width, int height);

  private:
    using base = Tile2DView<void>;
  };


  template <class T>
  class Tile2D : public Tile2DView<T>
  {
  public:
    Tile2D(int width, int height);
    ~Tile2D();

    Tile2D(const Tile2D&) = delete;
    Tile2D& operator=(const Tile2D&) = delete;
    Tile2D(Tile2D&&) noexcept;
    Tile2D& operator=(Tile2D&&) noexcept;
  private:
    using base = Tile2DView<T>;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T>
  Tile2DView<T> Tile2DView<T>::clip(int x, int y, int width, int height)
  {
    assert(0 <= x && x < m_width);
    assert(0 <= y && y < m_height);
    assert(0 < width && (x + width) <= m_width);
    assert(0 < height && (y + height) <= m_height);

    return Tile2DView<T>{this->at(x,y), width, height, this->m_stride};
  }

  template <class T>
  Tile2D<T>::Tile2D(int width, int height)
  {
    std::ptrdiff_t pitch;
    this->m_ptr    = aligned_alloc_2d<T>(width, height, pitch);
    this->m_stride = pitch;
    this->m_width  = width;
    this->m_height = height;
  }

  template <class T>
  Tile2D<T>::~Tile2D()
  {
    if (this->m_ptr)
      aligned_free_2d<T>(this->m_ptr, this->m_width, this->m_height, this->m_stride);
  }

  template <class T>
  Tile2D<T>::Tile2D(Tile2D&& other) noexcept
    : base(other)
  {
    other.m_ptr = nullptr;
  }

  template <class T>
  Tile2D<T>& Tile2D<T>::operator=(Tile2D&& other) noexcept
  {
    *(base*)(this) = *(base*)other;
    other.m_ptr = nullptr;
  }


} // namespace mln::bp
