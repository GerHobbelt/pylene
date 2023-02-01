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
    int                        match_count; // The numbers of time the observation was matched to a tracker_impl
    int                        t;           // The t-position at which the observation was made
    size_t                     id;          // The id of the observation (used for fusion)
  };

  /**
   * The Tracker_impl struct represents an ongoing kalman tracker_impl
   */
  struct Tracker_impl
  {
    int first; // t value of first integration

    std::vector<float> t_values; // t values of SEGDET_NB_VALUES_TO_KEEP last integrations
    std::vector<float> n_values;
    std::vector<float> thicknesses;
    std::vector<float> luminosities;

    float current_slope; // Allowed the handling of early stop of tracker

    std::vector<Span> under_other;
    std::vector<Span> segment_spans;

    float n_min;
    float n_max;
    float sigma_position;
    float sigma_thickness;
    float sigma_luminosity;

    Eigen::Matrix<float, 3, 1> X_predicted;

    std::optional<Observation>
                        observation; // matrix {{position (n)},{thickness},{luminosity}}, nullopt if none was matched
    int                 observation_distance; // n distance from last observation to current prediction
    std::vector<size_t> same_observation;

    int last_integration; // t value referring to the position of the last integration

    MemoryLinearRegressor reg;

    /**
     * Constructor
     * @param isHorizontal Boolean set to true if the tracker_impl is horizontal
     * @param t_integration The position of the first integration
     * @param observation The tracker_impl's first observation
     * @param descriptor Parameters struct
     */
    Tracker_impl(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    virtual ~Tracker_impl() = default;

    /**
     * The predict method, given a tracker_impl f, will update its predicted state.
     */
    virtual void predict();

    /**
     * The integrate method, given a tracker_impl f, will update the state with the a new value
     * @param t The position of the new last integration
     * @param descriptor All parameters
     */
    virtual void integrate(int t, const Descriptor& descriptor);


    /**
     * Update the given tracker_impl if enough values by computing the standard deviations of the position, thickness
     * and luminosity vectors
     * @param f
     */
    void compute_sigmas(const Descriptor& descriptor);

    /**
     * The accepts method will check if the given tracker_impl and observation are compatible
     * @param obs The observation Matrix
     * @param min The observation min value
     * @param max The observation max value
     * @return true if observation is compatible with tracker_impl else false
     */
    bool accepts(const Eigen::Matrix<float, 3, 1>& obs, int min, int max, const Descriptor& descriptor) const;

    /**
     * The choose_nearest method check whether the given observation, or the one already contained in the Tracker_impl is
     * the most compatible
     * @param f The tracker_impl struct
     * @param obs The observation to check
     * @return The best observation for the tracker_impl
     */
    std::optional<Observation> choose_nearest(Observation& obs);

    float compute_slope();
  };
} // namespace scribo::internal
