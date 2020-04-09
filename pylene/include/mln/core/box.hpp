#pragma once

#include <mln/core/experimental/point.hpp>
#include <mln/core/rangev3/mdindex.hpp>

#include <array>
#include <type_traits>
#include <algorithm>


namespace mln::experimental
{
  // Forward declaration
  template <class Impl>
  class _box;

  namespace impl
  {
    template <int D, typename T = int>
    struct _bstorage;

    template <int D, class T>
    struct _bref;
  }; // namespace impl

  /******************************************/
  /****   Box Types and Types Aliases    ****/
  /******************************************/

  using Box         = _box<impl::_bstorage<-1, int>>;
  using ConstBoxRef = _box<impl::_bref<-1, const int>>;

  template <int dim>
  using ndbox = _box<impl::_bstorage<dim, int>>;
  template <int dim>
  using ndboxref = _box<impl::_bref<dim, const int>>;


  using box1d           = ndbox<1>;
  using box2d           = ndbox<2>;
  using box3d           = ndbox<3>;
  using const_box1d_ref = ndboxref<1>;
  using const_box2d_ref = ndboxref<2>;
  using const_box3d_ref = ndboxref<3>;


  /******************************************/
  /****          Box Interface          *****/
  /******************************************/

  /// \brief Box Public Interface shared by all "box" types
  template <class Impl>
  class _box : private Impl
  {
  public:
    template <class _i>
    friend class _box;

    template <int, class>
    friend struct impl::_bstorage;

    template <int, class>
    friend struct impl::_bref;


    using Impl::Impl;
    using Impl::ndim;
    using typename Impl::coord_type;
    using point_type = ndpoint<ndim, coord_type>;

    /// Observers
    /// \{

    /// \brief Return the number of elements in the box
    constexpr std::size_t size() const noexcept;

    /// \brief Return the number of elements in the k-th dimension of the box
    constexpr int size(int k) const noexcept;

    /// \brief Return true if the box is empty
    constexpr bool empty() const noexcept;

    /// \brief Returns the width of the box
    constexpr int width() const noexcept requires(Impl::ndim == -1 || Impl::ndim >= 1)
    {
      return size(0);
    }

    /// \brief Returns the height of the box
    constexpr int height() const noexcept requires(Impl::ndim == -1 || Impl::ndim >= 2)
    {
      return size(1);
    }

    /// \brief Returns the depth of the box
    constexpr int depth() const noexcept requires (Impl::ndim == -1 || Impl::ndim >= 3)
    {
      return size(2);
    }

    using Impl::cursor;
    using Impl::backward_cursor;
    using Impl::begin_cursor;
    using Impl::end_cursor;
    using Impl::rbegin_cursor;
    using Impl::rend_cursor;


    template <int d = ndim, class = std::enable_if_t<d != -1>>
    auto rows() const
    {
      return this->Impl::rows();
    }

    template <int d = ndim, class = std::enable_if_t<d != -1>>
    auto rrows() const
    {
      return this->Impl::rrows();
    }


    /// \brief Returns the top-left corner point
    using Impl::tl;

    /// \brief Returns the x coordinate of the top-left corner point
    constexpr int x() const noexcept { return this->__begin(0); }

    /// \brief Returns the y coordinate of the top-left corner point
    template <int d = ndim, class = std::enable_if_t<d == -1 || d >= 2>>
    constexpr int y() const noexcept { return this->__begin(1); }

    /// \brief Returns the z coordinate of the top-left corner point
    template <int d = ndim, class = std::enable_if_t<d == -1 || d >= 3>>
    constexpr int z() const noexcept { return this->__begin(2); }

    /// \brief Returns the w coordinate of the top-left corner point
    template <int d = ndim, class = std::enable_if_t<d == -1 || d >= 4>>
    constexpr int w() const noexcept { return this->__begin(3); }


    /// \brief Returns the k-th coordinate of the top-left corner point
    constexpr int tl(int k) const noexcept { return this->__begin(k); }

    /// \brief Returns the k-th coordinate of the bottom-right corner point (past-the-end)
    constexpr int br(int k) const noexcept { return this->__end(k); }

    /// \brief Returns the bottom-right (past-the-end) corner point
    using Impl::br;

    /// \brief Returns the shape of the domain
    using Impl::shape;

    /// \brief Returns the number of dimensions
    using Impl::dim;

    /// \brief Returns the size of each dimension
    using Impl::extents;

