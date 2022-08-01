#pragma once

#include "linearregression.hpp"

#include "../descriptor.hpp"
#include <scribo/private/span.hpp>


#include <Eigen/Dense>
#include <optional>
#include <utility>
#include <vector>

namespace scribo::internal
{
  /**
   * The Observation class holds an observation matrix, the t-position at which it was matched, and the number of times
   * this observation was matched.
   */
  struct Observation
  {
    Eigen::Matrix<float, 3, 1> obs;         // The observation Matrix
    int                        match_count; // The numbers of time the observation was matched to a filter_impl
    int                        t;           // The t-position at which the observation was made
  };

  /**
   * The Filter_impl struct represents an ongoing kalman filter_impl
   */
  struct Filter_impl
  {
    int first; // t value of first integration

    std::vector<int> t_values; // t values of SEGDET_NB_VALUES_TO_KEEP last integrations
    std::vector<int> n_values;

    std::vector<float> thicknesses;
    std::vector<float> luminosities;

    float current_slope;

    std::vector<Span> under_other;
    std::vector<Span> segment_spans;

    float n_min;
    float n_max;
    float sigma_position;
    float sigma_thickness;
    float sigma_luminosity;

    Eigen::Matrix<float, 4, 1> S; // state matrix {{position (n)}, {slope}, {thickness}, {luminosity}}
    Eigen::Matrix<float, 4, 1> S_predicted;
    Eigen::Matrix<float, 3, 1> X_predicted;

    std::optional<Observation>
        observation;          // matrix {{position (n)},{thickness},{luminosity}}, nullopt if none was matched
    int observation_distance; // n distance from last observation to current prediction

    int last_integration; // t value referring to the position of the last integration

    MemoryLinearRegressor reg;

    /**
     * Constructor
     * @param isHorizontal Boolean set to true if the filter_impl is horizontal
     * @param t_integration The position of the first integration
     * @param observation The filter_impl's first observation
     * @param descriptor Parameters struct
     */
    Filter_impl(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    virtual ~Filter_impl() = default;

    /**
     * The predict method, given a filter_impl f, will update its predicted state.
     */
    virtual void predict();

    /**
     * The integrate method, given a filter_impl f, will update the state with the a new value
     * @param t The position of the new last integration
     * @param descriptor All parameters
     */
    virtual void integrate(int t, const Descriptor& descriptor);


    /**
     * Update the given filter_impl if enough values by computing the standard deviations of the position, thickness and
     * luminosity vectors
     * @param f
     */
    void compute_sigmas(const Descriptor& descriptor);

    /**
     * The accepts method will check if the given filter_impl and observation are compatible
     * @param obs The observation Matrix
     * @param min The observation min value
     * @param max The observation max value
     * @return true if observation is compatible with filter_impl else false
     */
    bool accepts(const Eigen::Matrix<float, 3, 1>& obs, int min, int max, const Descriptor& descriptor) const;

    /**
     * The choose_nearest method check whether the given observation, or the one already contained in the Filter_impl is
     * the most compatible
     * @param f The filter_impl struct
     * @param obs The observation to check
     * @return The best observation for the filter_impl
     */
    std::optional<Observation> choose_nearest(Observation& obs);

    float compute_slope();
  };
} // namespace scribo::internal
