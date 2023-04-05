#include "tracker_impl.hpp"

#include <algorithm>
#include <numeric>

namespace scribo::internal
{
  struct Descriptor;

  /**
   * Compute the standard deviation of the diven vector
   * @tparam T The type of a vector
   * @param vec The given vector
   * @return A float that is the standard deviation of the series
   */
  template <typename T>
  float std(const std::vector<T>& vec)
  {
    const size_t sz = vec.size();
    if (sz == 1)
    {
      return 0.f;
    }

    // Calculate the mean
    const float mean = std::accumulate(vec.begin(), vec.end(), 0.f) / sz;

    // Now calculate the variance
    auto variance_func = [mean](float accumulator, const float val) {
      return accumulator + ((val - mean) * (val - mean));
    };

    auto variance = std::accumulate(vec.begin(), vec.end(), 0.f, variance_func) / (sz);

    auto result = sqrt(variance);

    return result;
  }

  Tracker_impl::Tracker_impl(int t_integration, Eigen::Matrix<float, 3, 1> obs, const Descriptor& descriptor)
    : observation(std::nullopt)
    , observation_distance(0)
    , last_integration(t_integration)
    , reg(t_integration, obs(0, 0))
  {
    first    = t_integration;
    t_values = std::vector<float>({static_cast<float>(t_integration)});
    n_values = std::vector<float>({obs(0, 0)});

    thicknesses  = std::vector<float>({obs(1, 0)});
    luminosities = std::vector<float>({obs(2, 0)});

    t_values.reserve(descriptor.nb_values_to_keep);
    n_values.reserve(descriptor.nb_values_to_keep);
    thicknesses.reserve(descriptor.nb_values_to_keep);
    luminosities.reserve(descriptor.nb_values_to_keep);

    current_slope = 0;

    under_other = std::vector<Span>();

    n_min = 0;
    n_max = 0;

    sigma_position   = descriptor.default_sigma_position * 3;
    sigma_thickness  = descriptor.default_sigma_thickness * 3;
    sigma_luminosity = descriptor.default_sigma_luminosity * 3;

    X_predicted = Eigen::Matrix<float, 3, 1>::Zero();

    segment_spans = std::vector<Span>();
  }

  void Tracker_impl::compute_sigmas(const Descriptor& descriptor)
  {
    if (static_cast<int>(n_values.size()) > descriptor.min_nb_values_sigma)
    {
      sigma_position   = (std(n_values) + descriptor.sigma_pos_min) * 3;
      sigma_thickness  = (std(thicknesses) * 2 + descriptor.sigma_thickness_min) * 3;
      sigma_luminosity = (std(luminosities) + descriptor.sigma_luminosity_min) * 3;
    }
  }


  /**
   * Checks if the given observation is in the interval prediction +/- 3 sigma
   * @param prediction The prediction value
   * @param observation The observation value
   * @param sigma The standard deviation value
   * @return true if it is in the interval, else false
   */
  bool accepts_sigma(float prediction, float observation, float sigma)
  {
    return abs(prediction - observation) <= sigma;
  }

  bool Tracker_impl::accepts(const Eigen::Matrix<float, 3, 1>& obs, int min, int max,
                             const Descriptor& descriptor) const
  {
    if (static_cast<int>(n_values.size()) > descriptor.min_nb_values_sigma && obs(1, 0) / X_predicted(1, 0) > 1.5 &&
        std::abs(obs(1, 0) - X_predicted(1, 0)) > 3)
    {
      return false;
    }

    if (descriptor.extraction_type == e_segdet_process_extraction::BINARY &&
        (n_max < min || max < n_min)) // TODO: Check cette condition | Garder pour les zones connexes, mais peut
                                      // être trop contraignante
      return false;

    return accepts_sigma(X_predicted(0, 0), obs(0, 0), sigma_position) &&
           accepts_sigma(X_predicted(1, 0), obs(1, 0), sigma_thickness) &&
           accepts_sigma(X_predicted(2, 0), obs(2, 0), sigma_luminosity);
  }

  std::optional<Observation> Tracker_impl::choose_nearest(Observation& obs)
  {
    auto X             = obs.obs;
    auto obs_to_return = std::make_optional(obs);

    int distance = std::abs(X(0, 0) - X_predicted(0, 0));

    if (observation == std::nullopt || distance < observation_distance)
    {
      obs_to_return        = observation;
      observation          = obs;
      observation_distance = distance;
    }
    return obs_to_return;
  }

  /**
   * Compute the slope of the segment formed by the tracker using Linear regression
   * @return The computed slope
   */
  float Tracker_impl::compute_slope()
  {
    reg.push(t_values.back(), n_values.back());
    auto slope = reg.predict();
    return slope;
  }

  void Tracker_impl::predict()
  {
    int thik_d2 = X_predicted(1, 0) / 2;
    n_min       = X_predicted(0, 0) - thik_d2;
    n_max       = X_predicted(0, 0) + thik_d2;

    observation = std::nullopt;
  }

  /**
   * Integrate the observation values into the vectors, and make sure the length of those vector do not exceed the
   * maximum number of values to keep
   * @param observation The observation matrix to integrate
   * @param t The position at which the integration was made
   */
  void Tracker_impl::integrate(int t, const Descriptor& descriptor)
  {
    const auto& observation_non_opt = observation.value();
    const auto& obs                 = observation_non_opt.obs;

    if (observation_non_opt.match_count != 1)
      same_observation.push_back(observation_non_opt.id * observation_non_opt.t);
    else
      same_observation.clear();

    last_integration = t;

    n_values.push_back(obs(0, 0));
    thicknesses.push_back(obs(1, 0));
    luminosities.push_back(obs(2, 0));

    t_values.push_back(t);

    current_slope = compute_slope();

    if (static_cast<int>(n_values.size()) > descriptor.nb_values_to_keep)
    {
      auto thick = thicknesses[0];
      auto nn    = n_values[0];
      auto tt    = t_values[0];

      n_values.erase(n_values.begin());
      t_values.erase(t_values.begin());
      thicknesses.erase(thicknesses.begin());
      luminosities.erase(luminosities.begin());

      Span span{};
      span.x         = tt;
      span.y         = nn;
      span.thickness = thick;
      segment_spans.push_back(span);
    }

    compute_sigmas(descriptor);
  }
} // namespace scribo::internal