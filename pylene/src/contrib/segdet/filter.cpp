//
// Created by alexandre on 02/04/2021.
//

#include "mln/contrib/segdet/filter.hpp"
#include "mln/accu/accumulators/variance.hpp"
#include "mln/contrib/segdet/linearregression.hpp"
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

  std::optional<Observation> choose_nearest(Filter& f, Observation& obs)
  {
    auto X             = obs.obs;
    auto obs_to_return = std::make_optional(obs);

    uint32_t distance = std::abs(X(0, 0) - f.X_predicted(0, 0));

    if (f.observation == std::nullopt || distance < f.observation_distance)
    {
      if (f.observation != std::nullopt)
        obs_to_return = f.observation.value();
      else
        obs_to_return = std::nullopt;
      f.observation          = obs;
      f.observation_distance = distance;
    }
    return obs_to_return;
  }

  double compute_slope(Filter& f)
  {
    auto X = f.t_values;
    auto Y = f.n_values;

    std::vector<std::vector<uint32_t>> Z(X.size());
    Z[0] = X;
    Z[1] = Y;

    Linear_Regression <double, uint32_t> reg{};
    reg.fit(Z);

    auto slope = reg.b_1;

    if (std::abs(slope) > f.slope_max)
      f.nb_current_slopes_over_slope_max++;
    else
      f.nb_current_slopes_over_slope_max = 0;

    return slope;
  }

  void insert_into_filters_list(Filter& f, Eigen::Matrix<double, 3, 1> observation, uint32_t t)
  {
    f.n_values.push_back(observation(0, 0));
    f.thicknesses.push_back(observation(1, 0));
    f.luminosities.push_back(observation(2, 0));
    f.t_values.push_back(t);
    f.slopes.push_back(compute_slope(f));

    if (f.n_values.size() > SEGDET_NB_VALUES_TO_KEEP)
    {
      auto thick = f.thicknesses[0];
      auto nn    = f.n_values[0];
      auto tt    = f.t_values[0];

      f.thicknesses.erase(f.thicknesses.begin());
      f.t_values.erase(f.t_values.begin());
      f.n_values.erase(f.n_values.begin());
      f.luminosities.erase(f.luminosities.begin());
      f.slopes.erase(f.slopes.begin());

      if (f.first_slope == std::nullopt)
        f.first_slope = std::make_optional(f.slopes[f.slopes.size() - 1]);

      f.segment_points.emplace_back(nn, tt, thick, f.is_horizontal);
    }
  }

  void integrate(Filter& f, uint32_t t)
  {
    auto& observation = f.observation.value().obs;
    if (!f.currently_under_other.empty())
    {
      for (auto& elm : f.currently_under_other)
        f.under_other.push_back(elm);
      f.currently_under_other.clear();
    }

    auto G = f.H * C_transpose * (C * f.H * C_transpose + Vn).inverse();
    f.S    = f.S_predicted + G * (observation - f.X_predicted);
    f.H    = (Eigen::Matrix<double, 4, 4>::Identity() - G * C) * f.H;

    insert_into_filters_list(f, observation, t);

    auto   length = f.slopes.size();
    double second_derivative =
        (f.slopes[length - 1] - f.slopes[length - 2]) / (f.t_values[length - 1] - f.t_values[length - 2]);
    f.W(0,0) = 0.5 * second_derivative;
    f.W(1, 0) = second_derivative;
    f.last_integration = t;
  }

} // namespace mln::contrib::segdet