    /// \brief Returns a pointer to the coordinate array
    using Impl::data;
    /// \}

    /// Modifiers
    /// \{

    /// \brief Set the size of the box in the k-th dimension to a new size (does not move top-left corner)
    constexpr void set_size(int k, int size) noexcept { this->__end(k) = this->__begin(k) + size; }

    /// \brief Set the width of the box to a new width (does not move top-left corner)
    template <int d = ndim, class = std::enable_if_t<d == -1 || d >= 1>>
    constexpr void set_width(int width) noexcept
    {
      this->set_size(0, width);
    }

    /// \brief Set the height of the box to a new height (does not move top-left corner)
    template <int d = ndim, class = std::enable_if_t<d == -1 || d >= 2>>
    constexpr void set_height(int height) noexcept
    {
      this->set_size(1, height);
    }

    /// \brief Set the depth of the box to a new depth (does not move top-left corner)
    template <int d = ndim, class = std::enable_if_t<d == -1 || d >= 3>>
    constexpr void set_depth(int depth) noexcept
    {
      this->set_size(2, depth);
    }

    /// \brief Inflate (or deflate) the box
    constexpr void inflate(int k) noexcept;
    /// \}


    /// Set operations
    /// \{
    template <class _i>
    constexpr bool operator==(const _box<_i>& B) const noexcept;

    template <class _i>
    constexpr bool operator!=(const _box<_i>& B) const noexcept;

    template <class _p>
    constexpr bool has(_point<_p> x) const noexcept;

    template <class _i>
    constexpr bool includes(const _box<_i>& B) const noexcept;

    template <class _i>
    constexpr bool intersects(const _box<_i>& B) const noexcept;

    /// \brief Clip the box to a given region (compute the intersection)
    template <class _i>
    constexpr void clip(const _box<_i>& B) noexcept;
    /// \}

    /// Projection operations
    /// \{

    /// \brief Clamp a point to box boundaries
    ///
    /// yᵢ = clamp(xᵢ, aᵢ, bᵢ)
    /// \postcondition b.has(p)
    template <class _p>
    point_type clamp(_point<_p> x) const noexcept;


    /// \brief Project a point into a box by mirroring
    ///
    /// yᵢ = clamp(xᵢ, aᵢ, bᵢ)
    /// \postcondition box.has(p)
    template <class _p>
    point_type mirror(_point<_p> x) const noexcept;

    /// \brief Project a point into a box by mirroring
    /// \postcondition b.has(p)
    template <class _p>
    point_type periodize(_point<_p> x) const noexcept;
    /// \}



