#include "last_integration.hpp"

namespace scribo::internal
{
  LastIntegration::LastIntegration(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor)
    : Tracker_impl(t_integration, observation, descriptor)
  {
  }

  void LastIntegration::predict()
  {
    X_predicted(0, 0) = n_values.back();
    X_predicted(1, 0) = thicknesses.back();
    X_predicted(2, 0) = luminosities.back();

    Tracker_impl::predict();
  }

  void LastIntegration::integrate(int t, const Descriptor& descriptor)
  {
    Tracker_impl::integrate(t, descriptor);
  }
} // namespace scribo::internal
