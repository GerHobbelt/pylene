#pragma once

#include "ndimage_interface.hpp"
#include "ndimage.hpp"
#include "ndimage_pixel.hpp"
#include "ndimage_iter.hpp"
#include "ndimage_extension.hpp"

namespace mln
{

  template <int N, class V>
  using ndimage_interleaved = ndimage<N, V, layout_t::INTERLEAVED>;

  template <int N, class T, int vdim>
  using __ndimage_interleaved = __ndimage<N, T, layout_t::INTERLEAVED, vdim>;


  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <int N, class V>
  struct image_traits<ndimage_interleaved<N, V>>
  {
    typedef raw_image_tag category;
    typedef std::true_type accessible;
    typedef std::true_type indexable;
    typedef std::true_type concrete;
    typedef std::true_type shallow_copy;
    typedef std::true_type has_border;
    typedef mln::extension::border_extension_tag extension;
  };

  template <int N, class V>
  struct image_concrete<ndimage_interleaved<N, V>>
  {
    using type = ndimage_interleaved<N, V>;
  };

  template <int N, class V, class U>
  struct image_ch_value<ndimage_interleaved<N, V>, U>
  {
    using type = ndimage_interleaved<N, U>;
  };

  /*****************************************************************************************/
  /****              Defintition  of  __ndimage<N, T, layout_t::INTERLEAVED, vdim>      ****/
  /*****************************************************************************************/

  template <int N, class T, int vdim>
  class __ndimage<N, T, layout_t::INTERLEAVED, vdim> : public __ndimage<N, T, layout_t::UNKNOWN, 0>
  {
  private:
    using base = __ndimage<N, T, layout_t::UNKNOWN, 0>;
  public:
    using domain_type = typename base::domain_type;

    // As an Image
    // Constructors
    // \{
    explicit __ndimage(int border);
    __ndimage(const domain_type& domain, int border);
    __ndimage(const domain_type& domain, int border, T values[]);
    // \}

