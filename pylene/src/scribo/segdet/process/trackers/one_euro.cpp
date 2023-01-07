#include "one_euro.hpp"

namespace scribo::internal
{
  OneEuros::OneEuros(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor)
    : Filter_impl(t_integration, observation, descriptor)
    , x(OneEuro(observation(0, 0), t_integration))
    , thick(OneEuro(observation(1, 0), t_integration))
    , lumi(OneEuro(observation(2, 0), t_integration))
  {
  }

  void OneEuros::predict()
  {
    X_predicted(0, 0) = x.predict();
    X_predicted(1, 0) = thick.predict();
    X_predicted(2, 0) = lumi.predict();

    Filter_impl::predict();
  }

  void OneEuros::integrate(int t, const Descriptor& descriptor)
  {
    const auto& obs = observation.value().obs;

    x.integrate(obs(0, 0), t);
    thick.integrate(obs(1, 0), t);
    lumi.integrate(obs(2, 0), t);

    Filter_impl::integrate(t, descriptor);
  }
} // namespace scribo::internal