  private:
    template <int K>
    constexpr int __size() const noexcept;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace impl
  {
    template <class _i1, class _i2>
    using is_impl_compatible_with =
        std::integral_constant<bool, (_i1::ndim == -1 || _i2::ndim == -1 || _i1::ndim == _i2::ndim) &&
                                         std::is_convertible_v<typename _i1::coord_type, typename _i2::coord_type>>;


    /// Storage for a box
    template <int D, typename T>
    struct _bstorage : mln::ranges::mdindex_facade<D, T, ndpoint<D, T>, _bstorage<D, T>>
    {
    private:
      using self_t     = _bstorage;
      using point_type = ndpoint<D, T>;

    public:
      static constexpr int ndim = D;
      using coord_type          = T;

      constexpr void _zeros() noexcept
      {
        for (int k = 0; k < D; ++k)
        {
          m_begin[k] = 0;
          m_end[k]   = 0;
        }
      }

      constexpr _bstorage() = default;

      template <class _i>
      constexpr _bstorage(const _box<_i>& b) noexcept
      {
        static_assert(is_impl_compatible_with<_i, self_t>::value,
                      "Number of dimension and/or coordinate type mismatch.");
        assert(this->dim() == b.dim() && "Incompatible number of dimensions");

        for (int i = 0; i < D; ++i)
        {
          m_begin[i] = b.__begin(i);
          m_end[i]   = b.__end(i);
        }
      }


      constexpr _bstorage(ConstPointRef p1, ConstPointRef p2)
      {
        assert(p1.dim() == D && "Invalid number of dimensions");
        assert(p2.dim() == D && "Invalid number of dimensions");

        bool empty = false;
        for (int k = 0; k < D; ++k)
        {
          if (p1[k] < p2[k])
          {
            m_begin[k] = p1[k];
            m_end[k]   = p2[k];
          }
          else if (p1[k] > p2[k])
          {
            m_begin[k] = p2[k];
            m_end[k]   = p1[k];
          }
          else
          {
            empty = true;
            break;
          }
        }
        if (empty)
          _zeros();
      }


      constexpr _bstorage(int width) requires(D == 1)
        : m_begin{0}
        , m_end{width}
      {
        assert(width >= 0);
      }


      constexpr _bstorage(int x, int width) requires(D == 1)
        : m_begin{x}
        , m_end{x + width}
      {
        assert(width >= 0);
      }


      constexpr _bstorage(int width, int height) requires (D == 2)
        : m_begin{0, 0}
        , m_end{width, height}
      {
        assert(width >= 0 && height >= 0);
      }

      constexpr _bstorage(int x, int y, int width, int height) requires(D == 2)
        : m_begin{x, y}
        , m_end{x + width, y + height}
      {
        assert(width >= 0 && height >= 0);
      }


      template <int d = D, class = std::enable_if_t<d == 3>>
      constexpr _bstorage(int width, int height, int depth)
        : m_begin{0, 0, 0}
        , m_end{width, height, depth}
      {
        assert(width >= 0 && height >= 0 && depth >= 0);
      }

      constexpr _bstorage(int x, int y, int z, int width, int height, int depth) requires(D == 3)
        : m_begin{x, y, z}
        , m_end{x + width, y + height, z + depth}
      {
        assert(width >= 0 && height >= 0 && depth >= 0);
      }


      constexpr _bstorage(int width, int height, int depth, int duration) requires (D == 4)
        : m_begin{0, 0, 0, 0}
        , m_end{width, height, depth, duration}
      {
        assert(width >= 0 && height >= 0 && depth >= 0 && duration >= 0);
      }

      constexpr _bstorage(int x, int y, int z, int w, int width, int height, int depth, int duration) requires (D == 4)
        : m_begin{x, y, z, w}
        , m_end{x + width, y + height, z + depth, w + duration}
      {
        assert(width >= 0 && height >= 0 && depth >= 0 && duration >= 0);
      }



      static constexpr int dim() noexcept { return D; }
      constexpr const T*   data() const noexcept { return m_begin.data(); }
      constexpr T*         data() noexcept { return m_begin.data(); }

      constexpr const T* __begin() const noexcept { return m_begin.data(); }
      constexpr T*       __begin() noexcept { return m_begin.data(); }
      constexpr const T* __end() const noexcept { return m_end.data(); }
      constexpr T*       __end() noexcept { return m_end.data(); }

      constexpr T __begin(int k) const noexcept
      {
        assert(k < D);
        return m_begin[k];
      }
      constexpr T __end(int k) const noexcept
      {
        assert(k < D);
        return m_end[k];
      }
      constexpr T& __begin(int k) noexcept
      {
        assert(k < D);
        return m_begin[k];
      }
      constexpr T& __end(int k) noexcept
      {
        assert(k < D);
        return m_end[k];
      }


      constexpr point_type  tl() const noexcept { return m_begin; }
      constexpr point_type& tl() noexcept { return m_begin; }
      constexpr point_type  br() const noexcept { return m_end; }
      constexpr point_type& br() noexcept { return m_end; }

      constexpr auto shape() const noexcept { return br() - tl(); }
      constexpr auto extents() const noexcept
      {
        auto pmin = tl();
        auto pmax = br();
        auto ret  = std::array<std::size_t, D>{};
        for (int i = 0; i < D; i++)
          ret[i] = pmax[i] - pmin[i];
        return ret;
      }

      using base = mln::ranges::mdindex_facade<D, T, point_type, _bstorage<D, T>>;
      typename base::cursor          begin_cursor() const { return {m_begin.data(), m_end.data(), false}; }
      typename base::backward_cursor rbegin_cursor() const { return {m_begin.data(), m_end.data(), true}; }


      point_type m_begin = {};
      point_type m_end   = {};
    };


    /// Storage for a dynamic box
    template <typename T>
    struct _bstorage<-1, T>
    {
      static constexpr int ndim = -1;
      using coord_type          = T;

      constexpr void _zeros() noexcept
      {
        for (int i = 0; i < 8; i++)
          m_coords[i] = 0;
      }

      constexpr _bstorage() = default;

      // Converting constructors
      template <class _i>
      constexpr _bstorage(const _box<_i>& b)
        : m_dim(b.dim())
      {
        assert(m_dim <= 4 && "Cannot handle dyn dimension greater than 4");
        for (int i = 0; i < m_dim; ++i)
        {
          m_coords[i]         = b.__begin(i);
          m_coords[m_dim + i] = b.__end(i);
        }
      }

      constexpr _bstorage(T w) noexcept
        : m_dim{1}
        , m_coords{0, w, 0, 1, 0, 1, 0, 1}
      {
      }
      constexpr _bstorage(T w, T h) noexcept
        : m_dim{2}
        , m_coords{0, 0, w, h, 0, 1, 0, 1}
      {
      }
      constexpr _bstorage(T w, T h, T depth) noexcept
        : m_dim{3}
        , m_coords{0, 0, 0, w, h, depth, 0, 1}
      {
      }
      constexpr _bstorage(T w, T h, T depth, T duration) noexcept
        : m_dim{4}
        , m_coords{0, 0, 0, 0, w, h, depth, duration}
      {
      }
      constexpr _bstorage(ConstPointRef tl, ConstPointRef br) noexcept
        : m_dim{tl.dim()}
        , m_coords{}
      {
        assert(tl.dim() == br.dim());

        bool empty = false;
        for (int i = 0; i < m_dim; i++)
        {
          if (tl[i] < br[i])
          {
            m_coords[i]         = tl[i];
            m_coords[m_dim + i] = br[i];
          }
          else if (br[i] < tl[i])
          {
            m_coords[i]         = tl[i];
            m_coords[m_dim + i] = br[i];
          }
          else
          {
            empty = true;
            break;
          }
        }
        if (empty)
          _zeros();
      }

      constexpr int      dim() const noexcept { return m_dim; }
      constexpr T*       data() noexcept { return m_coords; }
      constexpr const T* data() const noexcept { return m_coords; }

      constexpr const T* __begin() const noexcept { return m_coords; }
      constexpr T*       __begin() noexcept { return m_coords; }
      constexpr const T* __end() const noexcept { return m_coords + m_dim; }
      constexpr T*       __end() noexcept { return m_coords + m_dim; }


      constexpr T __begin(int k) const noexcept
      {
        assert(k < m_dim);
        return m_coords[k];
      }
      constexpr T __end(int k) const noexcept
      {
        assert(k < m_dim);
        return m_coords[m_dim + k];
      }
      constexpr T& __begin(int k) noexcept
      {
        assert(k < m_dim);
        return m_coords[k];
      }
      constexpr T& __end(int k) noexcept
      {
        assert(k < m_dim);
        return m_coords[m_dim + k];
      }

      constexpr ConstPointRef tl() const noexcept { return {m_dim, m_coords}; }
      constexpr PointRef      tl() noexcept { return {m_dim, m_coords}; }
      constexpr ConstPointRef br() const noexcept { return {m_dim, m_coords + m_dim}; }
      constexpr PointRef      br() noexcept { return {m_dim, m_coords + m_dim}; }

      constexpr auto shape() const noexcept { return tl() - br(); }
      auto           extents() const noexcept
      {
        auto pmin = tl();
        auto pmax = br();
        auto ret  = std::vector<std::size_t>{m_dim, 0};
        for (int i = 0; i < m_dim; i++)
          ret[i] = pmax[i] - pmin[i];
        return ret;
      }

      constexpr int begin() const noexcept { return 0; }
      constexpr int end() const noexcept { return 1; }

      void begin_cursor() const {}
      void rbegin_cursor() const {};
      void end_cursor() const {}
      void rend_cursor() const {};
      using cursor = void;
      using backward_cursor = void;


      int m_dim       = 0;
      T   m_coords[8] = {0}; // (x1,y1,z1,w1) -- (x2,y2,z2,w2)
    };

    /// Reference to box wrapper
    template <int D, class T>
    struct _bref
      : mln::ranges::mdindex_facade<D, std::remove_const_t<T>, ndpoint<D, std::remove_const_t<T>>, _bref<D, T>>
    {
      using base =
          mln::ranges::mdindex_facade<D, std::remove_const_t<T>, ndpoint<D, std::remove_const_t<T>>, _bref<D, T>>;

      static constexpr int ndim = D;
      using coord_type          = T;

      template <class Impl>
      constexpr _bref(const _box<Impl>& b) noexcept
        : m_coords{b.data()}
      {
        assert(b.dim() == D && "Incompatible number of dimension.");
      }

      constexpr int dim() const noexcept { return D; }
      constexpr T*  data() const noexcept { return m_coords; }

      constexpr T& __begin(int k) const noexcept { return assert(k < D), m_coords[k]; }
      constexpr T& __end(int k) const noexcept { return assert(k < D), m_coords[D + k]; }

      constexpr ndpointref<D, T> tl() const noexcept { return {m_coords}; }
      constexpr ndpointref<D, T> br() const noexcept { return {m_coords + D}; }

      constexpr auto shape() const noexcept { return tl() - br(); }
      constexpr auto extents() const noexcept
      {
        auto pmin = tl();
        auto pmax = br();
        auto ret  = std::array<std::size_t, D>{};
        for (int i = 0; i < D; i++)
          ret[i] = pmax[i] - pmin[i];
        return ret;
      }

      typename base::cursor          begin_cursor() const { return {m_coords, m_coords + D, false}; }
      typename base::backward_cursor rbegin_cursor() const { return {m_coords, m_coords + D, true}; }

      T* m_coords;
    };

    /// Reference to box wrapper
    template <class T>
    struct _bref<-1, T>
    {
      static constexpr int ndim = -1;
      using coord_type          = T;

      template <class Impl>
      constexpr _bref(const _box<Impl>& b) noexcept
        : m_dim{b.dim()}
        , m_coords(b.data())
      {
      }

      constexpr int dim() const noexcept { return m_dim; }
      constexpr T*  data() const noexcept { return m_coords; }

      constexpr T& __begin(int k) const noexcept
      {
        assert(k < m_dim);
        return m_coords[k];
      }
      constexpr T& __end(int k) const noexcept
      {
        assert(k < m_dim);
        return m_coords[m_dim + k];
      }
      constexpr ConstPointRef tl() const noexcept { return {m_dim, m_coords}; }
      constexpr ConstPointRef br() const noexcept { return {m_dim, m_coords + m_dim}; }

      constexpr auto shape() const noexcept { return tl() - br(); }
      auto           extents() const noexcept
      {
        auto pmin = tl();
        auto pmax = br();
        auto ret  = std::vector<std::size_t>{m_dim, 0};
        for (int i = 0; i < m_dim; i++)
          ret[i] = pmax[i] - pmin[i];
        return ret;
      }

      int begin() const { return 0; }
      int end() const { return 1; }
      void begin_cursor() const {}
      void rbegin_cursor() const {};
      void end_cursor() const {}
      void rend_cursor() const {};
      using cursor = void;
      using backward_cursor = void;



      int m_dim;
      T*  m_coords;
    };

  } // namespace impl

