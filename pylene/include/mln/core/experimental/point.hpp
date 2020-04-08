#pragma once
#include <cassert>
#include <type_traits>
#include <algorithm>

#include <concepts/type_traits.hpp>

#if 0
#include <stl2/type_traits.hpp>
#endif


namespace mln::experimental
{
  // Forward declaration
  template <class Impl>
  class _point;

  namespace impl
  {
    template <int Dim, class T>
    struct pcontainer;

    template <int Dim_, class T>
    struct pref;
  }

  /******************************************/
  /****        Point Type Aliases        ****/
  /******************************************/

  /// Point type having value semantics
  template <int dim, class T = int>
  using ndpoint = _point<impl::pcontainer<dim, T>>;

  /// Point type having value semantics with dynamic number of dimension (up to 4 coordinates of type \p int)
  using Point   = ndpoint<-1>;

  /// Point types having value semantics with compile-time number of dims.
  /// \{
  using point1d = ndpoint<1>;
  using point2d = ndpoint<2>;
  using point3d = ndpoint<3>;
  /// \}

  /// Point type having reference semantics
  template <int dim, class T = int>
  using ndpointref = _point<impl::pref<dim, T>>;

  /// Point type having reference semantics with dynamic number of dimension (up to 4 coordinates of type \p int)
  /// \{
  using PointRef      = ndpointref<-1, int>;
  using ConstPointRef = ndpointref<-1, const int>;
  /// \}

  /// Point types having value semantics with compile-time number of dims.
  /// \{
  using point1d_ref = ndpointref<1, int>;
  using const_point1d_ref = ndpointref<1, const int>;
  using point2d_ref = ndpointref<2, int>;
  using const_point2d_ref = ndpointref<2, const int>;
  using point3d_ref = ndpointref<3, int>;
  using const_point3d_ref = ndpointref<3, const int>;
  /// \}

  /******************************************/
  /****      Point Types Interface      ****/
  /******************************************/

  namespace detail
  {
    constexpr bool have_compatible_dim(int d1, int d2) { return d1 == -1 || d2 == -1 || d1 == d2; }
  }

  template <class Impl>
  class _point : private Impl
  {
  private:
    template <int, class> friend struct impl::pref;
    template <int, class> friend struct impl::pcontainer;
    template <class U> friend class _point;
    using Impl::has_value_semantic;
    using T = typename Impl::value_type;

  public:
    using Impl::dim;
    using Impl::data;
    using Impl::ndim;
    using typename Impl::value_type;

    // Constructors
    // \{
    _point() = default;
    _point(const _point&) = default;

    // Special case for dynamic coord
    template <int d = Impl::ndim, class = std::enable_if_t<d == -1 && has_value_semantic>>
    _point(int dim)
      : Impl(dim)
    {
    }

    // From a span of value
    constexpr _point(int dim, T* values) noexcept
      : Impl(dim, values)
    {
    }


    // From initializer list
    constexpr _point(const std::initializer_list<T>& values) noexcept
      : Impl(values)
    {
    }

    // Converting constructeur
    template <class I2, class = std::enable_if_t<detail::have_compatible_dim(ndim, I2::ndim)>>
    constexpr _point(const _point<I2>& other) noexcept
      : Impl(other)
    {
    }


    // Cannot use inherited constructors because of
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91718
    /*
    template <class... Args>
    explicit _point(Args&&... args)
      : Impl(std::forward<Args>(args)...)
    {
    }
    */
    // \}

    constexpr T  at(int k) const noexcept { assert(k < dim()); return this->data(k); }
    constexpr T& at(int k) noexcept { assert(k < dim()); return this->data(k); }

    constexpr T  operator[](int k) const noexcept { assert(k < dim()); return this->data(k); }
    constexpr T& operator[](int k) noexcept { assert(k < dim()); return this->data(k); }
    constexpr int size() const noexcept { return dim(); }


    constexpr T x() const noexcept { assert(dim() >= 1); return this->data(0); }
    constexpr T& x() noexcept { assert(dim() >= 1);  return this->data(0); }

    template <int d = Impl::ndim, class = std::enable_if_t<d == -1 || d >= 2>>
    constexpr T y() const noexcept { assert(dim() >= 2);  return this->data(1); }

    template <int d = Impl::ndim, class = std::enable_if_t<d == -1 || d >= 2>>
    constexpr T& y() noexcept { assert(dim() >= 2);  return this->data(1); }

    template <int d = Impl::ndim, class = std::enable_if_t<d == -1 || d >= 3>>
    constexpr T z() const noexcept { assert(dim() >= 3);  return this->data(2); }

    template <int d = Impl::ndim, class = std::enable_if_t<d == -1 || d >= 3>>
    constexpr T& z() noexcept { assert(dim() >= 3);  return this->data(2); }

