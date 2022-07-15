#include "kalman.hpp"

namespace scribo::internal
{
  namespace
  {
    static constexpr float SEGDET_VARIANCE_POSITION   = 1.f;
    static constexpr float SEGDET_VARIANCE_THICKNESS  = 1.f;
    static constexpr float SEGDET_VARIANCE_LUMINOSITY = 4.f;

    // Defining useful matrices
    static const Eigen::Matrix<float, 4, 4>
        A((Eigen::Matrix<float, 4, 4>() << 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
    static const Eigen::Matrix<float, 4, 4>
        A_transpose((Eigen::Matrix<float, 4, 4>() << 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
    static const Eigen::Matrix<float, 3, 4>
        C((Eigen::Matrix<float, 3, 4>() << 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1).finished());
    static const Eigen::Matrix<float, 4, 3>
        C_transpose((Eigen::Matrix<float, 4, 3>() << 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1).finished());
    static const Eigen::Matrix<float, 3, 3> Vn((Eigen::Matrix<float, 3, 3>() << SEGDET_VARIANCE_POSITION, 0, 0, 0,
                                                SEGDET_VARIANCE_THICKNESS, 0, 0, 0, SEGDET_VARIANCE_LUMINOSITY)
                                                   .finished());
    static const Eigen::Matrix<float, 4, 4> Q(Eigen::Matrix<float, 4, 4>::Identity() * 0.00001);

  } // namespace kalman

  Kalman::Kalman(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor)
    : Filter_impl(t_integration, observation, descriptor)
    , H(Eigen::Matrix<float, 4, 4>::Identity())
  {
  }

  void Kalman::predict()
  {
    S_predicted = A * S;
    X_predicted = C * S_predicted;

    H = A * H * A_transpose + Q;

    Filter_impl::predict();
  }

  void Kalman::integrate(int t, const Descriptor& descriptor)
  {
    const auto& obs = observation.value().obs;

    const auto H_Ct = H * C_transpose;
    const auto G    = H_Ct * (C * H_Ct + Vn).inverse();
    S               = S_predicted + G * (obs - X_predicted);
    H               = (Eigen::Matrix<float, 4, 4>::Identity() - G * C) * H;

    Filter_impl::integrate(t, descriptor);
  }
} // namespace scribo::internal