#pragma once

#include "ndimage_interface.hpp"
#include <mln/core/image/image.hpp>
#include <mln/core/domain/box.hpp>
#include <memory>


namespace mln
{

  template <int N, class T, layout_t layout, int vdim>
  class __ndimage;

  template <int N, class V, layout_t layout>
  class ndimage;

  template <int N, class V, layout_t layout>
  class ndimage_base;


  namespace details
  {
    template <class T = void>
    struct ndimage_data;
  }

  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <int N, class V, layout_t L, class U>
  struct image_ch_value<ndimage<N, V, L>, U>
  {
    using type = ndimage<N, U, L>;
  };


  /**************************************************************/
  /****      Second level base class: N is known             ****/
  /****      Definition of ndimage<N, void, UNKNOWN, 0>      ****/
  /**************************************************************/

  /// Second level base class
  /// It precises domain-related functionalities
  template <int N>
  class __ndimage<N, void, layout_t::UNKNOWN, 0>
  {
    using domain_type = mln::box<int, N>;

  public:
    using __index_type = std::ptrdiff_t;
    using __point_type = mln::point<std::ptrdiff_t, N>;

    explicit __ndimage(int border)
      : m_border(border)
    {
    }

    __ndimage(const domain_type& domain, int border)
      : m_domain (domain),
        m_virtual_domain(m_domain.pmin - border, m_domain.pmax + border),
        m_border(border)
    {
      __init_strides();
      this->m_first_index = this->index_of_point(this->m_virtual_domain.pmin);
    }

    /// \name Index-related methods
    /// \{
    /// \copydoc image::index_of_point(const point_type&) const
    __index_type index_of_point(const __point_type& p) const
    {
      __index_type index = 0;
      for (int k = 0; k < N; ++k)
        index += p[k] * m_strides[k];
      return index;
    }

    /// \copydoc image::point_at_index(size_type i) const
    __point_type point_at_index(__index_type index) const
    {
      __point_type p = m_virtual_domain.pmin;
      __index_type diff = index - m_first_index;
      for (int i = 0; i < N - 1; ++i)
      {
        auto q = std::div(diff, m_strides[i]);
        p[i] += q.quot;
        diff = q.rem;
      }
      p[N - 1] = diff;
      mln_postcondition(m_virtual_domain.has(p));
      return p;
    }

    /// \copydoc image::delta_index(const point_type&) const
    __index_type delta_index(const __point_type& p) const
    {
      return index_of_point(p);
    }
    /// \}

    const domain_type& domain() const { return m_domain; }

    /// Return the border width of the image
    int border() const { return m_border; }

    /// \brief Inflate (or reduce, if the value is negative) the domain size by
    /// a given amount, by reducing (inflating) the border.
    ///
    /// \note No memory reallocation is performed by the method
    ///
    /// \param[in] delta The domain inflation value. It can be negative.
    /// \precondition -amin(shape() / 2) <= delta <= border
    void inflate_domain(int delta);

    [[deprecated]] void reindex(__index_type) {}

    std::ptrdiff_t stride(int k = 0) const { return this->m_strides[k]; }

  protected:
    void __from_buffer(const domain_type& domain)
    {
      this->m_domain = domain;
      this->m_virtual_domain = domain;
      this->m_border = 0;
      this->__init_strides();
      this->m_first_index = this->index_of_point(this->m_virtual_domain.pmin);
    }

    void __from_buffer(const domain_type& domain, const std::ptrdiff_t strides[])
    {
      this->m_domain = domain;
      this->m_virtual_domain = domain;
      for (int k = 0; k < N; ++k)
        this->m_strides[k] = strides[k];
      this->m_first_index = this->index_of_point(this->m_virtual_domain.pmin);
    }

    void clip(const domain_type& domain, bool extended_by_image)
    {
      m_domain = domain;
      if (!extended_by_image)
      {
        m_virtual_domain = domain;
        m_border = 0;
      }
      m_first_index = this->index_of_point(m_virtual_domain.pmin);
    }

  private:
    void __init_strides()
    {
      auto shp = this->m_virtual_domain.shape();
      this->m_strides[N-1] = 1;
      for (int i = N-2; i >= 0; --i)
        this->m_strides[i] = shp[i+1] * this->m_strides[i+1];
    }

  protected:
    domain_type m_domain = domain_type();
    domain_type m_virtual_domain = domain_type();
    std::ptrdiff_t m_strides[N];

    int m_border; // This is not the real border (just a hint)
    std::ptrdiff_t m_first_index; // The index of m_virtual_domain.pmin (this is a cache variable)
  };

  /*******************************************************/
  /****      Second level base class: T is known      ****/
  /*******************************************************/
  template <class T>
  class __ndimage<0, T, layout_t::UNKNOWN, 0>
  {
  public:
    __ndimage() = default;

    __ndimage(std::size_t size, int nchannels, layout_t layout)
        : m_data(std::make_shared<details::ndimage_data<T>>(size, nchannels, layout))
    {
    }

