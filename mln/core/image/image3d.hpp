#ifndef MLN_CORE_IMAGE_IMAGE3D_HPP
# define MLN_CORE_IMAGE_IMAGE3D_HPP

# include <mln/core/image/ndimage.hpp>
# include <mln/core/ch_value.hpp>

namespace mln {

  // FWD declaration
  template <typename T> struct image3d;


  // Specialization of traits
  template <typename T>
  struct image_traits< image3d<T> > : image_traits< ndimage_base<T, 3, image3d<T> > >
  {
  };

  template <typename T, typename V>
  struct image_ch_value< image3d<T>, V >
  {
    typedef image3d<V> type;
  };

  ///
  /// \brief The standard 3D image type.
  ///
  /// \p image3d is a type to represent 3d image. They are implemented with contiguous
  /// data buffer to allow pointer arithmetic. However data are not contiguous in this buffer
  /// since each row is aligned on a 128-bits boundary to get performance. \p image3d are soft image
  /// since two images can share the same buffer of data to avoid uncessary copies.
  ///
  /// \p image3d models a Writable Point-Accessible Image concept.
  /// See \see image3d
  template <typename T>
  struct image3d : ndimage_base<T, 3, image3d<T> >
  {
  private:
    typedef ndimage_base<T, 3, image3d<T> > base_type;
    typedef ndimage_base<T, 3, image3d<T> > base;

  public:
    typedef typename base_type::domain_type domain_type;


    explicit image3d (unsigned border = 3)
      : ndimage_base<T,3,image3d<T> > (border)
    {
    }

    explicit image3d(const domain_type& domain, unsigned border = 3, const T& init = T())
      : ndimage_base<T,3, image3d<T> >(domain, border, init)
    {
    }

    image3d(short nslices, short nrows, short ncols, unsigned border = 3)
      : ndimage_base<T,3, image3d<T> >( box<short,3>{{0,0,0},{nslices,nrows, ncols}}, border)
    {
    }

    /// \brief Initialization constructor
    /// \{
    template <typename U>
    image3d(const image3d<U>& f, mln::init)
      : base(f, mln::init() )
    {
    }

    template <typename U>
    image3d(const image3d<U>& f, unsigned border, const T& v = T())
      : base(f, border, v)
    {
    }
    /// \}



    image3d(std::initializer_list< std::initializer_list< std::initializer_list<T> > > l, unsigned border = 3)
    {
      mln_precondition(l.size() != 0);
      mln_precondition((l.begin())->size() != 0);

      auto ns = l.size();
      auto nr = (l.begin()->size());
      auto nc = (l.begin()->begin()->size());
      this->resize(domain_type{{0,0,0}, {(short)ns,(short)nr,(short)nc}}, border);

      mln_iter(v, this->values());
      v.init();
      for (const auto& slice : l)
        {
          mln_assertion(slice.size() == nr);
          for (const auto& row : slice)
            {
              mln_assertion(row.size() == nc);
              for (const T* val = row.begin(); val != row.end(); ++val, v.next())
                *v = *val;
            }
        }
    }

    using base::at;

    T& at(short slice, short row, short col)
    {
      return base::at(typename base::point_type(slice, row, col));
    }

    const T& at(short slice, short row, short col) const
    {
      return base::at(typename base::point_type(slice, row, col));
    }

    unsigned nslices() const { return this->m_domain.pmax[0] - this->m_domain.pmin[0]; }
    unsigned nrows() const { return this->m_domain.pmax[1] - this->m_domain.pmin[1]; }
    unsigned ncols() const { return this->m_domain.pmax[2] - this->m_domain.pmin[2]; }
  };

} // end of namespace mln



#endif //!MLN_CORE_IMAGE_IMAGE3D_HPP
