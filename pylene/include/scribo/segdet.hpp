#pragma once

#include <mln/core/image/ndimage_fwd.hpp>

#include <cstdint>
#include <map>
#include <tuple>
#include <vector>

namespace scribo
{
  /**
   * @brief e_segdet_preprocess Precise the preprocess to apply
   */
  enum class e_segdet_preprocess
  {
    NONE,          ///< None
    BLACK_TOP_HAT, ///< Black-Top-Hat with specific filter size and dynamic
  };

  enum class e_segdet_process_extraction
  {
    BINARY,   ///< Binary extraction with threshold
    GRADIENT, ///< Gradient extraction with threshold
  };

  /**
   * @brief e_segdet_process_tracking Precise which tracker is used
   */
  enum class e_segdet_process_tracking
  {
    KALMAN,                    ///< Kalman Filters following classics prediction and correction based on IRISA article
    ONE_EURO,                  ///< One Euro Filter (modification from Nicolas Roussel code)
    DOUBLE_EXPONENTIAL,        ///< Double exponential
    LAST_INTEGRATION,          ///< Last observation
    SIMPLE_MOVING_AVERAGE,     ///< Simple moving average
    EXPONENTIAL_MOVING_AVERAGE ///< Exponential moving average
  };

  /**
   * @brief e_segdet_process_traversal_mode Precise the traversal performed for line detection
   */
  enum class e_segdet_process_traversal_mode
  {
    HORIZONTAL,          ///< Only horizontal traversal is performed
    VERTICAL,            ///< Only vertical traversal is performed
    HORIZONTAL_VERTICAL, ///< Both horizontal and vertical traversal are performed
  };

  /**
   * @brief SegDetParams holds parameters of the line detection
   */
  struct SegDetParams
  {
    e_segdet_preprocess             preprocess = e_segdet_preprocess::NONE;         ///< Preprocess applied
    e_segdet_process_tracking       tracker    = e_segdet_process_tracking::KALMAN; ///< Tracker used
    e_segdet_process_traversal_mode traversal_mode =
        e_segdet_process_traversal_mode::HORIZONTAL_VERTICAL;                       ///< Traversal performed
    e_segdet_process_extraction extraction_type =
        e_segdet_process_extraction::BINARY;          ///< Extraction type for observations

    bool negate_image = false;                        ///< Say if image has to be reversed before processing

    float dyn                               = 0.6f;   ///< Dynamic when Black-Top-Hat preprocess is applied
    int   size_mask                         = 11;     ///< Filter size when Black-Top-Hat preprocess is applied
    float double_exponential_alpha          = 0.6f;   ///< Alpha used in double exponential tracker if chosen
    float simple_moving_average_memory      = 30.0f;  ///< Memory used in simple moving average tracker if chosen
    float exponential_moving_average_memory = 16.0f;  ///< Memory used in exponential moving average tracker if chosen
    float one_euro_beta                     = 0.007f; ///< Beta used in one euro tracker if chosen
    float one_euro_mincutoff                = 1.0f;   ///< Min cutoff used in one euro tracker if chosen
    float one_euro_dcutoff                  = 1.0f;   ///< Dcutoff used in one euro tracker if chosen

    int bucket_size = 32;                             ///< Bucket size during traversal

    int nb_values_to_keep = 30;                       ///< Memory of tracker to compute variances for the matching
    int discontinuity_relative =
        0; ///< Percentage. Discontinuity = discontinuity_absolute + discontinuity_relative * current_segment_size
    int discontinuity_absolute =
        0; ///< Discontinuity = discontinuity_absolute + discontinuity_relative * current_segment_size
    int minimum_for_fusion = 15; ///< Threshold to merge trackers following same observation
    int isolated_point     = 2;  ///< Threshold to say if a point is isolated and has to be removed
    unsigned int slope_checking_threshold =
        10; ///< Threshold after which the slope is taken into account to remove the segment if not in the bounderies
    unsigned int slope_max =
        50; ///< Max slope allowed in degrees. Has to be less than 90 or equal if there is not boundery

    int default_sigma_position   = 2;    ///< Position default variance value
    int default_sigma_thickness  = 2;    ///< Thickness default variance value
    int default_sigma_luminosity = 57;   ///< Luminosity default variance value

    int   min_nb_values_sigma  = 10;     ///< Threshold to compute variance and not use defauld values
    float sigma_pos_min        = 1.f;    ///< Minimum position variance value
    float sigma_thickness_min  = 0.64f;  ///< Minimum thickness variance value
    float sigma_luminosity_min = 13.f;   ///< Minimum luminosity variance value

    int   gradient_threshold = 30;       ///< Gradient threshold when gradient preprocess is applied
    int   llumi              = 225;      ///< First threshold for observation ternary extraction
    int   blumi              = 225;      ///< Second threshold for observation ternary extraction
    float ratio_lum          = 1.f;      ///< Ratio of kept luminosity in observation extraction

    int max_thickness = 100;             ///< Max allowed (vertical|horizontal) thickness of segment to detect

    float threshold_intersection = 0.8f; ///< Threshold for duplication removal
    bool  remove_duplicates      = true; ///< Say if duplication removal has to be computed

    /**
     * @brief Say if parameters values are compatible
     * @return true if parameters are valid
     */
    bool is_valid() const;
  };

  /**
   * @brief VSegment structure holding vectorial information about detected lines
   *
   */
  struct VSegment
  {
    int label; ///< Label of segment
    int x0;    ///< First coordinate of first point
    int y0;    ///< Second coordinate of first point
    int x1;    ///< First coordinate of second point
    int y1;    ///< Second coordinate of second point
  };

  /**
   * Detects lines in the given image using given parameters
   * @param image A ndbuffer representing the image to process
   * @param min_len The minimum length of segments to detect
   * @param params A struct containing the parameters of the method
   * @return A vector of detected segments
   */
  std::vector<VSegment> detect_line_vector(const mln::image2d<std::uint8_t>& image, int min_len,
                                           const SegDetParams& params = SegDetParams());

  static constexpr int first_label = 2;
  /**
   * @brief LSuperposition structure holding superposition information
   *
   */
  struct LSuperposition
  {
    int label; ///< Label of the segment superposing
    int x;     ///< First coordinate of the position of the superposition
    int y;     ///< Second coordinate of the position of the superposition
  };

  /**
   * Detects lines in the given image using given parameters
   * @param image A ndbuffer representing the image to process
   * @param min_len The minimum length of segments to detect
   * @param params A struct containing the parameters of the method
   * @return An image with pixel labelized (intersection with single label)
   */
  std::tuple<mln::image2d<uint16_t>, std::vector<LSuperposition>>
  detect_line_label(const mln::image2d<std::uint8_t>& image, int min_len, const SegDetParams& params = SegDetParams());

  /**
   * Detects lines in the given image using given parameters
   * @param image A ndbuffer representing the image to process
   * @param min_len The minimum length of segments to detect
   * @param params A struct containing the parameters of the method
   * @return Tuple with 1. Image with pixel labelized (intersection with single label)
   *                    2. Vector of detected segment
   *                    3. Vector of LSuperposition Type Intersection information
   */
  std::tuple<mln::image2d<std::uint16_t>, std::vector<LSuperposition>, std::vector<VSegment>>
  detect_line_full(const mln::image2d<std::uint8_t>& image, int min_len, const SegDetParams& params = SegDetParams());

  std::tuple<mln::image2d<std::uint8_t>, mln::image2d<std::uint8_t>, mln::image2d<std::uint8_t>>
  detect_line_pp(const mln::image2d<std::uint8_t>& image, const SegDetParams& params);
} // namespace scribo