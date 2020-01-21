#pragma once



#include <mln/core/box.hpp>
#include <mln/core/se/periodic_line2d.hpp>
#include <mln/core/trace.hpp>
#include <mln/morpho/experimental/private/running_max_1d.hpp>
#include <mln/morpho/experimental/private/dilation_vertical_block2d.hpp>
#include <mln/core/canvas/private/traverse2d.hpp>


namespace mln::morpho::details
{

  /// \param[in] in Input image
  /// \param[out] out Output image (can be the same as \p in for inplace version)
  /// \param[in] line Line structuring element
  /// \param[in] sup Supremum operator
  /// \param[in] roi Processing roi
  template <class I, class J, class BinaryFunction>
  [[gnu::noinline]] void dilation_by_periodic_line(I& in, J& out,
                                                   const mln::experimental::se::periodic_line2d& line,
                                                   BinaryFunction sup,
                                                   mln::experimental::box2d roi);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  // fixme: could be optimized for indexable images
  template <class I>
  [[gnu::noinline]] void copy_to_periodic_line(I& f,
                                               mln::experimental::point2d origin,
                                               mln::experimental::point2d direction,
                                               std::size_t n,
                                               image_value_t<I>* __restrict buffer)
  {
    auto p = origin;
    for (std::size_t i = 0; i < n; ++i, p += direction)
      buffer[i] = f.at(p);
  }

  // fixme: could be optimized for indexable images
  template <class J>
  [[gnu::noinline]] void copy_from_periodic_line(const image_value_t<J>* __restrict buffer,
                                                 mln::experimental::point2d origin,
                                                 mln::experimental::point2d direction,
                                                 std::size_t n,
                                                 J output)
  {
    auto p = origin;
    for (std::size_t i = 0; i < n; ++i, p += direction)
      output(p) = buffer[i];
  }





  // Generic implementation
  template <class I, class J, class BinaryFunction>
  void dilation_by_periodic_line_generic(I& in, J& out,
                                         const mln::experimental::se::periodic_line2d& line,
                                         BinaryFunction sup,
                                         mln::experimental::box2d roi)
  {
    using V = image_value_t<I>;

    int       k      = line.repetition();
    auto      period = line.period();

    // Some sanity check
    {
      assert(period.y() >= 0);
      assert(out.domain().includes(roi));
      assert(in.domain().includes(roi));
    }

    int buffer_size = std::max(roi.width(), roi.height()) + 2 * k;

    auto p = std::make_unique<V[]>(buffer_size); // temporary 1D-line
    auto g = std::make_unique<V[]>(buffer_size); // Max-forward integral over the line
    auto h = std::make_unique<V[]>(buffer_size); // Max-backward integral over the line


    // By supplying the extension mode (user or fill), we can speed up and avoid computing
    // the extension values.

    // std::fill_n(g.get(), buffer_size, V(0));
    // std::fill_n(h.get(), buffer_size, V(0));

    auto fun = [&, k](mln::experimental::point2d origin, mln::experimental::point2d dir, std::size_t n) {
      copy_to_periodic_line(in, origin - k * period, dir, n + 2 * k, p.get());
      //copy_to_periodic_line(in, origin, dir, n, p.get() + k);
      mln::morpho::experimental::details::running_max_1d(p.get() + k, g.get() + k, h.get() + k, (int)n, k, sup);
      copy_from_periodic_line(p.get() + k, origin, dir, n, out);
    };

    mln::canvas::details::traverse_along_direction(roi, period, fun);
  }

  // Dispatch to generic version
  template <class I, class J, class BinaryFunction>
  void dilation_by_periodic_line(I& in, J& out,
                                 const mln::experimental::se::periodic_line2d& line,
                                 BinaryFunction sup,
                                 mln::experimental::box2d roi)
  {
    dilation_by_periodic_line_generic(in, out, line, sup, roi);
  }


  template <class I, class T, class BinaryFunction>
  std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>
  dilation_by_periodic_line(I& in, mln::experimental::image2d<T>& out,
                            const mln::experimental::se::periodic_line2d& line, BinaryFunction sup,
                            mln::experimental::box2d roi)
  {
    int                   k      = line.repetition();
    [[maybe_unused]] auto period = line.period();


    // Some sanity check
    {
      assert(period.y() >= 0);
      assert(out.domain().includes(roi));
      assert(in.domain().includes(roi));
    }

    // Specialization for vertical line
    if (line.is_vertical())
    {
      mln_entering("Running specialization for vertical dilation over 2d buffer with arithmetic types");

      mln::morpho::experimental::details::vertical_running_max_algo_t<T, BinaryFunction> alg(sup);
      alg.running_max_v2d(in, out, roi, k, true);
      return;
    }
    else if (line.is_horizontal())
    {
      mln_entering("Running specialization for horizontal dilation over 2d buffer with arithmetic types");

      mln::morpho::experimental::details::vertical_running_max_algo_t<T, BinaryFunction> alg(sup);
      alg.running_max_h2d(in, out, roi, k, true);
      return;
    }

    dilation_by_periodic_line_generic(in, out, line, sup, roi);
  }



} // namespace mln::morpho::internal
