#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
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

    template <class ImageLhs, class ImageRhs>
    bool compare_with_border(ImageLhs f, ImageRhs g, std::size_t default_border = 3);
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
    template <class ImageLhs, class ImageRhs>
    bool compare(ImageLhs f, ImageRhs g)
    {
      static_assert(mln::is_a<ImageLhs, mln::experimental::Image>());
      static_assert(mln::is_a<ImageRhs, mln::experimental::Image>());

      using f_domain_t = typename ImageLhs::domain_type;
      using g_domain_t = typename ImageRhs::domain_type;

      constexpr auto f_dim = f_domain_t::dimension;
      constexpr auto g_dim = g_domain_t::dimension;
      static_assert(f_dim == g_dim);

      {
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
      }

      {
        auto zipped_vals = mln::ranges::view::zip(f.new_values(), g.new_values());
        for (auto&& r : mln::ranges::rows(zipped_vals))
          for (auto&& [f_v, g_v] : r)
            if (f_v != g_v)
              return false;
      }

      return true;
    }

    namespace detail
    {
      template <typename Pnt, typename S, std::size_t... I>
      Pnt shift_pnt_impl(const Pnt& p, S shift, std::index_sequence<I...>)
      {
        return Pnt((p[I] + shift)...);
      }

      template <std::size_t D, typename Pnt, typename S>
      Pnt shift_pnt(const Pnt& p, S shift)
      {
        return shift_pnt_impl(p, shift, std::make_index_sequence<D>{});
      }
    } // namespace detail

    template <class ImageLhs, class ImageRhs>
    bool compare_with_border(ImageLhs f, ImageRhs g, std::size_t default_border)
    {
      static_assert(mln::is_a<ImageLhs, mln::experimental::Image>());
      static_assert(mln::is_a<ImageRhs, mln::experimental::Image>());

      using f_domain_t = typename ImageLhs::domain_type;
      using g_domain_t = typename ImageRhs::domain_type;

      constexpr auto f_dim = f_domain_t::dimension;
      constexpr auto g_dim = g_domain_t::dimension;
      static_assert(f_dim == g_dim);

      auto pmin = typename f_domain_t::point_type{};
      auto pmax = typename f_domain_t::point_type{};

      {
        auto f_dom = f.domain();
        auto g_dom = g.domain();

        pmin         = *::ranges::begin(f_dom);
        auto f_dom_r = mln::ranges::view::reverse(f_dom);
        pmax         = *::ranges::begin(f_dom_r);

        auto f_dom_b = ::ranges::begin(f_dom);
        auto g_dom_b = ::ranges::begin(g_dom);
        auto f_dom_e = ::ranges::end(f_dom);
        auto g_dom_e = ::ranges::end(g_dom);

        for (; (f_dom_b != f_dom_e) || (g_dom_b != g_dom_e); ++f_dom_b, ++g_dom_b)
          if (*f_dom_b != *g_dom_b)
            return false;

        if ((f_dom_b != f_dom_e) || (g_dom_b != g_dom_e))
          return false;
      }

      {
        int  border = 0;
        auto f_ext  = f.extension();
        auto g_ext  = g.extension();
        border      = std::min(std::min(f_ext.extent(), g_ext.extent()), static_cast<int>(default_border));

        auto extended_dom =
            f_domain_t{detail::shift_pnt<f_dim>(pmin, -border), detail::shift_pnt<f_dim>(pmax, +border)};
        for (auto&& p : extended_dom)
          if (f.at(p) != g.at(p))
            return false;
      }

      return true;
    } // namespace experimental

    namespace impl
    {
      template <class ImageLhs, class ImageRhs>
      std::string err_compare_msg(ImageLhs f, ImageRhs g, bool eq = true)
      {
        static_assert(mln::is_a<ImageLhs, mln::experimental::Image>());
        static_assert(mln::is_a<ImageRhs, mln::experimental::Image>());

        std::stringstream msg;
        if (eq)
          msg << "The following images differ:\n";
        else
          msg << "The following images are identical:\n";
        mln::io::experimental::imprint(f, msg);
        msg << " and\n:";
        mln::io::experimental::imprint(g, msg);
        return msg.str();
      }

      template <class ImageLhs, class ImageRhs>
      std::string err_compare_with_border_msg(ImageLhs f, ImageRhs g, bool eq = true, std::size_t default_border = 3)
      {
        static_assert(mln::is_a<ImageLhs, mln::experimental::Image>());
        static_assert(mln::is_a<ImageRhs, mln::experimental::Image>());

        std::stringstream msg;
        if (eq)
          msg << "The following images differ:\n";
        else
          msg << "The following images are identical:\n";
        mln::io::experimental::imprint_with_border(f, msg, default_border);
        msg << " and\n:";
        mln::io::experimental::imprint_with_border(g, msg, default_border);
        return msg.str();
      }
    } // namespace impl
  }   // namespace experimental


} // namespace fixtures::ImageCompare

/// \brief GTest macro for image equality test
#define ASSERT_IMAGES_EQ(f, g)                                                                                         \
  ASSERT_TRUE(::fixtures::ImageCompare::compare(f, g)) << ::fixtures::ImageCompare::impl::err_compare_msg(f, g)

#define ASSERT_IMAGES_NE(f, g)                                                                                         \
  ASSERT_FALSE(::fixtures::ImageCompare::compare(f, g)) << ::fixtures::ImageCompare::impl::err_compare_msg(f, g, false)

#define ASSERT_IMAGES_EQ_EXP(f, g)                                                                                     \
  ASSERT_TRUE(::fixtures::ImageCompare::experimental::compare(f, g))                                                   \
      << ::fixtures::ImageCompare::experimental::impl::err_compare_msg(f, g)

#define ASSERT_IMAGES_NE_EXP(f, g)                                                                                     \
  ASSERT_FALSE(::fixtures::ImageCompare::experimental::compare(f, g))                                                  \
      << ::fixtures::ImageCompare::experimental::impl::err_compare_msg(f, g, false)

#define ASSERT_IMAGES_WITH_BORDER_EQ_EXP(f, g)                                                                         \
  ASSERT_TRUE(::fixtures::ImageCompare::experimental::compare_with_border(f, g))                                       \
      << ::fixtures::ImageCompare::experimental::impl::err_compare_with_border_msg(f, g)

#define ASSERT_IMAGES_WITH_BORDER_NE_EXP(f, g)                                                                         \
  ASSERT_FALSE(::fixtures::ImageCompare::experimental::compare_with_border(f, g))                                      \
      << ::fixtures::ImageCompare::experimental::impl::err_compare_with_border_msg(f, g, false)
