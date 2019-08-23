#include <mln/core/canvas/private/traverse2d.hpp>

namespace mln::canvas::details
{

  void traverse_along_direction(mln::experimental::box2d roi,
                                mln::experimental::point2d direction,
                                std::function<void(mln::experimental::point2d, mln::experimental::point2d, std::size_t n)> callback)
  {
    if (roi.empty())
      return;

    // Make the direction downward the y-axis
    if (direction.y() < 0)
      direction = -direction;



    int dx   = direction.x();
    int dy   = direction.y();
    int ymin = roi.tl().y();
    int ymax = roi.br().y();
    int xmin = roi.tl().x();
    int xmax = roi.br().x();

    assert(dx != 0 || dy != 0);

    auto count = [](int start, int end, int step) { return (end - start - 1) / step + 1; };

    // Top border pixels
    if (dy != 0)
    {
      for (int k = 0; k < dy; k++)
      {
        int c1 = count(ymin + k, ymax, dy);
        for (int x = xmin; x < xmax; ++x)
        {

          if (dx == 0)
            callback({x, ymin + k}, direction, c1);
          else
          {
            int c2 = (dx > 0) ? count(x, xmax, dx) : count(xmin, x + 1, -dx);
            callback({x, ymin + k}, direction, std::min(c1, c2));
          }
        }
      }
      ymin += dy;
    }

    // Left or right border
    if (dx != 0)
    {
      int x  = (dx > 0) ? xmin : xmax - 1;
      int dk = (dx > 0) ? 1 : -1;

      for (int k = 0; k != dx; k += dk)
      {
        int c1 = count(xmin + std::abs(k), xmax, std::abs(dx));

        for (int y = ymin; y < ymax; ++y)
        {
          if (dy == 0)
            callback({x + k, y}, direction, c1);
          else
          {
            int c2 = count(y, ymax, dy);
            callback({x + k, y}, direction, std::min(c1, c2));
          }
        }
      }
    }
  }
}
