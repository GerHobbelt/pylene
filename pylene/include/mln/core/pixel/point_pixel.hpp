#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/concept/pixel.hpp>
#include <mln/core/image_traits.hpp>


namespace mln
{

  /// \brief Define a pixel accessing value through a point
  ///
  /// \tparam Image must be a model of Accessible image
  template <class Image>
  struct point_pixel : Pixel<point_pixel<Image>>
  {
    using point_type = mln_point(Image);
    using site_type  = mln_point(Image);
    using image_type = Image;
    using reference  = mln_reference(Image);
    using value_type = mln_value(Image);

    point_pixel()
      : m_image(nullptr)
    {
    }

    point_pixel(Image& image, const point_type& point)
      : m_image(&image)
      , m_point(point)
    {
    }

    point_pixel(const point_pixel&) = default;

    // Allow conversion pixel -> const pixel
    template <class I2>
    point_pixel(const point_pixel<I2>& other,
                std::enable_if_t<std::is_convertible<I2*, Image*>::value, void*> = nullptr)
      : m_image(other.m_image)
      , m_point(other.m_point)
    {
    }

    image_type& image() const
    {
      mln_precondition(m_image);
      return *m_image;
    }
    constexpr int weight() const { return 1; }
    reference     val() const
    {
      mln_precondition(m_image);
      return m_image->at(m_point);
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

  private:
    image_type* m_image;
    site_type   m_point;
  };

} // namespace mln
