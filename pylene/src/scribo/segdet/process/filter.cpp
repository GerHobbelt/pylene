#include "filter.hpp"

// Include of trackers/*.hpp
#include "trackers/double_exponential.hpp"
#include "trackers/exponential_moving_average.hpp"
#include "trackers/kalman.hpp"
#include "trackers/kalman_IRISA.hpp"
#include "trackers/last_integration.hpp"
#include "trackers/one_euro.hpp"
#include "trackers/simple_moving_average.hpp"

namespace scribo::internal
{
  Filter::Filter(int t, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor)
  {
    switch (descriptor.filter_type)
    {
    case SEGDET_PROCESS_TRACKING_ENUM::KALMAN:
      impl = std::make_unique<Kalman>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::KALMAN_IRISA:
      impl = std::make_unique<KalmanIRISA>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::ONE_EURO:
      impl = std::make_unique<OneEuros>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::LAST_INTEGRATION:
      impl = std::make_unique<LastIntegration>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::DOUBLE_EXPONENTIAL:
      impl = std::make_unique<DoubleExponential>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::SIMPLE_MOVING_AVERAGE:
      impl = std::make_unique<SimpleMovingAverage>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::EXPONENTIAL_MOVING_AVERAGE:
      impl = std::make_unique<ExponentialMovingAverage>(t, observation, descriptor);
      break;
    }
  }

  void Filter::predict()
  {
    impl->predict();
  }

  void Filter::integrate(int t, const Descriptor& descriptor)
  {
    impl->integrate(t, descriptor);
  }

  int Filter::get_position() const noexcept
  {
    return static_cast<int>(impl->n_values.back());
  }
} // namespace scribo::internal