    /// Initialization constructors
    /// \{
    __ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init);
    __ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border);
    __ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, T values[]);
    /// \}


    /// \brief Resize the image to fit \p domain.
    ///
    /// Resize the image w.r.t to \p domain and \p border. The image is
    /// initialized with value  \p v.
    /// \warning The old values are not kept.
    /// \param domain The new domain of the image.
    /// \param border The new border of the image.
    /// \param values The initialization value of the image. If \p is given, values
    /// are copy constructed from \p v, otherwise, they are default-initialized.
    void resize(const domain_type& domain);
    void resize(const domain_type& domain, int border);
    void resize(const domain_type& domain, int border, T values[]);



    T* buffer(int k = 0) final;
    const T* buffer(int k = 0) const final;
    int nbuffers() const final;
    int nchannels() const final;
    layout_t layout() const final;
    std::ptrdiff_t byte_stride(int k = 0) const final;


  protected:
    void __from_buffer(void* buffer, const domain_type& domain);
    void __from_buffer(void* buffer, const domain_type& domain, const std::size_t strides[]);


  private:
    void __init(T* buffer);

  protected:
    std::uintptr_t m_ptr = 0; // Pointer adress to origin (0,0). The pointer adress may not be legal
  };

  /*****************************************************************************************/
  /****              Defintition  of  ndimage<N, V, layout_t::INTERLEAVED, E>           ****/
  /*****************************************************************************************/


  template <int N, class V>
  class ndimage<N, V, layout_t::INTERLEAVED> :
    public ndimage_base<N, V, layout_t::INTERLEAVED>,
    public Image<ndimage<N, V, layout_t::INTERLEAVED>>
  {
  private:
    using __point_type = mln::point<std::ptrdiff_t, N>;
    using __index_type = std::ptrdiff_t;
    using base = ndimage_base<N, V, layout_t::INTERLEAVED>;
    using this_type = ndimage<N, V, layout_t::INTERLEAVED>;

  protected:
    using sample_t = typename base::sample_t;

  public:
    using domain_type = typename base::domain_type;

    /// \copydoc image::pixel_type
    using pixel_type = details::ndimage_pixel<N, V, this_type>;

    /// \copydoc image::const_pixel_type
    using const_pixel_type = details::ndimage_pixel<N, const V, const this_type>;

    /// \copydoc image::value_type
    using value_type = V;

    /// \copydoc image::reference
    using reference = V&;

    /// \copydoc image::const_reference
    using const_reference = const V&;

    /// \copydoc image::size_type
    using index_type = int;

    // Extension
    using extension_type = details::ndimage_extension<N, V>;
    /// \}


    /// \name Image Ranges Types
    /// \{
    friend class details::ndimage_value_range<N, V, this_type>;
    friend class details::ndimage_value_range<N, const V, const this_type>;
    friend class details::ndimage_pixel_range<N, V, this_type>;
    friend class details::ndimage_pixel_range<N, const V, const this_type>;


    /// \copydoc image::value_range
    using value_range = details::ndimage_value_range<N, V, this_type>;

    /// \copydoc image::const_value_range
    using const_value_range = details::ndimage_value_range<N, const V, const this_type>;

    /// \copydoc image::pixel_range
    using pixel_range = details::ndimage_pixel_range<N, V, this_type>;

    /// \copydoc image::const_pixel_range
    using const_pixel_range = details::ndimage_pixel_range<N, const V, const this_type>;
    /// \}

    enum
    {
      ndim = N // Deprecated
    };

    /// As an Image
    /// Constructors
    /// \{
    explicit ndimage(int border = 3);
    explicit ndimage(const domain_type& domain, int border = 3);
    ndimage(const domain_type& domain, int border, V init);
    /// \}

    /// Initialization constructors
    /// \{
    ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init);
    ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border);
    ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, V init);
    // \}

    /// \brief Constructors from external sources
    /// \{
    static ndimage from_buffer(void* buffer, const domain_type& domain);

    static ndimage from_buffer(void* buffer, const domain_type& domain, const std::size_t byte_strides[]);
    /// \}

    /// \name Accession Operators
    /// \{

    /// \copydoc image::operator()(const site_type& p) const
    reference operator()(const __point_type& p);

    /// \copydoc image::operator()(const site_type& p) const
    const_reference operator()(const __point_type& p) const;

    /// \copydoc image::operator[](size_type i) const
    reference operator[](__index_type i);

    /// \copydoc image::operator[](size_type i) const
    const_reference operator[](__index_type i) const;

    /// \copydoc image::at(const site_type& p) const
    reference at(const __point_type& p);

    /// \copydoc image::at(const site_type& p) const
    const_reference at(const __point_type& p) const;
    /// \}

    /// \name Pixel utilities
    /// \{

    /// \copydoc image::pixel_at(const point_type&) const
    pixel_type pixel_at(const __point_type& p);

    /// \copydoc image::pixel_at(const point_type&) const
    const_pixel_type pixel_at(const __point_type& p) const;

    /// \copydoc image::pixel_at_index(size_type) const
    pixel_type pixel_at_index(__index_type i);

    /// \copydoc image::pixel_at_index(size_type) const
    const_pixel_type pixel_at_index(__index_type i) const;

    /// \copydoc image::pixel(const point_type&) const
    pixel_type pixel(const __point_type& p);

    /// \copydoc image::pixel(const point_type&) const
    const_pixel_type pixel(const __point_type& p) const;
    /// \}


    /// \name Image Ranges
    /// \{
    /// \copydoc image::values()
    value_range values();

    /// \copydoc image::values() const
    const_value_range values() const;

    /// \copydoc image::pixels()
    pixel_range pixels();

    /// \copydoc image::pixels() const
    const_pixel_range pixels() const;
    /// \}


    void resize(const domain_type& domain);
    void resize(const domain_type& domain, int border);
    void resize(const domain_type& domain, int border, V init);


    /// Return the extension of the image
    extension_type extension() const;


  };


  /*****************************************************************************************/
  /****          Implementation  of   __ndimage<N, T, layout_t::INTERLEAVED, vdim>      ****/
  /*****************************************************************************************/

    template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__ndimage(int border)
    : base(border)
  {
    // Do not initialize (just m_border is set)
  }

    template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__ndimage(const domain_type& domain, int border)
    : base(domain, border, vdim, layout_t::INTERLEAVED)
  {
    __init(this->m_data->buffer);
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__ndimage(const domain_type& domain, int border, T values[])
    : base(domain, border, vdim, layout_t::INTERLEAVED, values)
  {
    __init(this->m_data->buffer);
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init)
    : base(other.domain(), other.border(), vdim, layout_t::INTERLEAVED)
  {
    __init(this->m_data->buffer);
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border)
    : base(other.domain(), border, vdim, layout_t::INTERLEAVED)
  {
    __init(this->m_data->buffer);
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, T values[])
    : base(other.domain(), border, vdim, layout_t::INTERLEAVED, values)
  {
    __init(this->m_data->buffer);
  }


  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__init(T* buffer)
  {
    auto delta_origin = - this->m_virtual_domain.pmin;
    std::ptrdiff_t diff_origin = 0;

    for (int k = 0; k < N; ++k)
      diff_origin += this->m_strides[k] * delta_origin[k] * vdim;

    m_ptr = reinterpret_cast<std::uintptr_t>(buffer) + (diff_origin * sizeof(T));
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__from_buffer(void* buffer, const domain_type& domain)
  {
    this->base::__from_buffer(domain);
    this->__init(static_cast<T*>(buffer));
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::INTERLEAVED, vdim>::__from_buffer(void* buffer, const domain_type& domain, const std::size_t* byte_strides)
  {
    std::ptrdiff_t strides[N];
    for (int k = 0; k < N; ++k)
    {
      mln_assertion((byte_strides[k] % (sizeof(T) * vdim) == 0) && "Illegal strides. They must be multiple of the sizeof(T[vdim]).");
      strides[k] = static_cast<std::ptrdiff_t>(byte_strides[k] / (sizeof(T) * vdim));
    }

    this->base::__from_buffer(domain, strides);
    this->__init(static_cast<T*>(buffer));
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::INTERLEAVED, vdim>::resize(const domain_type& domain)
  {
    this->resize(domain, this->border());
  }


  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::INTERLEAVED, vdim>::resize(const domain_type& domain, int border)
  {
    // Note: optimize this as a no-op if the buffer is already allocated and large enough
    __ndimage f(domain, border);
    *this = std::move(f);
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::INTERLEAVED, vdim>::resize(const domain_type& domain, int border, T values[])
  {
    // Note: optimize this as a no-op if the buffer is already allocated and large enough
    __ndimage f(domain, border, values);
    *this = std::move(f);
  }

  template <int N, class T, int vdim>
  T* __ndimage<N, T, layout_t::INTERLEAVED, vdim>::buffer(int k)
  {
    (void)k;
    mln_precondition(k == 0);
    auto idx = this->index_of_point(this->m_domain.pmin);
    return ptr_offset<T>(m_ptr, (sizeof(T) * vdim) * idx);
  }

  template <int N, class T, int vdim>
  const T* __ndimage<N, T, layout_t::INTERLEAVED, vdim>::buffer(int k) const
  {
    (void)k;
    mln_precondition(k == 0);
    auto idx = this->index_of_point(this->m_domain.pmin);
    return ptr_offset<const T>(m_ptr, (sizeof(T) * vdim) * idx);
  }

  template <int N, class T, int vdim>
  int __ndimage<N, T, layout_t::INTERLEAVED, vdim>::nchannels() const
  {
    return vdim;
  }

  template <int N, class T, int vdim>
  int __ndimage<N, T, layout_t::INTERLEAVED, vdim>::nbuffers() const
  {
    return 1;
  }

  template <int N, class T, int vdim>
  std::ptrdiff_t __ndimage<N, T, layout_t::INTERLEAVED, vdim>::byte_stride(int k) const
  {
    mln_precondition(k < N);
    return this->m_strides[k] * sizeof(T) * vdim;
  }

  template <int N, class T, int vdim>
  layout_t __ndimage<N, T, layout_t::INTERLEAVED, vdim>::layout() const
  {
    return layout_t::INTERLEAVED;
  }

  /*****************************************************************************************/
  /****          Implementation  of  ndimage<N, V, layout_t::INTERLEAVED>               ****/
  /*****************************************************************************************/


  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>::ndimage(int border)
    : base(border)
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>::ndimage(const domain_type& domain, int border)
  : base(domain, border)
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>::ndimage(const domain_type& domain, int border, V init)
    : base(domain, border, reinterpret_cast<sample_t*>(&init))
  {
  }
  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>::ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init)
    : base(other, mln::init())
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>::ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border)
    : base(other, border)
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>::ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, V init)
    : base(other, border, reinterpret_cast<sample_t*>(&init))
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>
  ndimage<N, V, layout_t::INTERLEAVED>::from_buffer(void* buffer, const domain_type& domain)
  {
    ndimage output;
    output.__from_buffer(buffer, domain);
    return output;
  }

  template <int N, class V>
  ndimage<N, V, layout_t::INTERLEAVED>
  ndimage<N, V, layout_t::INTERLEAVED>::from_buffer(void* buffer, const domain_type& domain, const std::size_t byte_strides[])
  {
    ndimage output;
    output.__from_buffer(buffer, domain, byte_strides);
    return output;
  }

  template <int N, class V>
  void ndimage<N, V, layout_t::INTERLEAVED>::resize(const domain_type& domain,  int border, V init)
  {
    base::resize(domain, border, reinterpret_cast<sample_t*>(&init));
  }

  template <int N, class V>
  void ndimage<N, V, layout_t::INTERLEAVED>::resize(const domain_type& domain, int border)
  {
    base::resize(domain, border);
  }

  template <int N, class V>
  void ndimage<N, V, layout_t::INTERLEAVED>::resize(const domain_type& domain)
  {
    base::resize(domain);
  }

  template <int N, class V>
  V& ndimage<N, V, layout_t::INTERLEAVED>::operator[] (__index_type i)
  {
    return *(ptr_offset<V>(this->m_ptr, sizeof(V) * i));
  }

  template <int N, class V>
  const V& ndimage<N, V, layout_t::INTERLEAVED>::operator[] (__index_type i) const
  {
    return *(ptr_offset<V>(this->m_ptr, sizeof(V) * i));
  }

  template <int N, class V>
  V& ndimage<N, V, layout_t::INTERLEAVED>::operator() (const __point_type& p)
  {
    mln_precondition(this->m_domain.has(p));
    return this->at(p);
  }

  template <int N, class V>
  const V& ndimage<N, V, layout_t::INTERLEAVED>::operator() (const __point_type& p) const
  {
    mln_precondition(this->m_domain.has(p));
    return this->at(p);
  }

    template <int N, class V>
  V& ndimage<N, V, layout_t::INTERLEAVED>::at(const __point_type& p)
  {
    auto i = this->index_of_point(p);
    return (*this)[i];
  }

  template <int N, class V>
  const V& ndimage<N, V, layout_t::INTERLEAVED>::at(const __point_type& p) const
  {
    auto i = this->index_of_point(p);
    return (*this)[i];
  }


  template <int N, class V>
  details::ndimage_pixel<N, V, ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixel_at(const __point_type& p)
  {
    pixel_type pix(this->exact());
    pix.m_ptr = this->m_ptr;
    pix.m_point = p;
    pix.m_index = this->index_of_point(p);
    return pix;
  }

  template <int N, class V>
  details::ndimage_pixel<N, const V, const ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixel_at(const __point_type& p) const
  {
    const_pixel_type pix(this->exact());
    pix.m_ptr = this->m_ptr;
    pix.m_point = p;
    pix.m_index = this->index_of_point(p);
    return pix;
  }

  template <int N, class V>
  details::ndimage_pixel<N, V, ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixel_at_index(__index_type i)
  {
    pixel_type pix(this->exact());
    pix.m_ptr = this->m_ptr;
    pix.m_point = this->point_at_index(i);
    pix.m_index = i;
    return pix;
  }

  template <int N, class V>
  details::ndimage_pixel<N, const V, const ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixel_at_index(__index_type i) const
  {
    const_pixel_type pix(this->exact());
    pix.m_ptr = this->m_ptr;
    pix.m_point = this->point_at_index(i);
    pix.m_index = i;
    return pix;
  }


  template <int N, class V>
  details::ndimage_pixel<N, V, ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixel(const __point_type& p)
  {
    mln_precondition(m_domain.has(p));
    return pixel_at(p);
  }

  template <int N, class V>
  details::ndimage_pixel<N, const V, const ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixel(const __point_type& p) const
  {
    mln_precondition(m_domain.has(p));
    return pixel_at(p);
  }

  template <int N, class V>
  details::ndimage_extension<N, V>
  ndimage<N, V, layout_t::INTERLEAVED>::extension() const
  {
    details::ndimage_extension<N, V> e(this->m_domain, this->m_virtual_domain, this->m_strides, mln::ptr_offset<V>(this->m_ptr, this->m_first_index * sizeof(V)));
    return e;
  }


  template <int N, class V>
  details::ndimage_value_range<N, V, ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::values()
  {
    return value_range(this);
  }

  template <int N, class V>
  details::ndimage_value_range<N, const V, const ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::values() const
  {
    return const_value_range(this);
  }

  template <int N, class V>
  details::ndimage_pixel_range<N, V, ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixels()
  {
    return pixel_range(this);
  }

  template <int N, class V>
  details::ndimage_pixel_range<N, const V, const ndimage<N, V, layout_t::INTERLEAVED>>
  ndimage<N, V, layout_t::INTERLEAVED>::pixels() const
  {
    return const_pixel_range(this);
  }

}
