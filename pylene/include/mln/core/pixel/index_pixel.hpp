#ifndef MLN_CORE_PIXEL_INDEX_PIXEL_HPP
#define MLN_CORE_PIXEL_INDEX_PIXEL_HPP

#include <mln/core/assert.hpp>
#include <mln/core/concept/pixel.hpp>
#include <mln/core/image_traits.hpp>

namespace mln
{

  /// \brief Define a pixel accessing value through an index
  ///
  /// \tparam Image must be a model of Indexable image
  template <class Image>
  struct index_pixel : Pixel<index_pixel<Image>>
  {
    using point_type = mln_point(Image);
    using site_type  = mln_point(Image);
    using size_type  = typename Image::size_type;
    using image_type = Image;
    using reference  = mln_reference(Image);
    using value_type = mln_value(Image);

    index_pixel()
      : m_image(nullptr)
    {
    }

    index_pixel(Image& image, const point_type& point, size_type index)
      : m_image(&image)
      , m_point(point)
      , m_index(index)
    {
    }

    index_pixel(const index_pixel&) = default;

    // Allow conversion pixel -> const pixel
    template <class I2>
    index_pixel(const index_pixel<I2>& other,
                std::enable_if_t<std::is_convertible<I2*, Image*>::value, void*> = nullptr)
      : m_image(other.m_image)
      , m_point(other.m_point)
      , m_index(other.m_index)
    {
    }

    image_type& image() const
    {
      mln_precondition(m_image);
      return *m_image;
    }
    reference val() const
    {
      mln_precondition(m_image);
      return (*m_image)[m_index];
    }
    point_type point() const
    {
      mln_precondition(m_image);
      return m_point;
    }
    point_type site() const
    {
      mln_precondition(m_image);
      return m_point;
    }
    size_type index() const
    {
      mln_precondition(m_image);
      return m_index;
    }
    constexpr int weight() const { return 1; }

  private:
    image_type* m_image;
    site_type   m_point;
    size_type   m_index;
  };

} // end of namespace mln

#endif //! MLN_CORE_PIXEL_INDEX_PIXEL_HPP
