#include "double_exponential.hpp"

namespace scribo::internal
{
  DoubleExponential::DoubleExponential(int t_integration, Eigen::Matrix<float, 3, 1> observation,
                                       const Descriptor& descriptor)
    : Tracker_impl(t_integration, observation, descriptor)
    , x(DoubleExponentialInternal(observation(0, 0), descriptor.double_exponential_alpha))
    , thick(DoubleExponentialInternal(observation(1, 0), descriptor.double_exponential_alpha))
    , lumi(DoubleExponentialInternal(observation(2, 0), descriptor.double_exponential_alpha))
  {
  }

  void DoubleExponential::predict()
  {
    X_predicted(0, 0) = x.predict();
    X_predicted(1, 0) = thick.predict();
    X_predicted(2, 0) = lumi.predict();

    Tracker_impl::predict();
  }

  void DoubleExponential::integrate(int t, const Descriptor& descriptor)
  {
    const auto& obs = observation.value().obs;

    x.integrate(obs(0, 0));
    thick.integrate(obs(1, 0));
    lumi.integrate(obs(2, 0));

    Tracker_impl::integrate(t, descriptor);
  }
} // namespace scribo::internal
