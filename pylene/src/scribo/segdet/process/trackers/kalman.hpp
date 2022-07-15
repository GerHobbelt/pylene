#pragma once

#include "../filter_impl.hpp"

namespace scribo::internal
{
  struct Kalman final : public Filter_impl
  {
    Eigen::Matrix<float, 4, 4> H; // S prediction error variance matrix

    Kalman(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~Kalman() = default;

    void predict();
    void integrate(int t, const Descriptor& descriptor);
  };

} // namespace scribo::internal