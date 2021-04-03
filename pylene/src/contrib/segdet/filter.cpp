//
// Created by alexandre on 02/04/2021.
//

#include "mln/contrib/segdet/filter.hpp"
#include "mln/accu/accumulators/variance.hpp"
#include <algorithm>
#include <numeric>

#define SEGDET_MIN_NB_VALUES_SIGMA 10

#define SEGDET_SIGMA_POS_MIN 1
#define SEGDET_SIGMA_THK_MIN 1.5
#define SEGDET_SIGMA_LUM_MIN 13

#define SEGDET_DEFAULT_SIGMA_POS 2
#define SEGDET_DEFAULT_SIGMA_THK 2
#define SEGDET_DEFAULT_SIGMA_LUM 57


namespace mln::contrib::segdet
{

  template <typename T>
  double std(const std::vector<T>& vec)
  {
    const size_t sz = vec.size();
    if (sz == 1)
    {
      return 0.0;
    }

    // Calculate the mean
    const double mean = std::accumulate(vec.begin(), vec.end(), 0.0) / sz;

    // Now calculate the variance
    auto variance_func = [&mean, &sz](double accumulator, const double& val) {
      return accumulator + ((val - mean) * (val - mean) / ((double)sz - 1));
    };

    return std::accumulate(vec.begin(), vec.end(), 0.0, variance_func);
  }

  void compute_sigmas(Filter& f)
  {
    if (f.n_values.size() > SEGDET_MIN_NB_VALUES_SIGMA)
    {
      f.sigma_position   = std(f.n_values) + SEGDET_SIGMA_POS_MIN + f.currently_under_other.size() * 0.2;
      f.sigma_thickness  = std(f.thicknesses) + SEGDET_SIGMA_THK_MIN;
      f.sigma_luminosity = std(f.luminosities) + SEGDET_SIGMA_LUM_MIN;
    }
    else
    {
      f.sigma_position   = SEGDET_DEFAULT_SIGMA_POS;
      f.sigma_thickness  = SEGDET_DEFAULT_SIGMA_THK;
      f.sigma_luminosity = SEGDET_DEFAULT_SIGMA_LUM;
    }
  }

  void predict(Filter& f)
  {
    f.S_predicted = A * f.S + f.W;
    f.X_predicted = C * f.S_predicted + f.N;

    f.n_min = f.X_predicted(0, 0) - f.X_predicted(1, 0) / 2;
    f.n_max = f.X_predicted(0, 0) + f.X_predicted(1, 0) / 2;

    f.H = A * f.H * A_transpose;

    f.W = Eigen::Matrix<double, 4, 1>(0, 0, 0, 0);

    f.observation = std::nullopt;

    compute_sigmas(f);
  }

  bool accepts_sigma(uint32_t prediction, uint32_t observation, double sigma)
  {
    if (prediction > observation)
      return (prediction - observation) <= 3 * sigma;
    return (observation - prediction) <= 3 * sigma;
  }

  bool accepts(const Filter& f, const Eigen::Matrix<double, 3, 1>& obs, uint32_t min, uint32_t max)
  {
    if (f.n_values.size() > SEGDET_MIN_NB_VALUES_SIGMA && obs(1, 0) / f.X_predicted(1, 0) > 1.5 &&
        std::abs(obs(1, 0) - f.X_predicted(1, 0)) > 3)
    {
      return false;
    }

    if (min > f.n_max || max < f.n_min)
      return false;

    return accepts_sigma(f.X_predicted(0, 0), obs(0, 0), f.sigma_position) &&
           accepts_sigma(f.X_predicted(1, 0), obs(1, 0), f.sigma_thickness) &&
           accepts_sigma(f.X_predicted(2, 0), obs(2, 0), f.sigma_luminosity);
  }

  Observation choose_nearest(Filter& f, Observation& obs, const uint32_t& t)
  {
    auto X = obs.obs;
    auto obs_to_return = obs;

    uint32_t distance = std::abs(X(0, 0) - f.X_predicted(0, 0));

    if (f.observation == std::nullopt || distance < f.observation_distance)
    {
      obs_to_return = f.observation.value();
      f.observation = obs;
      f.observation_position = t;
      f.observation_distance = distance;
    }
    return obs_to_return;
  }

} // namespace mln::contrib::segdet