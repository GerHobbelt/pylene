#pragma once

#include "../tracker_impl.hpp"

namespace scribo::internal
{
  struct ExponentialMovingAverage final : public Tracker_impl
  {
    struct ExponentialMovingAverageInternal
    {
      float       current;
      float       nb_value;
      const float nb_value_max;

      ExponentialMovingAverageInternal(float value, float nb_value_max)
        : current(value)
        , nb_value(1.f)
        , nb_value_max(nb_value_max)
      {
      }

      float predict() { return current; }

      void integrate(float x)
      {
        float w  = 2.f / (nb_value + 1);
        nb_value = std::min(nb_value + 1, nb_value_max);
        current  = current * w + x * (1.f - w);
      }
    };

    float                            x;
    ExponentialMovingAverageInternal x_move;
    ExponentialMovingAverageInternal thick;
    ExponentialMovingAverageInternal lumi;

    ExponentialMovingAverage(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~ExponentialMovingAverage() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };
} // namespace scribo::internal