  template <class Impl>
  constexpr void _box<Impl>::inflate(int k) noexcept
  {
    auto&& pmin = this->tl();
    auto&& pmax = this->br();

    bool empty = false;
    for (int i = 0; i < this->dim(); ++i)
    {
      pmin[i] -= k;
      pmax[i] += k;
      if (pmax[i] <= pmin[i])
        empty = true;
    }
    if (empty)
      this->_zeros();
  }

  template <class Impl>
  constexpr int _box<Impl>::size(int k) const noexcept
  {
    assert(k < this->dim() && "Invalid dimension");
    return this->__end(k) - this->__begin(k);
  }

  template <class Impl>
  constexpr std::size_t _box<Impl>::size() const noexcept
  {
    std::size_t sz = 1;
    for (int k = 0; k < this->dim(); ++k)
      sz *= (this->__end(k) - this->__begin(k));
    if (sz < 0)
      sz = 0;
    return sz;
  }

  template <class Impl>
  constexpr bool _box<Impl>::empty() const noexcept
  {
    for (int k = 0; k < this->dim(); ++k)
      if (this->__begin(k) == 0 && this->__end(k) == 0)
        return true;
    return this->dim() == 0;
  }


  template <class Impl>
  template <class _p>
  constexpr bool _box<Impl>::has(_point<_p> x) const noexcept
  {
    assert(x.dim() == this->dim() && "Point dimension mismatch.");

    for (int k = 0; k < this->dim(); ++k)
      if (x[k] < this->__begin(k) || x[k] >= this->__end(k))
        return false;
    return true;
  }

