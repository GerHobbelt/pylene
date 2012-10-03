#ifndef MLN_CORE_CONCEPT_PIXEL_HPP
# define MLN_CORE_CONCEPT_PIXEL_HPP

# include <mln/core/concept/check.hpp>
# include <boost/concept_check.hpp>

namespace mln
{

  template <typename Pix>
  struct Pixel
  {

    BOOST_CONCEPT_USAGE(Pixel)
    {
      typedef typename Pix::value_type        value_type;
      typedef typename Pix::reference         reference;
      typedef typename Pix::point_type        point_type;
      typedef typename Pix::site_type         site_type;
      typedef typename Pix::image_type        image_type;

      reference   (Pix::*method1) () const = &Pix::val;
      point_type  (Pix::*method2) () const = &Pix::point;
      site_type   (Pix::*method3) () const = &Pix::site;
      image_type& (Pix::*method4) () const = &Pix::image;

      (void) method1;
      (void) method2;
      (void) method3;
      (void) method4;

      // site() and point() aliase each others
      check(std::is_same<point_type, site_type> ());

      // value_type should not be a reference
      check_false(std::is_reference<value_type> ());
    }


  };

} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_PIXEL_HPP
