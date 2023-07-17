#include <mln/core/canvas/private/traverse2d.hpp>
#include <mln/core/image/ndimage.hpp>

namespace mln::canvas::details
{

  void traverse_along_direction(mln::box2d roi,
                                mln::point2d direction,
                                std::function<void(mln::point2d, mln::point2d, std::size_t n)> callback)
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

    auto count = [](int start, int end, int step) { return start >= end ? 0 : ((end - start - 1) / step + 1); };

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


  /// \brief Traverse an image and calls f with a pointer to the data of each line
  void apply_line(mln::ndbuffer_image& input, std::function<void(std::byte*)> fun)
  {
    int pdim = input.pdim();

    if (pdim > 4)
      throw std::runtime_error("Not implemented for this number of dimensions.");


    std::ptrdiff_t strides[4] = {0};
    int            dims[4]    = {1, 1, 1, 1};

    for (int k = 0; k < pdim; ++k)
    {
      dims[k] = input.size(k);
      strides[k] = input.byte_stride(k);
    }


    for (int w = 0; w < dims[3]; ++w)
      for (int z = 0; z < dims[2]; ++z)
      {
        std::byte* lineptr = input.buffer() + w * strides[3] + z * strides[2];
        for (int y = 0; y < dims[1]; ++y)
        {
          fun(lineptr);
          lineptr += strides[1];
        }
      }
  }
} // namespace mln::canvas::details