    __ndimage(std::size_t size, int nchannels, layout_t layout, const T values[])
        : m_data(std::make_shared<details::ndimage_data<T>>(size, nchannels, layout, values))
    {
    }

  protected:
    std::shared_ptr< details::ndimage_data<T> > m_data;
  };


  /*************************************************************/
  /****      Third level base class: N and T are known      ****/
  /*************************************************************/

  template <int N, class T>
  class __ndimage<N, T, layout_t::UNKNOWN, 0> :
    public __ndimage<N, void, layout_t::UNKNOWN, 0>,
    public __ndimage<0, T, layout_t::UNKNOWN, 0>,
    public ImageNdInterface<N, T>
  {
  private:
    using base1 = __ndimage<N, void, layout_t::UNKNOWN, 0>;
    using base2 = __ndimage<0, T, layout_t::UNKNOWN, 0>;
    using interface_t = ImageNdInterface<N, T>;
  public:
    using domain_type = typename interface_t::domain_type;

    /// Constructors
    /// \{
    explicit __ndimage(int border);
    __ndimage(const domain_type& domain, int border, int nchannels, layout_t layout, const T values[]);
    __ndimage(const domain_type& domain, int border, int nbuffers, layout_t layout);
    /// \}


    const domain_type& domain() const final
    {
      return this->m_domain;
    }

    int dim(int k) const final
    {
      mln_precondition(k < N);
      return this->m_domain.pmax[k] - this->m_domain.pmin[k];
    }

    int get_pdim() const final
    {
      return N;
    }

    sample_type_id_t sample_type_id() const final { return sample_type_id_of<T>::value; }

  protected:
    void __from_buffer(const domain_type& domain) { this->base1::__from_buffer(domain); }
    void __from_buffer(const domain_type& domain, std::ptrdiff_t strides[]) { this->base1::__from_buffer(domain, strides); }
    void clip(const domain_type& domain, bool extended_by_image) { this->base1::clip(domain, extended_by_image); }
  };



  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <int N, class V, layout_t layout>
  class ndimage_base : public __ndimage<N, V, layout, 1>
  {
  private:
    using base = __ndimage<N, V, layout, 1>;
  public:
    using base::base;
    enum { vdim = 1 };
  };


  template <int N, class T, layout_t layout, unsigned M, class tag>
  class ndimage_base<N, mln::internal::vec_base<T, M, tag>, layout>  : public __ndimage<N, T, layout, M>
  {
  private:
    using base = __ndimage<N, T, layout, M>;
  public:
    using base::base;
    enum { vdim = M };
  };

  namespace details
  {

    template <class T>
    struct ndimage_data
    {
      ~ndimage_data()
      {
        delete buffer;
      }

      ndimage_data(std::size_t size, int nchannels, layout_t layout)
      {
        (void) layout;
        std::allocator<T> alloc;
        std::size_t alloc_size = size * nchannels;

        buffer = alloc.allocate(alloc_size);
        std::uninitialized_default_construct(buffer, buffer + alloc_size);
      }

      ndimage_data(std::size_t size, int nchannels, layout_t layout, const T values[])
      {
        std::allocator<T> alloc;
        std::size_t alloc_size = size * nchannels;

        buffer = alloc.allocate(alloc_size);

        if (layout == layout_t::PLANAR || nchannels == 1)
        {
          T* ptr = buffer;
          for (int i = 0; i < nchannels; ++i, ptr += size)
            std::uninitialized_fill(ptr, ptr + size, values[i]);
        }
        else // Interleaved
        {
          for (int k = 0; k < nchannels; ++k)
          {
            T val = values[k];
            T* ptr = buffer + k;
            for (std::size_t i = 0; i < size; ++i)
            {
              new (ptr) T(val);
              ptr += nchannels;
            }
          }
        }
      }

      T* buffer;
    };
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::UNKNOWN, 0>::__ndimage(int border) : base1(border)
  {
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::UNKNOWN, 0>::__ndimage(const domain_type& domain, int border, int nchannels, layout_t layout, const T values[])
    : base1(domain, border),
      base2(this->m_virtual_domain.size(), nchannels, layout, values)
  {
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::UNKNOWN, 0>::__ndimage(const domain_type& domain, int border, int nchannels, layout_t layout)
    : base1(domain, border),
      base2(this->m_virtual_domain.size(), nchannels, layout)
  {
  }


  template <int N>
  void __ndimage<N, void, layout_t::UNKNOWN, 0>::inflate_domain(int delta)
  {
    domain_type new_domain;
    new_domain.pmin = m_domain.pmin - delta;
    new_domain.pmax = m_domain.pmax + delta;

    for (unsigned k = 0; k < N; ++k)
    {
      mln_precondition(m_virtual_domain.pmin[k] <= new_domain.pmin[k]);
      mln_precondition(new_domain.pmin[k] <= new_domain.pmax[k]);
      mln_precondition(new_domain.pmax[k] <= m_virtual_domain.pmax[k]);
    }
    m_domain = new_domain;
    m_border -= delta;
  }

}