    template <class _i>
    constexpr bool operator== (const _point<_i>& other) const noexcept;

    template <class _i>
    constexpr bool operator< (const _point<_i>& other) const noexcept;

    template <class _i>
    constexpr bool operator<=(const _point<_i>& other) const noexcept;

    template <class _i>
    constexpr bool operator!=(const _point<_i>& other) const noexcept;

    template <class _i>
    constexpr bool operator>=(const _point<_i>& other) const noexcept;

    template <class _i>
    constexpr bool operator>(const _point<_i>& other) const noexcept;

    template <class _i>
    constexpr _point& operator+= (const _point<_i>& other) noexcept;

    template <class _i>
    constexpr _point& operator-=(const _point<_i>& other) noexcept;

  };

  /*
  template <class I1, class I2>
  auto operator+(_point<I1> a, _point<I2> b) noexcept
    -> _point<impl::pcontainer<std::max(I1::ndim, I2::ndim), decltype(a[0] + b[0])>>;

  template <class I1, class I2>
  auto operator-(_point<I1> a, _point<I2> b) noexcept
    -> _point<impl::pcontainer<std::max(I1::ndim, I2::ndim), decltype(a[0] + b[0])>>;
  */

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {


    template <int Dim, class T>
    struct pcontainer
    {
      template <class> friend class _point;


      pcontainer() = default;
      pcontainer(const pcontainer& other) = default;

      // From a span
      constexpr pcontainer(int dim, const T* data) noexcept
      {
        assert(dim == Dim && "Point dimensions mistmatch.");
        for (int i = 0; i < Dim; ++i)
          m_data[i] = data[i];
      }

      constexpr pcontainer(const std::initializer_list<T>& values) noexcept
      {
        assert(values.size() == Dim && "Point dimensions mistmatch.");
        auto buffer = values.begin();
        for (int i = 0; i < Dim; ++i)
          m_data[i] = buffer[i];
      }

      // Converting constructors
      template <class Impl>
      constexpr pcontainer(const _point<Impl>& other) noexcept
      {
        static_assert(std::is_convertible_v<typename Impl::value_type, T>);
        assert(other.dim() == Dim && "Point dimensions mistmatch.");
        for (int i = 0; i < Dim; ++i)
          m_data[i] = other[i];
      }


      static constexpr int dim() noexcept { return Dim; }
      constexpr T*         data() noexcept { return m_data; }
      constexpr const T*   data() const noexcept { return m_data; }
      constexpr T&         data(int k) noexcept { return m_data[k]; }
      constexpr T          data(int k) const noexcept { return m_data[k]; }


      using value_type                         = T;
      static constexpr bool has_value_semantic = true;
      static constexpr int  ndim               = Dim;

      T m_data[Dim] = {};
    };

    template <class T>
    struct pcontainer<-1, T>
    {
      template <class> friend class _point;

      pcontainer() = default;

      pcontainer(int dim) noexcept
        : m_dim{dim}
      {
      }

      // From a span
      constexpr pcontainer(int dim, const T* data)
      {
        assert(dim == m_dim && "Point dimensions mistmatch.");
        for (int i = 0; i < dim; ++i)
          m_data[i] = data[i];
      }


      constexpr pcontainer(const std::initializer_list<T>& coords) noexcept
        : m_dim{static_cast<int>(coords.size())}
        , m_data{
              (coords.size() >= 1) ? *(coords.begin() + 0) : 0,
              (coords.size() >= 2) ? *(coords.begin() + 1) : 0,
              (coords.size() >= 3) ? *(coords.begin() + 2) : 0,
              (coords.size() >= 4) ? *(coords.begin() + 3) : 0,
          }
      {
        assert(coords.size() <= 4 && "The number of dimensions is limited to 4.");
      }

      // Converting constructors
      template <class Impl, class = std::enable_if<std::is_convertible_v<typename Impl::value_type, T>>>
      constexpr pcontainer(const _point<Impl>& other) noexcept
        : m_dim(other.dim())
      {
        assert(other.dim() <= 4 && "The number of dimensions is limited to 4.");
        for (int i = 0; i < m_dim; ++i)
          m_data[i] = other[i];
      }



      constexpr int      dim() const noexcept { return m_dim; }
      constexpr T*       data() noexcept { return m_data; }
      constexpr const T* data() const noexcept { return m_data; }
      constexpr T&       data(int k) noexcept { return m_data[k]; }
      constexpr T        data(int k) const noexcept { return m_data[k]; }

      using value_type                         = T;
      static constexpr bool has_value_semantic = true;
      static constexpr int  ndim               = -1;

      int m_dim;
      T m_data[4];
    };

    template <int Dim_, class T>
    struct pref
    {
      template <class> friend class _point;

      pref() noexcept = default;

      constexpr pref(T* data) noexcept : m_data(data){}

      // Cannot create from a temporary
      template <int d, class U>
      constexpr pref(_point<pcontainer<d, U>>&& p) noexcept = delete;

      template <class _i, class = std::enable_if_t<std::is_convertible_v<typename _i::value_type const*, T*>>>
      constexpr pref(const _point<_i>& p) noexcept
        : m_data{p.data()}
      {
        assert(p.dim() == Dim_);
      }

      template <class _i, class = std::enable_if_t<std::is_convertible_v<typename _i::value_type*, T*>>>
      constexpr pref(_point<_i>& p) noexcept
        : m_data{p.data()}
      {
        assert(p.dim() == Dim_);
      }


      static constexpr int dim() noexcept { return Dim_; }
      constexpr T*         data() const noexcept { return m_data; }
      constexpr T&         data(int k) const noexcept { return m_data[k]; }

      using value_type                         = T;
      static constexpr bool has_value_semantic = false;
      static constexpr int  ndim               = Dim_;

      T* m_data;
    };

    template <class T>
    struct pref<-1, T>
    {
      template <class> friend class _point;

      pref() noexcept = default;

      template <class _i, class = std::enable_if_t<std::is_convertible_v<typename _i::value_type const *, T*>>>
      constexpr pref(const _point<_i>& p) noexcept
        : m_dim{p.dim()}
        , m_data{p.data()}
      {
      }

      template <class _i, class = std::enable_if_t<std::is_convertible_v<typename _i::value_type*, T*>>>
      constexpr pref(_point<_i>& p) noexcept
        : m_dim{p.dim()}
        , m_data{p.data()}
      {
      }


      template <int dim>
      constexpr pref(const _point<pref<dim, T>>& p) noexcept
        : m_dim{p.dim()}
        , m_data{p.data()}
      {
      }

      constexpr pref(const std::initializer_list<T>& coords)
        : m_dim{static_cast<int>(coords.size())}
        , m_data{coords.begin()}
      {
      }

      constexpr pref(int dim, T* data) noexcept : m_dim(dim), m_data(data){}

      constexpr int dim() const noexcept { return m_dim; }
      constexpr T*  data() const noexcept { return m_data; }
      constexpr T&  data(int k) const noexcept { return m_data[k]; }

      using value_type                         = T;
      static constexpr bool has_value_semantic = false;
      static constexpr int  ndim               = -1;

      int m_dim;
      T* m_data;
    };

  } // namespace impl


