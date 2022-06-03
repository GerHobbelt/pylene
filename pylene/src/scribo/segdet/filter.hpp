#pragma once

#include "linearregression.hpp"

#include <scribo/segdet.hpp>

#include <Eigen/Dense>
#include <optional>
#include <utility>
#include <vector>

namespace scribo::internal
{
  struct Span;
  /**
   * The Observation class holds an observation matrix, the t-position at which it was matched, and the number of times
   * this observation was matched.
   */
  struct Observation
  {
    Eigen::Matrix<float, 3, 1> obs;         // The observation Matrix
    int                        match_count; // The numbers of time the observation was matched to a filter
    int                        t;           // The t-position at which the observation was made
  };

  /**
   * The Filter struct represents an ongoing kalman filter
   */
  struct Filter
  {
    int first; // t value of first integration

    std::vector<int> t_values; // t values of SEGDET_NB_VALUES_TO_KEEP last integrations
    std::vector<int> n_values;

    std::vector<float>   thicknesses;
    std::vector<float>   luminosities;
    float                last_slope;
    float                current_slope;

    std::vector<Span> under_other;
    int               currently_under_other;
    std::vector<Span> segment_spans;

    Eigen::Matrix<float, 4, 1> S; // state matrix {{position (n)}, {slope}, {thickness}, {luminosity}}
    Eigen::Matrix<float, 4, 1> W; // noise matrix
    Eigen::Matrix<float, 4, 4> H; // S prediction error variance matrix

    float n_min;
    float n_max;
    float sigma_position;
    float sigma_thickness;
    float sigma_luminosity;

    Eigen::Matrix<float, 4, 1> S_predicted;
    Eigen::Matrix<float, 3, 1> X_predicted;
    std::optional<Observation>
        observation;          // matrix {{position (n)},{thickness},{luminosity}}, nullopt if none was matched
    int observation_distance; // n distance from last observation to current prediction

    int last_integration; // t value referring to the position of the last integration

    MemoryLinearRegressor reg;

    /**
     * Constructor
     * @param isHorizontal Boolean set to true if the filter is horizontal
     * @param t_integration The position of the first integration
     * @param observation The filter's first observation
     * @param params Parameters struct
     */
    Filter(int t_integration, Eigen::Matrix<float, 3, 1> observation, const scribo::SegDetParams& params);
  };

  /**
   * The predict method, given a filter f, will update its predicted state
   * @param f The filter for which to update
   */
  void predict(Filter& f);

  /**
   * Update the given filter if enough values by computing the standard deviations of the position, thickness and
   * luminosity vectors
   * @param f
   */
  void compute_sigmas(Filter& f, const SegDetParams& params);

  /**
   * The accepts method will check if the given filter and observation are compatible
   * @param filter The filter struct
   * @param obs The observation Matrix
   * @param min The observation min value
   * @param max The observation max value
   * @return true if observation is compatible with filter else false
   */
  bool accepts(const Filter& filter, const Eigen::Matrix<float, 3, 1>& obs, int min, int max, const SegDetParams& params);

  /**
   * The choose_nearest method check whether the given observation, or the one already contained in the Filter is the
   * most compatible
   * @param f The filter struct
   * @param obs The observation to check
   * @return The best observation for the filter
   */
  std::optional<Observation> choose_nearest(Filter& f, Observation& obs);

  /**
   * The integrate method, given a filter f, will update the state with the a new value
   * @param f The filter struct
   * @param t The position of the new last integration
   */
  void integrate(Filter& f, int t, const SegDetParams& params);
} // namespace mln::contrib::segdet
