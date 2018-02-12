#ifndef MLN_MORPHO_CANVAS_PRIVATE_DILATION_BY_PERIODIC_LINE_HPP
#define MLN_MORPHO_CANVAS_PRIVATE_DILATION_BY_PERIODIC_LINE_HPP

#include <mln/core/domain/box.hpp>
#include <mln/core/se/periodic_line2d.hpp>
#include <mln/core/trace.hpp>
#include "running_max_1d.hpp"

namespace mln
{
  namespace morpho
  {
    namespace internal
    {

      /// Copy the line starting at \p src_point with period \p period into a buffer
      template <class I, class V>
      void copy_to_periodic_line(const I& f, box2d domain, point2d src_point, point2d period, V* lineptr,
                                 std::size_t& size)
      {
        mln_precondition(period[0] >= 0 && (period[0] != 0 || period[1] != 0));
        mln_precondition(domain.has(src_point));

        const int dy = period[0];
        const int dx = period[1];

        const auto height = domain.pmax[0] - src_point[0];
        const auto width = dx > 0 ? (domain.pmax[1] - src_point[1]) : (src_point[1] - domain.pmin[1] + 1);
        const int nx = dx != 0 ? ((width - 1) / std::abs(dx)) : std::numeric_limits<int>::max();
        const int ny = dy != 0 ? ((height - 1) / std::abs(dy)) : std::numeric_limits<int>::max();
        const std::size_t n = std::min(nx, ny) + 1;

        point2d p = src_point;
        for (std::size_t i = 0; i < n; ++i, p += period)
          lineptr[i] = f(p);

        size = n;
      }

      /// Copy the buffer of the line of length \p size
      /// to the output image at \p pdest
      template <class J, class V>
      void copy_from_periodic_line(const V* lineptr, std::size_t size, point2d period, point2d pdest, J& output)
      {
        point2d p = pdest;
        for (std::size_t i = 0; i < size; ++i, p += period)
          output(p) = lineptr[i];
      }

      // Special algorithm for dilation by line
      // The algorithm runs inplace
      template <class I, class BinaryFunction>
      void dilation_by_periodic_line(I& f, const mln::se::periodic_line2d& line, BinaryFunction sup, mln_value(I) zero)
      {
        mln_entering("mln::morpho::internal::dilation_by_periodic_line");
        auto domain = f.domain();
        using V = mln_value(I);

        auto period = line.period();

        const int dy = period[0];
        const int dx = period[1];

        mln_assertion(dy >= 0);

        int buffer_size = std::max(domain.shape()[0], domain.shape()[1]);
        auto p = std::make_unique<V[]>(buffer_size); // temporary 1D-line
        auto g = std::make_unique<V[]>(buffer_size); // Max-forward integral over the line
        auto h = std::make_unique<V[]>(buffer_size); // Max-backward integral over the line
        int k = line.repetition();

        // Process row-wise
        if (dx != 0)
        {
          // Depending on dx, we process from left border or the right border
          int x0 = (dx > 0) ? domain.pmin[1] : domain.pmax[1] - 1;

          for (int i = std::abs(dx) - 1; i >= 0; --i)
          {
            int x = x0 + (dx > 0 ? i : -i);
            for (int y = domain.pmin[0]; y < domain.pmax[0]; ++y)
            {
              point2d origin = {(short)y, (short)x};
              std::size_t n;
              copy_to_periodic_line(f, domain, origin, period, p.get(), n);
              mln::morpho::internal::running_max_1d(p.get(), g.get(), h.get(), (int)n, 2 * k + 1, -k, sup, zero);
              copy_from_periodic_line(p.get(), n, period, origin, f);
            }
          }
        }

        // Process col-wise
        if (dy != 0)
        {
          // We always process from the top border (because dy > 0)
          auto y0 = domain.pmin[0];
          int xstart = (dx > 0) ? domain.pmin[1] + dx : domain.pmin[1];
          int xend = (dx >= 0) ? domain.pmax[1] : domain.pmax[1] + dx;
          for (int i = 0; i < dy; ++i)
          {
            int y = y0 + i;
            for (int x = xstart; x < xend; ++x)
            {
              point2d origin = {(short)y, (short)x};
              std::size_t n;
              copy_to_periodic_line(f, domain, origin, period, p.get(), n);
              mln::morpho::internal::running_max_1d(p.get(), g.get(), h.get(), (int)n, 2 * k + 1, -k, sup, zero);
              copy_from_periodic_line(p.get(), n, period, origin, f);
            }
          }
        }
      }

    } // end of namespace mln::morpho::internal
  }   // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_CANVAS_PRIVATE_DILATION_BY_PERIODIC_LINE_HPP
