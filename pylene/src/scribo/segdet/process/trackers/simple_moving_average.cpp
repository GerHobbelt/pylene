#include "simple_moving_average.hpp"

namespace scribo::internal
{
  SimpleMovingAverage::SimpleMovingAverage(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor)
    : Filter_impl(t_integration, observation, descriptor)
    , x(observation(0, 0))
    , x_move(SimpleMovingAverageInside(0, descriptor.simple_moving_average_memory))
    , thick(SimpleMovingAverageInside(observation(1, 0), descriptor.simple_moving_average_memory))
    , lumi(SimpleMovingAverageInside(observation(2, 0), descriptor.simple_moving_average_memory))
  {
  }

  void SimpleMovingAverage::predict()
  {
    X_predicted(0, 0) = x + x_move.predict();
    X_predicted(1, 0) = thick.predict();
    X_predicted(2, 0) = lumi.predict();

    Filter_impl::predict();
  }

  void SimpleMovingAverage::integrate(int t, const Descriptor& descriptor)
  {
    const auto& obs = observation.value().obs;

    x = obs(0, 0);
    thick.integrate(obs(1, 0));
    lumi.integrate(obs(2, 0));

    std::cout << "a:" << current_slope << std::endl;
    Filter_impl::integrate(t, descriptor);
    std::cout << "b:" << current_slope << std::endl;
    // x_move.integrate(current_slope);
    std::cout << "c:" << current_slope << std::endl;

  }
} // namespace scribo::internal
