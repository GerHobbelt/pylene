#pragma once

#include "../tracker_impl.hpp"

namespace scribo::internal
{
  struct Kalman final : public Tracker_impl
  {
    Eigen::Matrix<float, 4, 1> S; // state matrix {{position (n)}, {slope}, {thickness}, {luminosity}}
    Eigen::Matrix<float, 4, 4> H; // S prediction error variance matrix

    Kalman(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~Kalman() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };

} // namespace scribo::internal