  template <class Impl>
  template <class _i>
  constexpr bool _box<Impl>::operator==(const _box<_i>& B) const noexcept
  {
    assert(this->dim() == B.dim() && "Boxes must be of the same dimensions");

    for (int k = 0; k < this->dim(); ++k)
      if (B.__begin(k) != this->__begin(k) || B.__end(k) != this->__end(k))
        return false;
    return true;
  }

  template <class Impl>
  template <class _i>
  constexpr bool _box<Impl>::operator!=(const _box<_i>& B) const noexcept
  {
    return !(*this == B);
  }


  template <class Impl>
  template <class _i>
  constexpr bool _box<Impl>::includes(const _box<_i>& B) const noexcept
  {
    assert(this->dim() == B.dim() && "Boxes must be of the same dimensions");

    for (int k = 0; k < this->dim(); ++k)
      if (B.__begin(k) < this->__begin(k) || B.__end(k) > this->__end(k))
        return false;
    return true;
  }

  template <class Impl>
  template <class _i>
  constexpr bool _box<Impl>::intersects(const _box<_i>& B) const noexcept
  {
    assert(this->dim() == B.dim() && "Boxes must be of the same dimensions");

    for (int k = 0; k < this->dim(); ++k)
    {
      bool disjoint = this->__end(k) <= B.__begin(k) || B.__end(k) <= this->__begin(k);
      if (disjoint)
        return false;
    }
    return true;
  }

