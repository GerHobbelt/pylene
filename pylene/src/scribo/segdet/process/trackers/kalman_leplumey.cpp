#include "kalman_leplumey.hpp"

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
  } // namespace kalman_leplumey

  KalmanLeplumey::KalmanLeplumey(int t_integration, Eigen::Matrix<float, 3, 1> observation,
                                 const Descriptor& descriptor)
    : Filter_impl(t_integration, observation, descriptor)
    , H(Eigen::Matrix<float, 4, 4>::Identity())
    , W(Eigen::Matrix<float, 4, 1>::Zero())
  {
  }

  void KalmanLeplumey::predict()
  {
    S_predicted = A * S + W;
    X_predicted = C * S_predicted;

    H = A * H * A_transpose;

    W(0, 0) = 0;
    W(1, 0) = 0;

    Filter_impl::predict();
  }

  void KalmanLeplumey::integrate(int t, const Descriptor& descriptor)
  {
    const auto& obs = observation.value().obs;

    const auto H_Ct = H * C_transpose;
    const auto G    = H_Ct * (C * H_Ct + Vn).inverse();
    S               = S_predicted + G * (obs - X_predicted);
    H               = (Eigen::Matrix<float, 4, 4>::Identity() - G * C) * H;

    int save_last_intergration = last_integration;
    Filter_impl::integrate(t, descriptor);

    float second_derivative = (last_slope - current_slope) / (save_last_intergration - t);
    W(0, 0)                 = 0.5 * second_derivative;
    W(1, 0)                 = second_derivative;
  }
} // namespace scribo::internal