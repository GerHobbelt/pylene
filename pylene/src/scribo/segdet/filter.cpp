
#include "filter.hpp"

#include <scribo/private/segment.hpp>

#include <algorithm>
#include <numeric>

namespace scribo::internal
{
  static constexpr float SEGDET_VARIANCE_POSITION   = 2.f;
  static constexpr float SEGDET_VARIANCE_THICKNESS  = 1.f;
  static constexpr float SEGDET_VARIANCE_LUMINOSITY = 12.f;

  // Defining useful matrices

  static const Eigen::Matrix<float, 4, 4>
      A((Eigen::Matrix<float, 4, 4>() << 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
  static const Eigen::Matrix<float, 4, 4>
      A_transpose((Eigen::Matrix<float, 4, 4>() << 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
  static const Eigen::Matrix<float, 3, 4>
      C((Eigen::Matrix<float, 3, 4>() << 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
  static const Eigen::Matrix<float, 4, 3>
      C_transpose((Eigen::Matrix<float, 4, 3>() << 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1).finished());
  static const Eigen::Matrix<float, 3, 3> Vn((Eigen::Matrix<float, 3, 3>() << SEGDET_VARIANCE_POSITION, 0, 0, 0,
                                              SEGDET_VARIANCE_THICKNESS, 0, 0, 0, SEGDET_VARIANCE_LUMINOSITY)
                                                 .finished());

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

  Filter::Filter(int t_integration, Eigen::Matrix<float, 3, 1> observation, const scribo::SegDetParams& params)
    : S((Eigen::Matrix<float, 4, 1>() << observation(0, 0), 0, observation(1, 0), observation(2, 0)).finished())
    , W(Eigen::Matrix<float, 4, 1>::Zero())
    , H(Eigen::Matrix<float, 4, 4>::Identity())
    , observation(std::nullopt)
    , observation_distance(0)
    , last_integration(t_integration)
    , reg(t_integration, observation(0, 0))
  {
    first    = t_integration;
    t_values = std::vector<int>({t_integration});
    n_values = std::vector<int>({static_cast<int>(observation(0, 0))});

    thicknesses  = std::vector<float>({observation(1, 0)});
    luminosities = std::vector<float>({observation(2, 0)});

    last_slope    = 0;
    current_slope = 0;

    under_other           = std::vector<Span>();
    currently_under_other = 0;

    n_min = 0;
    n_max = 0;

    sigma_position   = params.default_sigma_position;
    sigma_thickness  = params.default_sigma_thickness;
    sigma_luminosity = params.default_sigma_luminosity;

    S_predicted = Eigen::Matrix<float, 4, 1>::Zero();
    X_predicted = Eigen::Matrix<float, 3, 1>::Zero();

    segment_spans = std::vector<Span>();
  }

  void compute_sigmas(Filter& f, const SegDetParams& params)
  {
    if (static_cast<int>(f.n_values.size()) > params.min_nb_values_sigma)
    {
      f.sigma_position   = std(f.n_values) + params.sigma_pos_min + f.currently_under_other * 0.2;
      f.sigma_thickness  = std(f.thicknesses) * 2 + params.sigma_thickness_min;
      f.sigma_luminosity = std(f.luminosities) + params.sigma_luminosity_min;
    }
  }

  void predict(Filter& f)
  {
    f.S_predicted = A * f.S + f.W;
    f.X_predicted = C * f.S_predicted;

    int thik_d2 = f.X_predicted(1, 0) / 2;
    f.n_min     = f.X_predicted(0, 0) - thik_d2;
    f.n_max     = f.X_predicted(0, 0) + thik_d2;

    f.H = A * f.H * A_transpose;

    f.W(0, 0) = 0;
    f.W(1, 0) = 0;

    f.observation = std::nullopt;
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
    return abs(prediction - observation) <= 3 * sigma;
  }

  bool accepts(const Filter& f, const Eigen::Matrix<float, 3, 1>& obs, int min, int max, const SegDetParams& params)
  {
    if (static_cast<int>(f.n_values.size()) > params.min_nb_values_sigma && obs(1, 0) / f.X_predicted(1, 0) > 1.5 &&
        std::abs(obs(1, 0) - f.X_predicted(1, 0)) > 3)
    {
      return false;
    }

    if (f.n_max < min || max < f.n_min)
      return false;

    return accepts_sigma(f.X_predicted(0, 0), obs(0, 0), f.sigma_position) &&
           accepts_sigma(f.X_predicted(1, 0), obs(1, 0), f.sigma_thickness) &&
           accepts_sigma(f.X_predicted(2, 0), obs(2, 0), f.sigma_luminosity);
  }

  std::optional<Observation> choose_nearest(Filter& f, Observation& obs)
  {
    auto X             = obs.obs;
    auto obs_to_return = std::make_optional(obs);

    int distance = std::abs(X(0, 0) - f.X_predicted(0, 0));

    if (f.observation == std::nullopt || distance < f.observation_distance)
    {
      obs_to_return          = f.observation;
      f.observation          = obs;
      f.observation_distance = distance;
    }
    return obs_to_return;
  }

  /**
   * Compute the slope of the segment formed by the filter using Linear regression
   * @param f The filter for which to compute the slope
   * @return The computed slope
   */
  float compute_slope(Filter& f)
  {
    f.reg.push(f.t_values.back(), f.n_values.back());
    auto slope = f.reg.predict();
    return slope;
  }

  /**
   * Integrate the observation values into the vectors, and make sure the length of those vector do not exceed the
   * maximum number of values to keep
   * @param f The filter for which to integrate
   * @param observation The observation matrix to integrate
   * @param t The position at which the integration was made
   */
  void insert_into_filters_list(Filter& f, Eigen::Matrix<float, 3, 1> observation, int t, const SegDetParams& params)
  {
    f.n_values.push_back(observation(0, 0));
    f.thicknesses.push_back(observation(1, 0));
    f.luminosities.push_back(observation(2, 0));

    f.t_values.push_back(t);

    f.last_slope    = f.current_slope;
    f.current_slope = compute_slope(f);

    if (static_cast<int>(f.n_values.size()) > params.nb_values_to_keep)
    {
      auto thick = f.thicknesses[0];
      auto nn    = f.n_values[0];
      auto tt    = f.t_values[0];

      f.n_values.erase(f.n_values.begin());
      f.t_values.erase(f.t_values.begin());
      f.thicknesses.erase(f.thicknesses.begin());
      f.luminosities.erase(f.luminosities.begin());

      Span span{};
      span.x         = tt;
      span.y         = nn;
      span.thickness = thick;
      span.angle     = std::atan(f.current_slope) * 180 / 3.14;
      f.segment_spans.push_back(span);
    }
  }

  void integrate(Filter& f, int t, const SegDetParams& params)
  {
    auto& observation = f.observation.value().obs;

    f.currently_under_other = 0;

    auto G = f.H * C_transpose * (C * f.H * C_transpose + Vn).inverse();
    f.S    = f.S_predicted + G * (observation - f.X_predicted);
    f.H    = (Eigen::Matrix<float, 4, 4>::Identity() - G * C) * f.H;

    insert_into_filters_list(f, observation, t, params);

    float second_derivative = (f.last_slope - f.current_slope) / (f.last_integration - t);
    f.W(0, 0)               = 0.5 * second_derivative;
    f.W(1, 0)               = second_derivative;
    f.last_integration      = t;
  }
} // namespace scribo::internal