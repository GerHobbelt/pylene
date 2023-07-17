#pragma once

#include <mln/core/image/ndimage_fwd.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>
#include <mln/core/range/view/zip.hpp>
#include <mln/io/imprint.hpp>

#include <range/v3/range/access.hpp>

#include <string>
#include <functional>
#include <algorithm>
#include <type_traits>

#include <gtest/gtest.h>


namespace fixtures::ImageCompare
{

  enum
  {
    COMPARE_DOMAIN    = 0x01,
    COMPARE_EXTENSION = 0x02
  };


  /// \brief Compare if two images are equals
  ///
  /// \param[in] f First image to compare
  /// \param[in] g Second image to compare
  /// Two image are equals iff every pixel (point + value) are equals.
  template <class ImageLhs, class ImageRhs>
  ::testing::AssertionResult compare(ImageLhs&& f, ImageRhs&& g, int comparison_flags = 0);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace impl
  {

    /// Compare overload sets
    /// \{

    /// Type-erased implementation for ndimages
    ::testing::AssertionResult compare(const mln::ndbuffer_image& a, const mln::ndbuffer_image& b, int comparison_flags,
                                       std::function<int(const void* a, const void* b, std::size_t n)> linecmp_fn,
                                       std::function<void(const mln::ndbuffer_image&)>                 print_fn);


    /// Best-match implementation for ndimages (forwarding to the type-erased one)
    template <class TA, class TB, int adim, int bdim>
    ::testing::AssertionResult compare(const mln::__ndbuffer_image<TA, adim>& a,
                                       const mln::__ndbuffer_image<TB, bdim>& b, int comparison_flags)
    {
      std::function<void(mln::ndbuffer_image)>                        print_fn;
      std::function<int(const void* a, const void* b, std::size_t n)> linecmp_fn;

      if constexpr (fmt::has_formatter<TA, fmt::format_context>() &&
                    fmt::has_formatter<TB, fmt::format_context>())
      {
        print_fn = [](const mln::ndbuffer_image& ima) {
          if (const auto* a = ima.cast_to<TA, adim>())
            mln::io::imprint(*a);
          else if (const auto* b = ima.cast_to<TB, bdim>())
            mln::io::imprint(*b);
        };
      }

      // If value types are the same and they are tricially copyable, we will use memcpy
      // Otherwise, we type-erase the comparison function
      if constexpr (!(std::is_same_v<TA, TB> && std::is_trivially_copyable_v<TA>))
      {
        linecmp_fn = [](const void* a, const void* b, std::size_t n) -> int {
          return !std::equal(reinterpret_cast<const TA*>(a), reinterpret_cast<const TA*>(a) + n,
                             reinterpret_cast<const TB*>(b));
        };
      }

      return compare(static_cast<const mln::ndbuffer_image&>(a), static_cast<const mln::ndbuffer_image&>(b),
                     comparison_flags, linecmp_fn, print_fn);
    }



    template <class ImageA, class ImageB>
    ::testing::AssertionResult compare(const mln::details::Image<ImageA>& f_,
                                       const mln::details::Image<ImageB>& g_, int comparison_flags)
    {
      ImageA f = static_cast<const ImageA&>(f_);
      ImageB g = static_cast<const ImageB&>(g_);

      using f_domain_t = typename ImageA::domain_type;
      using g_domain_t = typename ImageB::domain_type;

      constexpr auto f_dim = f_domain_t::ndim;
      constexpr auto g_dim = g_domain_t::ndim;
      static_assert(f_dim == g_dim);

      if (comparison_flags & COMPARE_EXTENSION)
      {
        throw std::runtime_error("Do not known how to compare extensions in a generic way.");
      }

      if (comparison_flags & COMPARE_DOMAIN)
      {
        if (f.domain() != g.domain())
          return testing::AssertionFailure() << "The domains of A and B differ.";
      }

      bool pass = true;
      {
        auto zipped_vals = mln::ranges::view::zip(f.values(), g.values());
        for (auto&& r : mln::ranges::rows(zipped_vals))
          for (auto&& [f_v, g_v] : r)
            if (f_v != g_v)
              pass = false;
      }
      if constexpr (std::is_same_v<f_domain_t, mln::box2d> && //
                    std::is_same_v<g_domain_t, mln::box2d> && //
                    fmt::has_formatter<mln::image_value_t<ImageA>, fmt::format_context>() &&
                    fmt::has_formatter<mln::image_value_t<ImageB>, fmt::format_context>())
      {
        if (!pass)
        {
          fmt::print("A = \n");
          mln::io::imprint(f);
          fmt::print("vs B = \n");
          mln::io::imprint(g);
        }
      }

      if (!pass)
        return ::testing::AssertionFailure() << "The values of image A and B differ";
      else
        return ::testing::AssertionSuccess();
    }

    /// \}
  } // namespace impl


  template <class ImageLhs, class ImageRhs>
  ::testing::AssertionResult compare(ImageLhs&& f, ImageRhs&& g, int comparison_flags)
  {
    using A = std::remove_reference_t<ImageLhs>;
    using B = std::remove_reference_t<ImageRhs>;
    static_assert(mln::is_a<A, mln::details::Image>());
    static_assert(mln::is_a<B, mln::details::Image>());

    return impl::compare(std::forward<ImageLhs>(f), std::forward<ImageRhs>(g), comparison_flags);
  }

} // namespace fixtures::ImageCompare

/// \brief GTest macro for image equality test

#define ASSERT_IMAGES_EQ_EXP(f, g) ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g))
#define ASSERT_IMAGES_NE_EXP(f, g) ASSERT_FALSE(::fixtures::ImageCompare::compare(f, g))


#define ASSERT_IMAGES_EQ_EXP2(f, g, flags) ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g, flags))
#define ASSERT_IMAGES_NE_EXP2(f, g, flags) ASSERT_FALSE(::fixtures::ImageCompare::compare(f, g, flags))

