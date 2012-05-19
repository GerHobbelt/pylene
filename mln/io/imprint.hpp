#ifndef MLN_IO_IMPRINT_HPP
# define MLN_IO_IMPRINT_HPP

# include <type_traits>
# include <iostream>


# include <mln/core/domain/box.hpp>
# include <mln/core/macros.hpp>


namespace mln
{
  namespace io
  {

    namespace internal
    {

      template <typename Image, typename Domain>
      typename std::enable_if< image_traits<Image>::accessible, void>::type
      imprint(const Image& ima, Domain domain)
      {
        typedef typename Image::value_type V;
	typedef typename std::conditional<std::is_same<V, uint8>::value, int, V>::type T;

        for (auto p : domain)
          std::cout << '{' << p << "," << (T)ima(p) << "}," << std::endl;
      }


      template <typename Image, typename Domain>      
      typename std::enable_if< std::is_convertible<typename image_traits<Image>::category,
                                                   forward_image_tag>::value, void>::type
      imprint(const Image& ima, Domain domain)
      {
        typedef typename Image::value_type V;
	typedef typename std::conditional<std::is_same<V, uint8>::value, int, V>::type T;
        (void) domain;
        for (auto pix : ima.pixels())
          std::cout << '{' << pix.point() << "," << (T)pix.val() << "}," << std::endl;
      }


      template <typename Image, typename T>
      void imprint(const Image& ima, box<T, 2> domain)
      {
	typedef typename boost::mpl::if_< boost::is_same<mln_value(Image), uint8>, int, mln_value(Image)>::type t2;
	mln_point(Image) p;
        std::cout << domain << std::endl;
	for (p[0] = domain.pmin[0]; p[0] < domain.pmax[0]; ++p[0]) {
	  for (p[1] = domain.pmin[1]; p[1] < domain.pmax[1]; ++p[1]) {
	    std::cout << ((t2) ima(p)) << " ";
	  }
	  std::cout << std::endl;
	}
      }



    }

    template <typename Image>
    void imprint(const Image& ima)
    {
      internal::imprint(ima, ima.domain());
    }

  }
}

#endif
