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

  void HoughLineAccumulator::_init(std::span<float> angles, float max_distance)
  {
    m_count = angles.size();
    m_sin_angles = std::unique_ptr<float[]>(new float[m_count]);
    m_cos_angles = std::unique_ptr<float[]>(new float[m_count]);

    std::transform(angles.begin(), angles.end(), m_sin_angles.get(), [](float x) { return std::sin(x); });
    std::transform(angles.begin(), angles.end(), m_cos_angles.get(), [](float x) { return std::cos(x); });

    m_max_distance = static_cast<int>(std::ceil(max_distance));
  }

  HoughLineAccumulator::HoughLineAccumulator(std::span<float> angles, float max_distance, mln::ndbuffer_image* acc)
  {
    this->_init(angles, max_distance);
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

  HoughLineAccumulator::HoughLineAccumulator(std::span<float> angles, float max_distance, bool weighted)
  {
    this->_init(angles, max_distance);

    auto bparams = image_build_params {
      .border = 0,
      .init_value = weighted ? std::any(0.f) : std::any(int(0))
    };
    int w = static_cast<int>(m_count);
    int h = static_cast<int>(2 * m_max_distance + 1);

    if (weighted)
      m_acc = mln::image2d<float>(w, h, bparams);
    else
      m_acc = mln::image2d<int>(w, h, bparams);
  }

  void HoughLineAccumulator::take(point2d p) noexcept
  {
    int offset = m_max_distance;
    auto& acc = m_acc.__cast<int, 2>();
    for (std::size_t i = 0; i < m_count; ++i)
    {
      int radius = std::round(m_sin_angles[i] * p.x() + m_cos_angles[i] * p.y());
      assert(-m_max_distance <= radius && radius <= m_max_distance);
      acc({(int)i, offset + radius}) += 1;
    }
  }

  void HoughLineAccumulator::take(point2d p, float w) noexcept
  {
    int offset = m_max_distance;
    auto& acc = m_acc.__cast<float, 2>();
    for (std::size_t i = 0; i < m_count; ++i)
    {
      int radius = std::round(m_sin_angles[i] * p.x() + m_cos_angles[i] * p.y());
      assert(-m_max_distance <= radius && radius <= m_max_distance);
      acc({(int)i, offset + radius}) += w;
    }
  }


  mln::image2d<int> hough_lines(const image2d<bool>& input, std::span<float> angles, image2d<int>* out)
  {
    float max_distance = std::hypot(input.width(), input.height());
    auto hl = out ? HoughLineAccumulator{angles, max_distance, out} : HoughLineAccumulator{angles, max_distance, false};

    mln_foreach(auto px, input.pixels())
      if (px.val())
        hl.take(px.point());

    return hl.get_accumulator_image();
  }


  mln::image2d<float> hough_lines(const image2d<float>& input, std::span<float> angles, image2d<float>* out)
  {
    float max_distance = std::hypot(input.width(), input.height());
    auto hl = out ? HoughLineAccumulator{angles, max_distance, out} : HoughLineAccumulator{angles, max_distance, true};

    mln_foreach(auto px, input.pixels())
      if (px.val() > 0)
        hl.take(px.point(), px.val());

    return hl.get_waccumulator_image();
  }

  namespace
  {

    template <class T>
    std::vector<HoughLine> //
    hough_lines_detect_peaks_T(const image2d<T>& acc, std::span<float> angles, float reject, int min_sep_distance,
                               int min_sep_angle, int num_peaks)
    {
      image2d<T> nms_image;

      T    minv = static_cast<T>(reject * mln::accumulate(acc, accu::accumulators::max<T>()));
      auto res  = mln::filters::nms(acc, min_sep_angle, min_sep_distance, minv, num_peaks, &nms_image);

      float                  offset = acc.height() / 2;
      std::vector<HoughLine> out(res.size());
      std::transform(res.begin(), res.end(), out.begin(), [&, offset](point2d p) {
        return HoughLine{.angle  = angles[p.x()],  //
                         .radius = p.y() - offset, //
                         .count  = float(nms_image(p))};
      });

      std::sort(out.begin(), out.end(), [](auto a, auto b) { return a.count > b.count; });
      return out;
    }

    /// \brief Detect the main angles in a Hough vote image
    std::vector<HoughLineAngle> hough_lines_detect_peak_angles_T(const ndbuffer_image& _acc,              //
                                                                 std::span<float>  angles,           //
                                                                 float             reject,           //
                                                                 int               min_sep_distance, //
                                                                 int               min_sep_angle)

    {
      
      std::vector<HoughLine> lines;
      if (auto* acc = _acc.cast_to<int, 2>())
        lines = hough_lines_detect_peaks(*acc, angles, reject, min_sep_distance, min_sep_angle, -1);
      else if (auto* acc = _acc.cast_to<float, 2>())
        lines = hough_lines_detect_peaks(*acc, angles, reject, min_sep_distance, min_sep_angle, -1);
      else
        throw std::runtime_error("Invalid input image type.");

      std::map<float, float> counts;
      for (auto [a, r, c] : lines)
        counts[a] += c;

      std::vector<HoughLineAngle> result(counts.size());
      std::transform(counts.begin(), counts.end(), result.begin(), [](std::pair<float, float> e) {
        return HoughLineAngle{e.first, e.second};
      });
      std::sort(result.begin(), result.end(), [](auto a, auto b) { return a.count > b.count; });

      return result;
    }

  } // namespace

  std::vector<HoughLine> //
  hough_lines_detect_peaks(const image2d<int>& acc, std::span<float> angles, float reject, int min_sep_distance,
                           int min_sep_angle, int num_peaks)
  {
    return hough_lines_detect_peaks_T(acc, angles, reject, min_sep_distance, min_sep_angle, num_peaks);
  }

  std::vector<HoughLine> //
  hough_lines_detect_peaks(const image2d<float>& acc, std::span<float> angles, float reject, int min_sep_distance,
                           int min_sep_angle, int num_peaks)
  {
    return hough_lines_detect_peaks_T(acc, angles, reject, min_sep_distance, min_sep_angle, num_peaks);
  }

 

    /// \brief Detect the main angles in a Hough vote image
  std::vector<HoughLineAngle> //
  hough_lines_detect_peak_angles(const image2d<float>& acc, std::span<float> angles, float reject, int min_sep_distance,
                                 int min_sep_angle)

  {
    return hough_lines_detect_peak_angles_T(acc, angles, reject, min_sep_distance, min_sep_angle);
  }

      /// \brief Detect the main angles in a Hough vote image
  std::vector<HoughLineAngle> //
  hough_lines_detect_peak_angles(const image2d<int>& acc, std::span<float> angles, float reject, int min_sep_distance,
                                 int min_sep_angle)

  {
    return hough_lines_detect_peak_angles_T(acc, angles, reject, min_sep_distance, min_sep_angle);
  }
}
