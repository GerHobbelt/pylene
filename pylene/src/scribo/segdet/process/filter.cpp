#include "filter.hpp"

// Include of trackers/*.hpp
#include "trackers/kalman.hpp"
#include "trackers/kalman_leplumey.hpp"

namespace scribo::internal
{
  Filter::Filter(int t, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor)
  {
    switch (descriptor.filter_type)
    {
    case SEGDET_PROCESS_TRACKING_ENUM::KALMAN:
      impl = std::make_unique<Kalman>(t, observation, descriptor);
      break;
    case SEGDET_PROCESS_TRACKING_ENUM::KALMAN_LEPLUMEY:
      impl = std::make_unique<KalmanLeplumey>(t, observation, descriptor);
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

} // namespace scribo::internal