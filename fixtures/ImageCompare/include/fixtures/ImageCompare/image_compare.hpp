#pragma once

#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/io/imprint.hpp>

#include <range/v3/begin_end.hpp>

#include <string>

#include <gtest/gtest.h>


namespace fixtures::ImageCompare
{

  /// \brief Compare if two images are equals
  ///
  /// \param[in] f First image to compare
  /// \param[in] g Second image to compare
  /// Two image are equals iff every pixel (point + value) are equals.
  template <class I, class J>
  bool compare(const mln::Image<I>& f, const mln::Image<J>& g);

  namespace experimental
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
  } // namespace experimental

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class I, class J>
  bool compare(const mln::Image<I>& reference, const mln::Image<J>& input)
  {
    mln_pixter(px1, mln::exact(reference));
    mln_pixter(px2, mln::exact(input));

    mln_forall (px1, px2)
    {
      if (px1->point() != px2->point() or px1->val() != px2->val())
        return false;
    }
    if (px1.finished() && px2.finished())
      return true;
    return false;
  }

  namespace impl
  {
    template <class I, class J>
    std::string err_compare_msg(const mln::Image<I>& reference, const mln::Image<J>& input, bool eq = true)
    {
      std::stringstream msg;
      if (eq)
        msg << "The following images differ:\n";
      else
        msg << "The following images are identical:\n";
      mln::io::imprint(reference, msg);
      msg << " and\n:";
      mln::io::imprint(input, msg);
      return msg.str();
    }
  } // namespace impl


  namespace experimental
  {
    namespace impl
    {

      /// Compare overload sets
      /// \{

      /// Type-erased implementation for ndimages
      ::testing::AssertionResult compare(const mln::ndbuffer_image& a,
                                         const mln::ndbuffer_image& b,
                                         int comparison_flags);


      /// Best-match implementation for ndimages (forwarding to the type-erased one)
      template <class TA, class TB, int adim, int bdim>
      ::testing::AssertionResult compare(const mln::__ndbuffer_image<TA, adim>& a,
                                         const mln::__ndbuffer_image<TB, bdim>& b,
                                         int comparison_flags)
      {
        return compare(static_cast<const mln::ndbuffer_image&>(a),
                       static_cast<const mln::ndbuffer_image&>(b), comparison_flags);
      }

      template <class ImageA, class ImageB>
      ::testing::AssertionResult compare(const mln::experimental::Image<ImageA>& f_,
                                         const mln::experimental::Image<ImageB>& g_,
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

        {
          auto zipped_vals = mln::ranges::view::zip(f.new_values(), g.new_values());
          for (auto&& r : mln::ranges::rows(zipped_vals))
            for (auto&& [f_v, g_v] : r)
              if (f_v != g_v)
                return testing::AssertionFailure() << "The values of image A and B differ";
        }
        return ::testing::AssertionSuccess();
      }

      /// \}
    } // namespace impl


    template <class ImageLhs, class ImageRhs>
    ::testing::AssertionResult compare(ImageLhs&& f, ImageRhs&& g, int comparison_flags)
    {
      using A = std::remove_reference_t<ImageLhs>;
      using B = std::remove_reference_t<ImageRhs>;
      static_assert(mln::is_a<A, mln::experimental::Image>());
      static_assert(mln::is_a<B, mln::experimental::Image>());

      return impl::compare(std::forward<ImageLhs>(f), std::forward<ImageRhs>(g), comparison_flags);
    }

  } // namespace experimental


} // namespace fixtures::ImageCompare

/// \brief GTest macro for image equality test
#define ASSERT_IMAGES_EQ(f, g)                                                                                         \
  ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g)) << ::fixtures::ImageCompare::impl::err_compare_msg(f, g)

#define ASSERT_IMAGES_NE(f, g)                                                                                         \
  ASSERT_FALSE(::fixtures::ImageCompare::compare(f, g)) << ::fixtures::ImageCompare::impl::err_compare_msg(f, g, false)


#define ASSERT_IMAGES_EQ_EXP(f, g) ASSERT_TRUE(::fixtures::ImageCompare::experimental::compare(f, g))
#define ASSERT_IMAGES_NE_EXP(f, g) ASSERT_FALSE(::fixtures::ImageCompare::experimental::compare(f, g))


#define ASSERT_IMAGES_EQ_EXP2(f, g, flags) ASSERT_TRUE(::fixtures::ImageCompare::experimental::compare(f, g, flags))
#define ASSERT_IMAGES_NE_EXP2(f, g, flags) ASSERT_FALSE(::fixtures::ImageCompare::experimental::compare(f, g, flags))

