#pragma once

#include "../filter_impl.hpp"

namespace scribo::internal
{
  struct KalmanLeplumey final : public Filter_impl
  {
    Eigen::Matrix<float, 4, 1> S; // state matrix {{position (n)}, {slope}, {thickness}, {luminosity}}
    Eigen::Matrix<float, 4, 4> H; // S prediction error variance matrix
    Eigen::Matrix<float, 4, 1> W; // Correction matrix
    float save_last_slope;

    KalmanLeplumey(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~KalmanLeplumey() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };

} // namespace scribo::internal