  template <class Impl>
  template <class _i>
  constexpr bool _point<Impl>::operator==(const _point<_i>& other) const noexcept
  {
    assert(dim() == other.dim() && "Point dimensions mistmatch.");
    for (int k = 0; k < dim(); ++k)
      if (!(this->data(k) == other.data(k)))
        return false;
    return true;
  }

  template <class Impl>
  template <class _i>
  constexpr bool _point<Impl>::operator<(const _point<_i>& other) const noexcept
  {
    assert(dim() == other.dim() && "Point dimensions mistmatch.");
    for (int k = dim() - 1; k >= 0; --k)
      if (this->data(k) < other.data(k))
        return true;
      else if (this->data(k) > other.data(k))
        return false;
    return false;
  }

  template <class Impl>
  template <class _i>
  constexpr bool _point<Impl>::operator<=(const _point<_i>& other) const noexcept
  {
    assert(dim() == other.dim() && "Point dimensions mistmatch.");
    for (int k = dim() - 1; k >= 0; --k)
      if (this->data(k) < other.data(k))
        return true;
      else if (this->data(k) > other.data(k))
        return false;
    return true;
  }

  template <class Impl>
  template <class _i>
  constexpr bool _point<Impl>::operator!=(const _point<_i>& other) const noexcept
  {
    return !(*this == other);
  }


  template <class Impl>
  template <class _i>
  constexpr bool _point<Impl>::operator>=(const _point<_i>& other) const noexcept
  {
    return !(*this < other);
  }

  template <class Impl>
  template <class _i>
  constexpr bool _point<Impl>::operator>(const _point<_i>& other) const noexcept
  {
    return !(*this <= other);
  }

  template <class Impl>
  template <class _i>
  constexpr auto _point<Impl>::operator+=(const _point<_i>& other) noexcept -> _point&
  {
    assert(dim() == other.dim() && "Point dimensions mistmatch.");
    for (int k = 0; k < dim(); ++k)
      this->data(k) += other.data(k);
    return *this;
  }

  template <class Impl>
  template <class _i>
  constexpr auto _point<Impl>::operator-=(const _point<_i>& other) noexcept -> _point&
  {
    assert(dim() == other.dim() && "Point dimensions mistmatch.");
    for (int k = 0; k < dim(); ++k)
      this->data(k) -= other.data(k);
    return *this;
  }

