#pragma once

#include <mln/core/domain/box.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/value/value_traits.hpp>
#include <mln/io/format.hpp>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <type_traits>


namespace mln
{

  namespace io
  {

    template <typename I>
    void imprint(const Image<I>& ima, std::ostream& os = std::cout);

    template <typename I>
    void imprint_with_border(const Image<I>& ima, std::ostream& os = std::cout, std::size_t default_border_size = 3);


    namespace experimental
    {
      template <typename InputImage>
      void imprint(InputImage ima, std::ostream& os = std::cout);

      template <typename InputImage>
      void imprint_with_border(InputImage ima, std::ostream& os = std::cout, std::size_t default_border_size = 3);
    } // namespace experimental

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
          if constexpr (mln::is_a<Image, mln::experimental::Image>())
          {
            format(os, (*const_cast<Image*>(&ima))(p)) << "}," << std::endl;
          }
          else
          {
            format(os, ima(p)) << "}," << std::endl;
          }
        }
      }

      template <typename Image, typename Domain>
      std::enable_if_t<std::is_convertible<typename image_traits<Image>::category, forward_image_tag>::value>
          imprint(const Image& ima, Domain domain, std::ostream& os)
      {
        (void)domain;

        if constexpr (mln::is_a<Image, mln::experimental::Image>())
        {
          auto pixels = const_cast<Image*>(&ima)->new_pixels();
          for (auto&& r : mln::ranges::rows(pixels))
            for (auto&& pix : r)
            {
              os << '{' << pix.point() << ",";
              format(os, pix.val()) << "}," << std::endl;
            }
        }
        else
        {
          mln_foreach (const auto& pix, ima.pixels())
          {
            os << '{' << pix.point() << ",";
            format(os, pix.val()) << "}," << std::endl;
          }
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

        if constexpr (mln::is_a<Image, mln::experimental::Image>())
        {
          auto vals = const_cast<Image*>(&ima)->new_values();
          for (auto&& r : mln::ranges::rows(vals))
            for (auto&& v : r)
            {
              wtext = std::max(wtext, frmter(v));
            }
        }
        else
        {
          mln_foreach (V v, ima.values())
            wtext = std::max(wtext, frmter(v));
        }

        os << domain << "(" << typeid(V).name() << ")" << std::endl;
        os.width(4);

        mln_point(Image) p;
        for (p[0] = domain.pmin[0]; p[0] < domain.pmax[0]; ++p[0])
        {
          for (p[1] = domain.pmin[1]; p[1] < domain.pmax[1]; ++p[1])
          {
            os << std::setw(wtext);
            if constexpr (mln::is_a<Image, mln::experimental::Image>())
            {
              format(os, (*const_cast<Image*>(&ima))(p)) << " ";
            }
            else
            {
              format(os, ima(p)) << " ";
            }
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

        if constexpr (mln::is_a<Image, mln::experimental::Image>())
        {
          auto vals = const_cast<Image*>(&ima)->new_values();
          for (auto&& r : mln::ranges::rows(vals))
            for (auto&& v : r)
            {
              wtext = std::max(wtext, frmter(v));
            }
        }
        else
        {
          mln_foreach (V v, ima.values())
            wtext = std::max(wtext, frmter(v));
        }

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
              if constexpr (mln::is_a<Image, mln::experimental::Image>())
              {
                format(os, (*const_cast<Image*>(&ima))(p)) << " ";
              }
              else
              {
                format(os, ima(p)) << " ";
              }
            }
            os << std::endl;
          }
          os << "====== \n";
        }
        os.copyfmt(state);
      }

      template <typename Image, typename P>
      std::enable_if_t<image_traits<Image>::accessible::value>
          imprint_with_border(const Image& ima, box<P, 2> domain, std::ostream& os, std::size_t default_border_size)
      {
        typedef typename Image::value_type V;

        std::ios state(NULL);
        state.copyfmt(os);

        int               wtext = 0;
        frmt_max_width<V> frmter;

        if constexpr (mln::is_a<Image, mln::experimental::Image>())
        {
          auto vals = const_cast<Image&>(ima).new_values();
          for (auto&& r : mln::ranges::rows(vals))
            for (auto&& v : r)
            {
              wtext = std::max(wtext, frmter(v));
            }
        }
        else
        {
          mln_foreach (V v, ima.values())
            wtext = std::max(wtext, frmter(v));
        }

        os << domain << "(" << typeid(V).name() << ")" << std::endl;
        os.width(4);

        mln_point(Image) p;
        int border = 0;
        if constexpr (std::is_convertible_v<typename Image::extension_type::is_finite, std::true_type>)
        {
          (void)default_border_size;
          border = ima.extension().size();
        }
        else // infinite border, we use a defaulted value
        {
          border = default_border_size;
        }

        for (p[0] = domain.pmin[0] - border; p[0] < domain.pmax[0] + border; ++p[0])
        {
          for (p[1] = domain.pmin[1] - border; p[1] < domain.pmax[1] + border; ++p[1])
          {
            os << std::setw(wtext);
            if constexpr (mln::is_a<Image, mln::experimental::Image>())
            {
              format(os, const_cast<Image*>(&ima)->at(p)) << " ";
            }
            else
            {
              format(os, ima.at(p)) << " ";
            }
          }
          os << std::endl;
        }
        os.copyfmt(state);
      }
    } // namespace internal

    template <typename I>
    void imprint(const Image<I>& ima, std::ostream& os)
    {
      internal::imprint(exact(ima), exact(ima).domain(), os);
    }

    template <typename I>
    void imprint_with_border(const Image<I>& ima, std::ostream& os, std::size_t default_border_size)
    {
      internal::imprint_with_border(exact(ima), exact(ima).domain(), os, default_border_size);
    }

    namespace experimental
    {
      template <typename InputImage>
      void imprint(InputImage ima, std::ostream& os)
      {
        static_assert(mln::is_a<InputImage, mln::experimental::Image>());

        internal::imprint(ima, ima.domain(), os);
      }

      template <typename InputImage>
      void imprint_with_border(InputImage ima, std::ostream& os, std::size_t default_border_size)
      {
        static_assert(mln::is_a<InputImage, mln::experimental::Image>());

        internal::imprint_with_border(ima, ima.domain(), os, default_border_size);
      }
    } // namespace experimental
  }   // namespace io
} // namespace mln