  template <class Impl>
  template <class _i>
  constexpr void _box<Impl>::clip(const _box<_i>& B) noexcept
  {
    assert(this->dim() == B.dim() && "Boxes must be of the same dimensions");

    bool empty = false;
    for (int k = 0; k < this->dim(); ++k)
    {
      if (this->__begin(k) < B.__begin(k))
        this->__begin(k) = B.__begin(k);
      if (this->__end(k) > B.__end(k))
        this->__end(k) = B.__end(k);
      if (this->__end(k) <= this->__begin(k))
      {
        empty = true;
        break;
      }
    }
    if (empty)
      this->_zeros();
  }

  template <class Impl>
  template <class _p>
  auto _box<Impl>::clamp(_point<_p> x) const noexcept -> point_type
  {
    assert(this->dim() == x.dim() && "Objects must be of the same dimensions");
    assert(!this->empty() && "Cannot clamp to an empty box.");

    point_type q;
    for (int k = 0; k < this->dim(); ++k)
      q[k] = std::clamp(x[k], this->__begin(k), this->__end(k) - 1);
    return q;
  }

  template <class Impl>
  template <class _p>
  auto _box<Impl>::mirror(_point<_p> x) const noexcept -> point_type
  {
    assert(this->dim() == x.dim() && "Objects must be of the same dimensions");
    assert(!this->empty() && "Cannot clamp to an empty box.");

    point_type q;
    for (int k = 0; k < this->dim(); ++k)
    {
      auto a = this->__begin(k);
      auto b = this->__end(k);
      auto n = b - a;
      auto s = 2 * n;
      auto v = x[k] - a;
      v      = v % s;
      v      = v < 0 ? v + s : v; // ensure v>0
      v      = v < n ? v : s - v - 1;
      q[k]   = a + v;
    }
    return q;
  }

  template <class Impl>
  template <class _p>
  auto _box<Impl>::periodize(_point<_p> x) const noexcept -> point_type
  {
    assert(this->dim() == x.dim() && "Objects must be of the same dimensions");
    assert(!this->empty() && "Cannot clamp to an empty box.");

    point_type q;
    for (int k = 0; k < this->dim(); ++k)
    {
      auto a = this->__begin(k);
      auto b = this->__end(k);
      auto n = b - a;
      auto v = x[k] - a;
      v      = v % n;
      v      = v < 0 ? v + n : v;
      q[k]   = a + (v % n);
    }
    return q;
  }

} // namespace mln::experimental


#ifdef PYLENE_CONCEPT_TS_ENABLED
// Specialization of std::common_reference
namespace concepts
{
  template <class UImpl, class VImpl, template <class> class TQual, template <class> class UQual>
  struct basic_common_reference<mln::experimental::_box<UImpl>, mln::experimental::_box<VImpl>, TQual, UQual>
  {
    static_assert((UImpl::ndim == -1) || (VImpl::ndim == -1) || (UImpl::ndim == UImpl::ndim),
                  "Incompatible number of dimensions.");
    using type = mln::experimental::ndbox<(UImpl::ndim == VImpl::ndim) ? UImpl::ndim : -1>;
  };
}
#endif

namespace ranges
{
  template <typename Impl>
  struct range_cardinality<mln::experimental::_box<Impl>, void> :
    std::integral_constant<cardinality, finite>
  {
  };
}

