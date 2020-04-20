#include <mln/io/imprint.hpp>

namespace mln::io::internal
{

  void imprint2d(const formatter<mln::point2d>& fmter, mln::box2d roi)
  {
    // Compute column width
    int width = 0;
    mln_foreach_new (auto p, roi)
    {
      int w = static_cast<int>(fmter.formatted_size(p));
      if (width < w)
        width = w;
    }

    // Print data
    auto rows = ranges::rows(roi);
    for (auto&& r : rows)
    {
      for (mln::point2d p : r)
        fmter.print(p, width + 1);
      fmt::print("\n");
    }
  }

  void imprint3d(const formatter<mln::point3d>& fmter, mln::box3d roi)
  {
    // Compute column width
    int width = 0;
    mln_foreach_new (auto p, roi)
    {
      int w = static_cast<int>(fmter.formatted_size(p));
      if (width < w)
        width = w;
    }

    // Print data
    int zmin = roi.tl().z();
    int zmax = roi.br().z();
    int ymin = roi.tl().y();
    int ymax = roi.br().y();
    int xmin = roi.tl().x();
    int xmax = roi.br().x();

    for (int z = zmin; z < zmax; ++z)
    {
      for (int y = ymin; y < ymax; ++y)
      {
        for (int x = xmin; x < xmax; ++x)
          fmter.print({x, y, z}, width + 1);
        fmt::print("\n");
      }
      fmt::print("========\n");
    }
  }
} // namespace mln::io::internal
