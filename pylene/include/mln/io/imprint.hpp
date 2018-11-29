#ifndef MLN_IO_IMPRINT_HPP
#define MLN_IO_IMPRINT_HPP

#include <iomanip>
#include <iostream>
#include <type_traits>

#include <mln/core/domain/box.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/value/value_traits.hpp>
#include <mln/io/format.hpp>

#include <cmath>

namespace mln
{

  namespace io
  {

    template <typename I>
    void imprint(const Image<I>& ima, std::ostream& os = std::cout);

    template <typename I>
    void imprint_with_border(const Image<I>& ima, std::ostream& os = std::cout);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace internal
    {

      template <typename Image, typename Domain>
      std::enable_if_t<image_traits<Image>::accessible> imprint(const Image& ima, Domain domain, std::ostream& os)
      {
        for (auto p : domain)
        {
          os << '{' << p << ",";
          format(os, ima(p)) << "}," << std::endl;
        }
      }

      template <typename Image, typename Domain>
      std::enable_if_t<std::is_convertible<typename image_traits<Image>::category, forward_image_tag>::value>
          imprint(const Image& ima, Domain domain, std::ostream& os)
      {
        (void)domain;
        mln_foreach (const auto& pix, ima.pixels())
        {
          os << '{' << pix.point() << ",";
          format(os, pix.val()) << "}," << std::endl;
        }
      }

      template <typename Image, typename P>
      std::enable_if_t<image_traits<Image>::accessible::value> imprint(const Image& ima, box<P, 2> domain,
                                                                       std::ostream& os)
      {
        typedef typename Image::value_type V;

        std::ios state(NULL);
        state.copyfmt(os);

        int               wtext = 0;
        frmt_max_width<V> frmter;
        mln_foreach (V v, ima.values())
          wtext = std::max(wtext, frmter(v));

        os << domain << "(" << typeid(V).name() << ")" << std::endl;
        os.width(4);

        mln_point(Image) p;
        for (p[0] = domain.pmin[0]; p[0] < domain.pmax[0]; ++p[0])
        {
          for (p[1] = domain.pmin[1]; p[1] < domain.pmax[1]; ++p[1])
          {
            os << std::setw(wtext);
            format(os, ima(p)) << " ";
          }
          os << std::endl;
        }
        os.copyfmt(state);
      }

      template <typename Image, typename P>
      typename std::enable_if<image_traits<Image>::accessible::value, void>::type
          imprint(const Image& ima, box<P, 3> domain, std::ostream& os)
      {
        typedef typename Image::value_type V;

        std::ios state(NULL);
        state.copyfmt(os);

        int               wtext = 0;
        frmt_max_width<V> frmter;
        mln_foreach (V v, ima.values())
          wtext = std::max(wtext, frmter(v));

        os << domain << "(" << typeid(V).name() << ")" << std::endl;
        os.width(4);

        mln_point(Image) p;
        for (p[0] = domain.pmin[0]; p[0] < domain.pmax[0]; ++p[0])
        {
          for (p[1] = domain.pmin[1]; p[1] < domain.pmax[1]; ++p[1])
          {
            for (p[2] = domain.pmin[2]; p[2] < domain.pmax[2]; ++p[2])
            {
              os << std::setw(wtext);
              format(os, ima(p)) << " ";
            }
            os << std::endl;
          }
          os << "====== \n";
        }
        os.copyfmt(state);
      }

      template <typename Image, typename P>
      std::enable_if_t<image_traits<Image>::accessible::value> imprint_with_border(const Image& ima, box<P, 2> domain,
                                                                                   std::ostream& os)
      {
        typedef typename Image::value_type V;

        std::ios state(NULL);
        state.copyfmt(os);

        int               wtext = 0;
        frmt_max_width<V> frmter;
        mln_foreach (V v, ima.values())
          wtext = std::max(wtext, frmter(v));

        os << domain << "(" << typeid(V).name() << ")" << std::endl;
        os.width(4);

        mln_point(Image) p;
        int border = ima.border();
        for (p[0] = domain.pmin[0] - border; p[0] < domain.pmax[0] + border; ++p[0])
        {
          for (p[1] = domain.pmin[1] - border; p[1] < domain.pmax[1] + border; ++p[1])
          {
            os << std::setw(wtext);
            format(os, ima.at(p)) << " ";
          }
          os << std::endl;
        }
        os.copyfmt(state);
      }
    }

    template <typename I>
    void imprint(const Image<I>& ima, std::ostream& os)
    {
      internal::imprint(exact(ima), exact(ima).domain(), os);
    }

    template <typename I>
    void imprint_with_border(const Image<I>& ima, std::ostream& os)
    {
      internal::imprint_with_border(exact(ima), exact(ima).domain(), os);
    }
  }
}

#endif
