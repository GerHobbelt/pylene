#pragma once

#include <ano/core/image/ndimage.hpp>

#include <memory>
#include <span>


namespace ano::transforms
{

  class HoughLineAccumulator
  {
    std::unique_ptr<float[]> m_sin_angles;
    std::unique_ptr<float[]> m_cos_angles;
    std::size_t              m_count;
    int                      m_max_distance;
    ano::ndbuffer_image      m_acc;

    void _init(std::span<float> angles, float max_distance);

  public:
    HoughLineAccumulator(std::span<float> angles, float max_distance, bool weighted);
    HoughLineAccumulator(std::span<float> angles, float max_distance, ano::ndbuffer_image* acc);

    void take(point2d p) noexcept;
    void take(point2d p, float w) noexcept;

    image2d<int>   get_accumulator_image() const { return m_acc.__cast<int, 2>(); }
    image2d<float> get_waccumulator_image() const { return m_acc.__cast<float, 2>(); }
  };


  struct HoughLine
  {
    float angle;
    float radius;
    float count;
  };

  struct HoughLineAngle
  {
    float angle;
    float count;
  };


  ///
  /// @brief Compute the hough transform a binary image and returns the vote image
  ///
  /// @param input The input binary image
  /// @param angles The angles (in radians) for which to vote (of length W)
  /// @param out (optional) The output image of size (W × H) where the vote will be added. It is usefull for example if
  /// you want α and α ± π/2 to vote for the same thing.
  /// @return image2d<int> The output image of size (W × H) where H is the length of the diagonal
  image2d<int> hough_lines(const image2d<bool>& input, std::span<float> angles, image2d<int>* out = nullptr);

  /// @brief Compute the weighted hough transform of an image and returns the vote image.
  ///
  /// Each non-zero pixel votes with weight equals to its value.
  ///
  /// @param input The input image
  /// @param angles The angles (in radians) for which to vote (of length W)
  /// @param out (optional) The output image of size (W × H) where the vote will be added. It is usefull for example if
  /// you want α and α ± π/2 to vote for the same thing.
  /// @return image2d<float> The output image of size (W × H) where H is the length of the diagonal
  image2d<float> hough_lines(const image2d<float>& input, std::span<float> angles, image2d<float>* out = nullptr);

  ///
  /// @brief Detect the peaks in a Hough vote image
  ///
  /// @param acc The vote image of size (W x H)
  /// @param angles The angles used to generate the vote image (of length W)
  /// @param instensity_reject The percentage of the global max peak value to consider the local max as a peak
  /// @param min_sep_distance Size of the NMS window (units of radius in pixels)
  /// @param min_sep_angle Size of the NMS window (units of angles)
  /// @param max_num_peaks The maximum number of peaks returned
  /// @return std::vector<HoughLine> The peaks of the vote image ordered by vote
  std::vector<HoughLine> hough_lines_detect_peaks(const image2d<float>& acc,                     //
                                                  std::span<float>      angles,                  //
                                                  float                 intensity_reject = 0.5f, //
                                                  int                   min_sep_distance = 5,    //
                                                  int                   min_sep_angle    = 5,    //
                                                  int                   max_num_peaks    = 10);

  std::vector<HoughLine> hough_lines_detect_peaks(const image2d<int>& acc,                     //
                                                  std::span<float>    angles,                  //
                                                  float               intensity_reject = 0.5f, //
                                                  int                 min_sep_distance = 5,    //
                                                  int                 min_sep_angle    = 5,    //
                                                  int                 max_num_peaks    = 10);


  ///
  /// @brief Detect the main angles in a Hough vote image
  ///
  /// @param acc The vote image of size (W x H)
  /// @param angles The angles used to generate the vote image (of length W)
  /// @param instensity_reject The percentage of the global max peak value to consider the local max as a peak
  /// @param min_sep_distance Size of the NMS window (units of radius in pixels)
  /// @param min_sep_angle Size of the NMS window (units of angles)
  /// @return std::vector<HoughLineAngle> The main directions in the image
  std::vector<HoughLineAngle> hough_lines_detect_peak_angles(const image2d<float>& acc, //
                                                             std::span<float>      angles,
                                                             float                 intensity_reject = 0.5f, //
                                                             int                   min_sep_distance = 5,    //
                                                             int                   min_sep_angle    = 5);

  std::vector<HoughLineAngle> hough_lines_detect_peak_angles(const image2d<int>& acc, //
                                                             std::span<float>    angles,
                                                             float               intensity_reject = 0.5f, //
                                                             int                 min_sep_distance = 5,    //
                                                             int                 min_sep_angle    = 5);

} // namespace ano::transforms
