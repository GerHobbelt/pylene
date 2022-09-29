#pragma once

#include "../filter_impl.hpp"

namespace scribo::internal
{
  struct KalmanLeplumey final : public Filter_impl
  {
    Eigen::Matrix<float, 4, 4> H; // S prediction error variance matrix
    Eigen::Matrix<float, 4, 1> W; // Correction matrix

    KalmanLeplumey(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~KalmanLeplumey() = default;

    void predict();
    void integrate(int t, const Descriptor& descriptor);
  };

} // namespace scribo::internal