  template <class Impl, class = std::enable_if_t<Impl::ndim == -1>>
  auto operator-(_point<Impl> a) noexcept -> ndpoint<-1, decltype(-a[0])>
  {
    using R = decltype(-a[0]);
    ndpoint<-1, R> c(a.dim());

    for (int k = 0; k < a.dim(); ++k)
      c[k] = -a[k];
    return c;
  }

  template <class Impl, class = std::enable_if_t<Impl::ndim != -1>>
  auto operator-(_point<Impl> a) noexcept -> ndpoint<Impl::ndim, decltype(-a[0])>
  {
    using R = decltype(-a[0]);
    ndpoint<Impl::ndim, R> c;

    for (int k = 0; k < a.dim(); ++k)
      c[k] = -a[k];
    return c;
  }

  template <class Impl, class U, class = std::enable_if_t<Impl::ndim != -1>>
  auto operator*(U x, _point<Impl> a) noexcept -> ndpoint<Impl::ndim, decltype(x * a[0])>
  {
    using R = decltype(x * a[0]);
    ndpoint<Impl::ndim, R> c;

    for (int k = 0; k < a.dim(); ++k)
      c[k] = x * a[k];
    return c;
  }

  template <class Impl, class U, class = std::enable_if_t<Impl::ndim != -1>>
  auto operator*(_point<Impl> a, U x) noexcept -> ndpoint<Impl::ndim, decltype(a[0] * x)>
  {
    using R = decltype(x * a[0]);
    ndpoint<Impl::ndim, R> c;

    for (int k = 0; k < a.dim(); ++k)
      c[k] = a[k] * x;
    return c;
  }



  template <class I1, class I2, class = std::enable_if_t<I1::ndim == -1 && I2::ndim == -1>>
  auto operator+(_point<I1> a, _point<I2> b) noexcept -> ndpoint<-1, decltype(a[0] + b[0])>
  {
    assert(a.dim() == b.dim() && "Point dimensions mistmatch.");

    using R = decltype(a[0] + b[0]);
    ndpoint<-1, R> c(a.dim());

    for (int k = 0; k < a.dim(); ++k)
      c[k] = a[k] + b[k];
    return c;
  }

  template <class I1, class I2, class = std::enable_if_t<I1::ndim != -1 || I2::ndim != -1>>
  auto operator+(_point<I1> a, _point<I2> b) noexcept -> ndpoint<std::max(I1::ndim, I2::ndim), decltype(a[0] + b[0])>
  {
    assert(a.dim() == b.dim() && "Point dimensions mistmatch.");

    using P = ndpoint<std::max(I1::ndim, I2::ndim), decltype(a[0] + b[0])>;
    P c;

    for (int k = 0; k < P::ndim; ++k)
      c[k] = a[k] + b[k];
    return c;
  }

  template <class I1, class I2, class = std::enable_if_t<I1::ndim == -1 && I2::ndim == -1>>
  auto operator-(_point<I1> a, _point<I2> b) noexcept -> ndpoint<-1, decltype(a[0] - b[0])>
  {
    assert(a.dim() == b.dim() && "Point dimensions mistmatch.");

    using R = decltype(a[0] - b[0]);
    ndpoint<-1, R> c(a.dim());

    for (int k = 0; k < a.dim(); ++k)
      c[k] = a[k] - b[k];
    return c;
  }

  template <class I1, class I2, class = std::enable_if_t<I1::ndim != -1 || I2::ndim != -1>>
  auto operator-(_point<I1> a, _point<I2> b) noexcept -> ndpoint<std::max(I1::ndim, I2::ndim), decltype(a[0] - b[0])>
  {
    assert(a.dim() == b.dim() && "Point dimensions mistmatch.");

    using P = ndpoint<std::max(I1::ndim, I2::ndim), decltype(a[0] - b[0])>;
    P c;

    for (int k = 0; k < P::ndim; ++k)
      c[k] = a[k] - b[k];
    return c;
  }




} // namespace mln::experimental



// Specialization of std::common_reference
namespace concepts
{
  template <class UImpl, class VImpl, template<class> class TQual, template<class> class UQual>
    struct basic_common_reference<mln::experimental::_point<UImpl>, mln::experimental::_point<VImpl>, TQual, UQual>
  {
    static_assert((UImpl::ndim == -1) || (VImpl::ndim == -1) || (UImpl::ndim == UImpl::ndim),
                  "Incompatible number of dimensions.");
    using type = mln::experimental::ndpoint<(UImpl::ndim == VImpl::ndim) ? UImpl::ndim : -1,
                                            common_reference_t<typename UImpl::value_type, typename VImpl::value_type>>;
  };
}
#endif
