#pragma once

#include <ano/core/image/image.hpp>
#include <ano/core/image/ndbuffer_image.hpp>
#include <ano/core/image/ndimage_fwd.hpp>
#include <ano/core/range/rows.hpp>
#include <ano/core/range/view/zip.hpp>
#include <ano/io/imprint.hpp>


#include <range/v3/range/access.hpp>

#include <algorithm>
#include <functional>
#include <string>
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
    ::testing::AssertionResult compare(const ano::ndbuffer_image& a, const ano::ndbuffer_image& b, int comparison_flags,
                                       std::function<int(const void* a, const void* b, std::size_t n)> linecmp_fn,
                                       std::function<void(const ano::ndbuffer_image&)>                 print_fn);


    /// Best-match implementation for ndimages (forwarding to the type-erased one)
    template <class TA, class TB, int adim, int bdim>
    ::testing::AssertionResult compare(const ano::__ndbuffer_image<TA, adim>& a,
                                       const ano::__ndbuffer_image<TB, bdim>& b, int comparison_flags)
    {
      std::function<void(ano::ndbuffer_image)>                        print_fn;
      std::function<int(const void* a, const void* b, std::size_t n)> linecmp_fn;

      if constexpr (fmt::internal::has_formatter<TA, fmt::format_context>() &&
                    fmt::internal::has_formatter<TB, fmt::format_context>())
      {
        print_fn = [](const ano::ndbuffer_image& ima) {
          if (const auto* a = ima.cast_to<TA, adim>())
            ano::io::imprint(*a);
          else if (const auto* b = ima.cast_to<TB, bdim>())
            ano::io::imprint(*b);
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

      return compare(static_cast<const ano::ndbuffer_image&>(a), static_cast<const ano::ndbuffer_image&>(b),
                     comparison_flags, linecmp_fn, print_fn);
    }


    template <class ImageA, class ImageB>
    ::testing::AssertionResult compare(const ano::details::Image<ImageA>& f_, const ano::details::Image<ImageB>& g_,
                                       int comparison_flags)
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
        auto zipped_vals = ano::ranges::view::zip(f.values(), g.values());
        for (auto&& r : ano::ranges::rows(zipped_vals))
          for (auto&& [f_v, g_v] : r)
            if (f_v != g_v)
              pass = false;
      }
      if constexpr (std::is_same_v<f_domain_t, ano::box2d> && //
                    std::is_same_v<g_domain_t, ano::box2d> && //
                    fmt::internal::has_formatter<ano::image_value_t<ImageA>, fmt::format_context>() &&
                    fmt::internal::has_formatter<ano::image_value_t<ImageB>, fmt::format_context>())
      {
        if (!pass)
        {
          fmt::print("A = \n");
          ano::io::imprint(f);
          fmt::print("vs B = \n");
          ano::io::imprint(g);
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
    static_assert(ano::is_a<A, ano::details::Image>());
    static_assert(ano::is_a<B, ano::details::Image>());

    return impl::compare(std::forward<ImageLhs>(f), std::forward<ImageRhs>(g), comparison_flags);
  }

} // namespace fixtures::ImageCompare

/// \brief GTest macro for image equality test

#define ASSERT_IMAGES_EQ_EXP(f, g) ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g))
#define ASSERT_IMAGES_NE_EXP(f, g) ASSERT_FALSE(::fixtures::ImageCompare::compare(f, g))


#define ASSERT_IMAGES_EQ_EXP2(f, g, flags) ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g, flags))
#define ASSERT_IMAGES_NE_EXP2(f, g, flags) ASSERT_FALSE(::fixtures::ImageCompare::compare(f, g, flags))
