#include <ano/io/imprint.hpp>

namespace ano::io::internal
{

  void imprint2d(const formatter<ano::point2d>& fmter, ano::box2d roi)
  {
    // Compute column width
    int width = 0;
    ano_foreach (auto p, roi)
    {
      int w = static_cast<int>(fmter.formatted_size(p));
      if (width < w)
        width = w;
    }

    // Print data
    auto rows = ranges::rows(roi);
    for (auto&& r : rows)
    {
      for (ano::point2d p : r)
        fmter.print(p, width + 1);
      fmt::print("\n");
    }
  }

  void imprint3d(const formatter<ano::point3d>& fmter, ano::box3d roi)
  {
    // Compute column width
    int width = 0;
    ano_foreach (auto p, roi)
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
} // namespace ano::io::internal
