#include <mln/core/concept/new/images.hpp>

#include <gtest/gtest.h>

#include <tuple>
#include <type_traits>

namespace concepts = mln::concepts;

struct mock_image;

struct mock_image
{

  struct mock_domain
  {
    struct mock_point
    {
      int x;
    };

    using value_type = mock_point;
    using reference  = mock_point&;

    mock_point* begin();
    mock_point* end();

    bool     has(value_type) const;
    bool     empty() const;
    unsigned size() const;
  };

  struct mock_pixel
  {

    using point_type = mock_domain::value_type;
    using value_type = double;
    using reference  = double&;

    point_type point() const { return pnt; }
    reference  val() const { return *v; }

  private:
    point_type  pnt;
    value_type* v;
  };

  using new_pixel_type = mock_pixel;
  using value_type     = mock_pixel::value_type;
  using reference      = mock_pixel::reference;
  using point_type     = mock_domain::value_type;
  using domain_type    = mock_domain;
  using category_type  = mln::new_forward_image_tag;
  using concrete_type  = mock_image;

  template <concepts::Value Val>
  using ch_value_type = mock_image;

  // additional traits
  using extension_category = mln::extension::none_extension_tag;
  using indexable          = std::false_type;
  using accessible         = std::false_type;
  using bidirectional      = std::false_type;
  using raw                = std::false_type;


  template <concepts::Value Val>
  ch_value_type<Val> ch_value() const;
  concrete_type      concretize() const;
  domain_type        domain() const;

  struct mock_pixel_rng
  {
    using value_type = new_pixel_type;
    using reference  = new_pixel_type&;
    new_pixel_type* begin();
    new_pixel_type* end();
  };
  mock_pixel_rng new_pixels();

  struct mock_value_rng
  {
    using value_type = mock_image::value_type;
    using reference  = mock_image::reference;
    mock_image::value_type* begin();
    mock_image::value_type* end();
  };
  mock_value_rng new_values();
};


bool operator==(const mock_image::mock_domain::mock_point& lhs, const mock_image::mock_domain::mock_point& rhs)
{
  return lhs.x == rhs.x;
}
bool operator!=(const mock_image::mock_domain::mock_point& lhs, const mock_image::mock_domain::mock_point& rhs)
{
  return !(lhs == rhs);
}
bool operator<(const mock_image::mock_domain::mock_point& lhs, const mock_image::mock_domain::mock_point& rhs)
{
  return lhs.x < rhs.x;
}
bool operator>(const mock_image::mock_domain::mock_point& lhs, const mock_image::mock_domain::mock_point& rhs)
{
  return rhs < lhs;
}
bool operator<=(const mock_image::mock_domain::mock_point& lhs, const mock_image::mock_domain::mock_point& rhs)
{
  return !(lhs > rhs);
}
bool operator>=(const mock_image::mock_domain::mock_point& lhs, const mock_image::mock_domain::mock_point& rhs)
{
  return !(lhs < rhs);
}

bool operator==(const mock_image::mock_pixel& lhs, const mock_image::mock_pixel& rhs)
{
  return lhs.point() == rhs.point() && lhs.val() == rhs.val();
}
bool operator!=(const mock_image::mock_pixel& lhs, const mock_image::mock_pixel& rhs)
{
  return !(lhs == rhs);
}


template <typename Ima>
requires concepts::Image<Ima> void foo(Ima)
{
}

TEST(Core, Concept_Image)
{
  static_assert(concepts::Domain<mock_image::mock_domain>);
  static_assert(concepts::Pixel<mock_image::mock_pixel>);
  static_assert(concepts::stl::ForwardRange<mock_image::mock_pixel_rng>);
  static_assert(concepts::stl::ForwardRange<mock_image::mock_value_rng>);
  static_assert(concepts::Image<mock_image>);
}


// TODO check all images properties
