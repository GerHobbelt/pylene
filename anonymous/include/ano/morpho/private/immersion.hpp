#pragma once

#include <ano/core/concepts/image.hpp>
#include <ano/core/trace.hpp>
#include <ano/morpho/private/immersion.spe.hpp>

#include <algorithm>
#include <utility>

namespace ano::morpho::details
{

  /// \brief Immerse a 2d or 3d image into twice-as-big 2d image
  /// where the in-between pixel are intervals.
  ///
  /// Given 4 2D-pixels:
  /// a   b
  /// c   d
  /// The resulting image is the intervaled valued image:
  ///
  ///      [a]       [  (a∧b)     (a∨b)]        [b]
  /// [(a∧c) (a∨c)]  [(a∧b∧c∧d) (a∨c∨d∨d)]  [(d∧b) (d∨b)]
  ///      [c]       [  (c∧d)     (c∨d)]        [d]
  ///
  /// \param[in] ima The input 2D image
  /// \return An intervaled valued 2D image
  template <class I>
  std::pair<image_concrete_t<I>, image_concrete_t<I>> //
  immersion(I ima);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {


    // Generic implementation
    template <class I, class P>
    [[gnu::noinline]] //
    void
    line_immersion_T(I& f, P p, int xmin, int xmax, image_concrete_t<I>& inf, image_concrete_t<I>& sup)
    {
      if (xmin >= xmax)
        return;

      p.x() = xmin;

      auto q = 2 * p;
      auto a = f(p);
      inf(q) = a;
      sup(q) = a;

      for (int x = xmin + 1; x < xmax; ++x)
      {
        p.x() = x;

        auto prev   = a;
        a           = f(p);
        auto [m, M] = std::minmax(a, prev);

        q.x()  = 2 * x - 1;
        inf(q) = m;
        sup(q) = M;
        q.x()  = 2 * x;
        inf(q) = a;
        sup(q) = a;
      }
    }

    template <class I>
    [[gnu::noinline]] //
    void
    line_interpolation2d_T(int y, int xmin, int xmax, I& inf, I& sup)
    {
      for (int x = xmin; x < xmax; ++x)
      {
        inf({x, y}) = std::min(inf({x, y - 1}), inf({x, y + 1}));
        sup({x, y}) = std::max(sup({x, y - 1}), sup({x, y + 1}));
      }
    }

    // 2D generic version of immersion
    template <class I>
    void immersion_T(I& f, const ano::box2d& roi, image_concrete_t<I>& inf, image_concrete_t<I>& sup)
    {
      if (roi.empty())
        return;


      int xmin = roi.tl().x();
      int ymin = roi.tl().y();
      int xmax = roi.br().x();
      int ymax = roi.br().y();

      // First line
      line_immersion_T(f, ano::point2d{0, ymin}, xmin, xmax, inf, sup);

      for (int y = ymin + 1; y < ymax; ++y)
      {
        line_immersion_T(f, ano::point2d{0, y}, xmin, xmax, inf, sup);
        line_interpolation2d_T(2 * y - 1, xmin * 2, xmax * 2 - 1, inf, sup);
      }
    }

    // Dispatch
    // \{

    // For 2d-buffer images
    template <class T>
    void immersion(ano::image2d<T>& f, ano::box2d, ano::image2d<T>& inf, ano::image2d<T>& sup)
    {
      ano_entering("ano::morpho::details::immersion (2d-buffer)") immersion_impl_table_t<T> impl;
      immersion_ndimage(f, inf, sup, &impl);
    }

    // For 3d-buffer images (To be implemented)
    template <class T>
    void immersion(ano::image3d<T>& f, ano::box3d, ano::image3d<T>& inf, ano::image3d<T>& sup)
    {
      ano_entering("ano::morpho::details::immersion (3d-buffer)") immersion_impl_table_t<T> impl;
      immersion_ndimage(f, inf, sup, &impl);
    }

    // Fallback for 2d-images
    template <class I>
    void immersion(ano::details::Image<I>& f, ano::box2d roi, image_concrete_t<I>& inf, image_concrete_t<I>& sup)
    {
      ano_entering("ano::morpho::details::immersion (generic)") immersion_T(static_cast<I&>(f), roi, inf, sup);
    }

    // Fallback for 3d-images (To be implemented)
    template <class I>
    void immersion(ano::details::Image<I>& f, ano::box3d roi, image_concrete_t<I>& inf, image_concrete_t<I>& sup);
    // \}


  } // namespace impl

  template <class I>
  std::pair<image_concrete_t<I>, image_concrete_t<I>> //
  immersion(I ima)
  {
    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(std::is_same_v<image_domain_t<I>, ano::box2d> || std::is_same_v<image_domain_t<I>, ano::box3d>,
                  "Input domain must be a box2d or a box3d");

    int dim = image_point_t<I>::ndim;

    // Compute the extended domain
    auto dom = ima.domain();
    for (int d = 0; d < dim; ++d)
    {
      dom.tl()[d] = dom.tl()[d] * 2;
      dom.br()[d] = dom.br()[d] * 2 - 1;
    }

    image_concrete_t<I> inf(dom);
    image_concrete_t<I> sup(dom);
    impl::immersion(ima, ima.domain(), inf, sup);

    return {std::move(inf), std::move(sup)};
  }

} // namespace ano::morpho::details
