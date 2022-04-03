#include <mln/transforms/hough_lines.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/filters/nms.hpp>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <map>
#include <version>

#include <fmt/core.h>



namespace mln::transforms
{


  HoughLineAccumulator::HoughLineAccumulator(std::span<float> angles, float max_distance, image2d<int>* acc)
  {
    m_count = angles.size();
    m_sin_angles = std::unique_ptr<float[]>(new float[m_count]);
    m_cos_angles = std::unique_ptr<float[]>(new float[m_count]);

    std::ranges::transform(angles, m_sin_angles.get(), [](float x) { return std::sin(x); });
    std::ranges::transform(angles, m_cos_angles.get(), [](float x) { return std::cos(x); });

    m_max_distance = static_cast<int>(std::ceil(max_distance));

    if (acc == nullptr)
    {
      m_acc.resize(static_cast<int>(m_count), 2 * m_max_distance + 1, image_build_params{
          .border = 0,
          .init_value = 0
        });
    }
    else
    {
      m_acc = *acc;
      if ((m_acc.width() != (int)m_count) || (m_acc.height() != (2 * m_max_distance + 1)))
      {
        auto msg = fmt::format(
          "Invalid accumulator image (bad dimensions)\n"
          "Expected (w={},h={}), got (w={},h={})", m_count, 2 * m_max_distance + 1,
          m_acc.width(), m_acc.height());
        throw std::runtime_error(msg);
      }
    }
  }

  void HoughLineAccumulator::take(point2d p) noexcept
  {
    int offset = m_max_distance;
    for (std::size_t i = 0; i < m_count; ++i)
    {
      int radius = std::round(m_sin_angles[i] * p.x() + m_cos_angles[i] * p.y());
      assert(-m_max_distance <= radius && radius <= m_max_distance);
      m_acc({(int)i, offset + radius})++;
    }
  }


  mln::image2d<int> hough_lines(const image2d<bool>& input, std::span<float> angles, image2d<int>* out)
  {
    float                max_distance = std::hypot(input.width(), input.height());
    HoughLineAccumulator hl{angles, max_distance, out};

    mln_foreach(auto px, input.pixels())
      if (px.val())
        hl.take(px.point());

    return hl.get_accumulator_image();
  }

  std::vector<HoughLine> //
  hough_lines_detect_peaks(const image2d<int>& acc, std::span<float> angles, float reject, int min_sep_distance, int min_sep_angle,
                           int num_peaks)
  {
    image2d<int> nms_image;

    int  minv = static_cast<int>(reject * mln::accumulate(acc, accu::accumulators::max<int>()));
    auto res = mln::filters::nms(acc, min_sep_angle, min_sep_distance, minv, num_peaks, &nms_image);

    float                  offset = acc.height() / 2;
    std::vector<HoughLine> out(res.size());
    std::ranges::transform(res, out.begin(), [&, offset](point2d p) {
      return HoughLine{.angle  = angles[p.x()],  //
                       .radius = p.y() - offset, //
                       .count  = nms_image(p)};
    });

    std::ranges::sort(out, [](auto a, auto b) { return a.count > b.count; });
    return out;
  }

  /// \brief Detect the main angles in a Hough vote image
  std::vector<HoughLineAngle> hough_lines_detect_peak_angles(const image2d<int>& acc,              //
                                                             std::span<float>    angles,           //
                                                             float               reject,           //
                                                             int                 min_sep_distance, //
                                                             int                 min_sep_angle)

  {
    auto lines = hough_lines_detect_peaks(acc, angles, reject, min_sep_distance, min_sep_angle, -1);


    std::map<float, int> counts;
    for (auto [a,r,c] : lines)
      counts[a] += c;

    std::vector<HoughLineAngle> result(counts.size());
    std::ranges::transform(counts, result.begin(), [](std::pair<float, int> e) { return HoughLineAngle{e.first, e.second}; });
    std::ranges::sort(result, [](auto a, auto b) { return a.count > b.count; });

    return result;
  }
}
