#ifndef MLN_CORE_IMAGE_IMAGE2D_HPP
#define MLN_CORE_IMAGE_IMAGE2D_HPP

#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>

namespace mln
{

  // FWD declaration
  template <typename T>
  struct image2d;

  // Specialization of traits
  template <typename T>
  struct image_traits<image2d<T>> : image_traits<ndimage_base<T, 2, image2d<T>>>
  {
  };

  template <typename T, typename V>
  struct image_ch_value<image2d<T>, V>
  {
    typedef image2d<V> type;
  };

  ///
  /// \brief The standard 2D image type.
  ///
  /// \p image2d is a type to represent 2d image. They are implemented with contiguous
  /// data buffer to allow pointer arithmetic. However data are not contiguous in this buffer
  /// since each row is aligned on a 128-bits boundary to get performance. \p image2d are soft image
  /// since two images can share the same buffer of data to avoid uncessary copies.
  ///
  /// \p image2d models a Writable Point-Accessible Image concept.
  /// See \see image2d \see image3d
  template <typename T>
  struct image2d : ndimage_base<T, 2, image2d<T>>
  {
  private:
    using base_type = ndimage_base<T, 2, image2d<T>>;
    using base      = ndimage_base<T, 2, image2d<T>>;

  public:
    typedef typename base_type::domain_type domain_type;

    template <class V>
    using ch_value_type = image2d<V>;

    using base::base;

    image2d(short nrows, short ncols, unsigned border = 3, const T& init = T())
      : base(box<short, 2>{{0, 0}, {nrows, ncols}}, border, init)
    {
    }

    /// \brief Initialization constructor
    /// \{
    template <typename U>
    image2d(const image2d<U>& f, mln::init)
      : base(f, mln::init())
    {
    }

    template <typename U>
    image2d(const image2d<U>& f, unsigned border, const T& v = T())
      : base(f, border, v)
    {
    }
    /// \}

    image2d(std::initializer_list<std::initializer_list<T>> l, unsigned border = 3)
    {
      mln_precondition(l.size() != 0);
      mln_precondition((l.begin())->size() != 0);

      auto nr = l.size();
      auto nc = (l.begin()->size());
      this->resize(domain_type{{0, 0}, {(short)nr, (short)nc}}, border);

      mln_iter(v, this->values());
      v.init();
      for (const std::initializer_list<T>& row : l)
      {
        mln_assertion(row.size() == nc);
        for (const T* val = row.begin(); val != row.end(); ++val, v.next())
          *v = *val;
      }
    }

    using base::at;

    T& at(short nrows, short ncols) { return base::at(typename base::point_type(nrows, ncols)); }

    const T& at(short nrows, short ncols) const { return base::at(typename base::point_type(nrows, ncols)); }

    unsigned nrows() const { return this->m_domain.pmax[0] - this->m_domain.pmin[0]; }
    unsigned ncols() const { return this->m_domain.pmax[1] - this->m_domain.pmin[1]; }
  };

} // end of namespace mln

#endif //! MLN_CORE_IMAGE_IMAGE2D_HPP
