#pragma once

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
    template <class ImageLhs, class ImageRhs>
    bool compare(ImageLhs f, ImageRhs g);
  }

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
    std::string err_compare_msg(const mln::Image<I>& reference, const mln::Image<J>& input)
    {
      std::stringstream msg;
      msg << "The following images differs:\n";
      mln::io::imprint(reference, msg);
      msg << " and\n:";
      mln::io::imprint(input, msg);
      return msg.str();
    }
  } // namespace impl


  namespace experimental
  {
    template <class ImageLhs, class ImageRhs>
    bool compare(ImageLhs f, ImageRhs g)
    {
      static_assert(mln::is_a<ImageLhs, mln::experimental::Image>());
      static_assert(mln::is_a<ImageRhs, mln::experimental::Image>());

      auto f_dom = f.domain();
      auto g_dom = g.domain();

      auto f_dom_b = ::ranges::begin(f_dom);
      auto g_dom_b = ::ranges::begin(g_dom);
      auto f_dom_e = ::ranges::end(f_dom);
      auto g_dom_e = ::ranges::end(g_dom);
      for (; (f_dom_b != f_dom_e) || (g_dom_b != g_dom_e); ++f_dom_b, ++g_dom_b)
        if (*f_dom_b != *g_dom_b)
          return false;

      if ((f_dom_b != f_dom_e) || (g_dom_b != g_dom_e))
        return false;

      auto zipped_vals = mln::ranges::view::zip(f.new_values(), g.new_values());
      for (auto&& r : mln::ranges::rows(zipped_vals))
        for (auto&& [f_v, g_v] : r)
          if (f_v != g_v)
            return false;

      return true;
    }

    namespace impl
    {
      template <class ImageLhs, class ImageRhs>
      std::string err_compare_msg(ImageLhs f, ImageRhs g)
      {
        static_assert(mln::is_a<ImageLhs, mln::experimental::Image>());
        static_assert(mln::is_a<ImageRhs, mln::experimental::Image>());

        std::stringstream msg;
        msg << "The following images differs:\n";
        mln::io::experimental::imprint(f, msg);
        msg << " and\n:";
        mln::io::experimental::imprint(g, msg);
        return msg.str();
      }
    } // namespace impl
  }   // namespace experimental


} // namespace fixtures::ImageCompare

/// \brief GTest macro for image equality test
#define ASSERT_IMAGES_EQ(f, g)                                                                                         \
  ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g)) << ::fixtures::ImageCompare::impl::err_compare_msg(f, g)

#define ASSERT_IMAGES_EQ_EXP(f, g)                                                                                     \
  ASSERT_TRUE(::fixtures::ImageCompare::experimental::compare(f, g))                                                   \
      << ::fixtures::ImageCompare::experimental::impl::err_compare_msg(f, g)
