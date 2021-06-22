#pragma once

#include <mln/bp/alloc.hpp>
#include <utility>
#include <cassert>

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
    bool is_aligned(int width = 16) { return ((intptr_t)m_ptr & (intptr_t)(width - 1)) == 0; }


    /// \brief Clip a buffer view to the given region
    /// \paral size The size (in bytes) of the elements
    Tile2DView<void> clip(int x, int y, int width, int height, int size) const noexcept;

    /// \brief Clip a buffer view to the given size
    Tile2DView<void> clip(int width, int height) const noexcept;

  protected:
    void*          m_ptr = nullptr;
    int            m_width;
    int            m_height;
    std::ptrdiff_t m_stride;
  };

  /// \brief Downcast
  template <class T>
  Tile2DView<T>& downcast(Tile2DView<void>& in) { return static_cast<Tile2DView<T>&>(in); }

  /// \brief Downcast
  template <class T>
  const Tile2DView<T>& downcast(const Tile2DView<void>& in) { return static_cast<const Tile2DView<T>&>(in); }




  template <class T>
  class Tile2DView : public Tile2DView<void>
  {
  public:
    Tile2DView() = default;
    Tile2DView(T* ptr, int width, int height, std::ptrdiff_t pitch) noexcept
      : base(ptr, width, height, pitch)
    {
    }

    /// \brief Return a raw to pointer to the buffer
    T* data() const noexcept { return (T*)m_ptr; }


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
    using free_fn_t = void (*)(void*);
  public:
    Tile2D() = default;
    Tile2D(int width, int height);
    ~Tile2D();

    Tile2D(const Tile2D&) = delete;
    Tile2D& operator=(const Tile2D&) = delete;

    Tile2D(Tile2D&&) noexcept;
    Tile2D& operator=(Tile2D&&) noexcept;


    static Tile2D<T> acquire(T* ptr, int width, int height, std::ptrdiff_t stride, free_fn_t free_fn= nullptr) noexcept;
    T*               release() noexcept;


  private:
    using base = Tile2DView<T>;
    template <class> friend class Tile2D;
    free_fn_t m_free = nullptr;
  };


  template <>
  class Tile2D<void> : public Tile2DView<void>
  {
    using free_fn_t = void (*)(void*);
    using destroy_fn_t = void(*)(void*, std::size_t);
  public:
    Tile2D() = default;
    ~Tile2D();

    template <class T>
    Tile2D(Tile2D<T>&& other) noexcept;
    template <class T>
    Tile2D& operator=(Tile2D<T>&& other) noexcept;


    Tile2D(const Tile2D&) = delete;
    Tile2D& operator=(const Tile2D&) = delete;

    Tile2D(Tile2D&&) noexcept;
    Tile2D& operator=(Tile2D&&) noexcept;


    static Tile2D<void> acquire(void* ptr, int width, int height, std::ptrdiff_t stride,
                                free_fn_t free_fn = nullptr) noexcept;
    void*            release() noexcept;

  private:
    using base = Tile2DView<void>;
    destroy_fn_t m_destroy = nullptr; // Type related
    free_fn_t    m_free    = nullptr; // Allocator related (malloc vs cudaMalloc)
  };



  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class T>
  inline Tile2D<void>::Tile2D(Tile2D<T>&& other) noexcept
    : base(other)
  {
    this->m_ptr = std::exchange(other.m_ptr, nullptr);
    this->m_free = std::exchange(other.m_free, nullptr);

    if constexpr (!std::is_trivially_destructible_v<T>)
      this->m_destroy = [](void* ptr, std::size_t n) { std::destroy_n((T*)ptr, n); };
  }

  template <class T>
  inline Tile2D<void>& Tile2D<void>::operator=(Tile2D<T>&& other) noexcept
  {
    std::destroy_at(this);

    (base&)(*this) = (base&)other;
    this->m_ptr = std::exchange(other.m_ptr, nullptr);
    this->m_free = std::exchange(other.m_free, nullptr);
    if constexpr (!std::is_trivially_destructible_v<T>)
      this->m_destroy = [](void* ptr, std::size_t n) { std::destroy_n((T*)ptr, n); };

    return *this;
  }

  inline Tile2D<void>::Tile2D(Tile2D<void>&& other) noexcept
    : base(other)
  {
    this->m_ptr     = std::exchange(other.m_ptr, nullptr);
    this->m_free    = std::exchange(other.m_free, nullptr);
    this->m_destroy = std::exchange(other.m_destroy, nullptr);
  }

  inline Tile2D<void>& Tile2D<void>::operator=(Tile2D<void>&& other) noexcept
  {
    std::swap(*(base*)this, (base&)other);
    std::swap(m_free, other.m_free);
    std::swap(m_destroy, other.m_destroy);
    return *this;
  }

  inline Tile2D<void>::~Tile2D()
  {
    if (this->m_ptr)
    {
      if (m_destroy)
        for (int y = 0; y < this->m_height; y++)
          m_destroy(mln::bp::ptr_offset(this->m_ptr, this->m_stride * y), this->m_width);
      this->m_free(this->m_ptr);
    }
  }



  inline Tile2DView<void> Tile2DView<void>::clip(int x, int y, int width, int height, int size) const noexcept
  {
    assert(0 <= x && x < m_width);
    assert(0 <= y && y < m_height);
    assert(0 < width && (x + width) <= m_width);
    assert(0 < height && (y + height) <= m_height);

    return Tile2DView<void>{mln::bp::ptr_offset(this->row(y), x * size), width, height, this->m_stride};
  }

  inline Tile2DView<void> Tile2DView<void>::clip(int width, int height) const noexcept
  {
    assert(0 < width && width <= m_width);
    assert(0 < height && height <= m_height);
    return Tile2DView<void>{this->m_ptr, width, height, this->m_stride};
  }



  template <class T>
  Tile2DView<T> Tile2DView<T>::clip(int x, int y, int width, int height)
  {
    assert(0 <= x && x < m_width);
    assert(0 <= y && y < m_height);
    assert(0 < width && (x + width) <= m_width);
    assert(0 < height && (y + height) <= m_height);

    return Tile2DView<T>{this->row(y) + x, width, height, this->m_stride};
  }


  template <class T>
  Tile2D<T>::Tile2D(int width, int height)
  {
    std::ptrdiff_t pitch;
    this->m_ptr    = aligned_alloc_2d<T>(width, height, pitch);
    this->m_stride = pitch;
    this->m_width  = width;
    this->m_height = height;
    this->m_free   = (free_fn_t)aligned_free_2d;
  }

  template <class T>
  Tile2D<T>::~Tile2D()
  {
    if (this->m_ptr)
    {
      for (int y = 0; y < this->m_height; y++)
        std::destroy_n(mln::bp::ptr_offset((T*)this->m_ptr, this->m_stride * y), this->m_width);
      this->m_free(this->m_ptr);
    }
  }


  template <class T>
  Tile2D<T>::Tile2D(Tile2D&& other) noexcept
    : base(other)
  {
    this->m_ptr = std::exchange(other.m_ptr, nullptr);
    this->m_free = std::exchange(other.m_free, nullptr);
  }

  template <class T>
  Tile2D<T>& Tile2D<T>::operator=(Tile2D&& other) noexcept
  {
    std::swap((base&)(*this), (base&)other);
    std::swap(this->m_free, other.m_free);
    return *this;
  }

  template <class T>
  T* Tile2D<T>::release() noexcept
  {
    return (T*)std::exchange(this->m_ptr, nullptr);
  }

  template <class T>
  Tile2D<T> Tile2D<T>::acquire(T* ptr, int width, int height, std::ptrdiff_t pitch, free_fn_t free_fn) noexcept
  {
    Tile2D t;
    t.m_ptr     = ptr;
    t.m_stride  = pitch;
    t.m_width   = width;
    t.m_height  = height;
    t.m_free    = free_fn;
    return t;
  }


} // namespace mln::bp
