#include "exponential_moving_average.hpp"

namespace scribo::internal
{
  ExponentialMovingAverage::ExponentialMovingAverage(int t_integration, Eigen::Matrix<float, 3, 1> observation,
                                                     const Descriptor& descriptor)
    : Tracker_impl(t_integration, observation, descriptor)
    , x(observation(0, 0))
    , x_move(ExponentialMovingAverageInternal(0, descriptor.exponential_moving_average_memory))
    , thick(ExponentialMovingAverageInternal(observation(1, 0), descriptor.exponential_moving_average_memory))
    , lumi(ExponentialMovingAverageInternal(observation(2, 0), descriptor.exponential_moving_average_memory))
  {
  }

  void ExponentialMovingAverage::predict()
  {
    X_predicted(0, 0) = x + x_move.predict();
    X_predicted(1, 0) = thick.predict();
    X_predicted(2, 0) = lumi.predict();

    Tracker_impl::predict();
  }

  void ExponentialMovingAverage::integrate(int t, const Descriptor& descriptor)
  {
    const auto& obs = observation.value().obs;

    x = obs(0, 0);
    thick.integrate(obs(1, 0));
    lumi.integrate(obs(2, 0));

    Tracker_impl::integrate(t, descriptor);

    x_move.integrate(current_slope);
  }
} // namespace scribo::internal
