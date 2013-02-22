#ifndef MLN_IO_IMPRINT_HPP
# define MLN_IO_IMPRINT_HPP

# include <type_traits>
# include <iostream>
# include <iomanip>

# include <mln/core/domain/box.hpp>
# include <mln/core/macros.hpp>
# include <mln/core/grays.hpp>


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
	typedef typename std::conditional<std::is_same<V, uint8>::value, int, typename Image::const_reference>::type T;

        for (auto p : domain)
          std::cout << '{' << p << "," << (T)ima(p) << "}," << std::endl;
      }


      template <typename Image, typename Domain>
      typename std::enable_if< std::is_convertible<typename image_traits<Image>::category,
                                                   forward_image_tag>::value, void>::type
      imprint(const Image& ima, Domain domain)
      {
        typedef typename Image::value_type V;
	typedef typename std::conditional<std::is_same<V, uint8>::value, int, typename Image::const_reference>::type T;
        (void) domain;
	typedef typename Image::const_pixel_type pixel_t;
        mln_foreach (const pixel_t& pix, ima.pixels())
          std::cout << '{' << pix.point() << "," << (T)pix.val() << "}," << std::endl;
      }


      template <typename Image, typename P>
      typename std::enable_if< image_traits<Image>::accessible::value, void>::type
      imprint(const Image& ima, box<P, 2> domain)
      {
        typedef typename Image::value_type V;
	typedef typename std::conditional<std::is_same<V, uint8>::value, int, typename Image::const_reference>::type T;
	mln_point(Image) p;
        std::ios state(NULL);
        state.copyfmt(std::cout);

        std::cout << domain << "(" << typeid(V).name() << ")" << std::endl;

        const int wtext =
          std::is_same<V, uint8>::value ? 3 :
          std::is_same<V, unsigned>::value ? 10 :
          std::is_same<V, std::size_t>::value ? 10 : 0;

        //if (std::is_same<V, uint8>::value)
        std::cout.width(4);
	for (p[0] = domain.pmin[0]; p[0] < domain.pmax[0]; ++p[0]) {
	  for (p[1] = domain.pmin[1]; p[1] < domain.pmax[1]; ++p[1]) {
	    std::cout << std::setw(wtext) << ((T) ima(p)) << " ";
	  }
	  std::cout << std::endl;
	}
        std::cout.copyfmt(state);
      }

      template <typename Image, typename P>
      typename std::enable_if< image_traits<Image>::accessible::value, void>::type
      imprint_with_border(const Image& ima, box<P, 2> domain)
      {
        typedef typename Image::value_type V;
	typedef typename std::conditional<std::is_same<V, uint8>::value, int, typename Image::const_reference>::type T;
	mln_point(Image) p;
        int border = ima.border();

	const int wtext =
          std::is_same<V, uint8>::value ? 3 :
          std::is_same<V, unsigned>::value ? 10 :
          std::is_same<V, std::size_t>::value ? 10 : 0;

        std::cout << domain << "(" << typeid(V).name() << ")" << std::endl;
	std::cout.width(4);
	for (p[0] = domain.pmin[0] - border; p[0] < domain.pmax[0] + border; ++p[0]) {
	  for (p[1] = domain.pmin[1] - border; p[1] < domain.pmax[1] + border; ++p[1]) {
	    std::cout << std::setw(wtext) << ((T) ima.at(p)) << " ";
	  }
	  std::cout << std::endl;
	}
      }

    }

    template <typename I>
    void imprint(const Image<I>& ima)
    {
      internal::imprint(exact(ima), exact(ima).domain());
    }

    template <typename I>
    void imprint_with_border(const Image<I>& ima)
    {
      internal::imprint_with_border(exact(ima), exact(ima).domain());
    }

  }
}

#endif
