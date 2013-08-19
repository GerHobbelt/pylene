#ifndef MLN_CORE_CONCEPT_NEIGHBORHOOD_HPP
# define MLN_CORE_CONCEPT_NEIGHBORHOOD_HPP

# include <mln/core/concept/object.hpp>
# include <mln/core/concept/check.hpp>
# include <boost/concept_check.hpp>
# include <mln/core/concept/pixel.hpp>

namespace mln
{

  template <class N> struct Neighborhood;
  template <class N> struct Neighborhood_;

  template <class N>
  struct Neighborhood : Object<N>
  {
    BOOST_CONCEPT_USAGE(Neighborhood)
    {
      BOOST_CONCEPT_ASSERT((Neighborhood_<N>));
    }

  protected:
    Neighborhood() = default;
    Neighborhood(const Neighborhood&) = default;
    Neighborhood(Neighborhood&&) = default;
  };

  template <class N>
  struct Neighborhood_
  {
    typedef neighborhood_traits<N> traits;

    typedef typename N::point_type P;
    typedef typename N::site_type  S;
    typedef typename traits::category category;

    BOOST_CONCEPT_USAGE(Neighborhood_)
    {

      check(std::is_same<P, S> ());

      // nbh(p) with p a rvalue e.g
      {
	auto r = nbh(P());
	typedef decltype(r) R;
	// FIXME: check that R is forward range
	// FIXME: check that R value type is convertible to p;
      }

      // nbh(p) with p a lvalue e.g
      {
	auto r = nbh(p);
	typedef decltype(r) R;
      }

      // nbh(pix) with pix a lvalue e.g
      // nbh(pix) require a valid image...
      // c4(px)
      // {
      // 	auto r = nbh(pix);
      // 	typedef decltype(r) R;
      // }

    }

  private:
    N nbh;
    P p;
    pixel_archetype<P> pix;
  };


}

#endif // ! MLN_CORE_CONCEPT_NEIGHBORHOOD_HPP
