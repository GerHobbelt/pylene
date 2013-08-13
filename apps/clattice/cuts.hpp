#ifndef CUTS_HPP
# define CUTS_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/labeling/blobs.hpp>
# include <mln/accu/accumulators/infsup.hpp>
# include <mln/morpho/saturate.hpp>
# include "shape.hpp"


template<typename R, typename V,
	 class NbhFg,
	 class NbhBg,
	 typename shape_t,
	 typename Compare>
void cut_and_get_shapes(const mln::image2d<R>&				ima,
			const NbhFg&					nbhBg,
			const NbhBg&					nbhFg,
			V						lambda,
			Compare						cmp,
			shape_set<shape_t>&				shapes)
{
  using namespace mln;

  typedef short L;

  // Thresholding
  auto input = imtransform(ima, [cmp, lambda](const R& x) {
      return cmp(x, lambda);
    });


  // Labelisation
  L nlabel;
  image2d<L> imlabel;
  std::tie(imlabel, nlabel) = labeling::blobs(input, nbhFg, L());


  // Shape computation
  image2d<bool> cc;
  resize(cc, imlabel);
  accu::accumulators::infsup< point2d, productorder_less<point2d> > bbox;

  // format(std::cout, lambda) << std::endl;
  // io::imprint(input);
  // io::imprint(imlabel);

  for (int l = 1; l <= nlabel; ++l)
    {
      int sz = 0;
      shape_t shp(lambda, cmp);

      morpho::saturate(imlabel == l, nbhBg, point2d(0,0), cc);
      bbox.init();
      mln_foreach(const point2d& p, where(cc))
	{
	  bbox.take(p);
	  ++sz;
	}

      shp.m_bbox = { accu::extractor::inf(bbox), accu::extractor::sup(bbox) };
      shp.m_set.reserve(sz);

      mln_foreach(const point2d& p, where(cc))
	shp.m_set.push_back(p);

      // insert new shape in the set
      auto res = shapes.insert(std::move(shp));

      // If shape already there, update with new information
      if (not res.second)
	res.first->update_with(shp);
    }
}


#endif // ! CUTS_HPP
