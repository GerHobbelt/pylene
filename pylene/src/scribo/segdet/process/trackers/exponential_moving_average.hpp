#pragma once

#include "../filter_impl.hpp"

namespace scribo::internal
{
  struct ExponentialMovingAverage final : public Filter_impl
  {
    struct ExponentialMovingAverageInside
    {
      float current;
      float nb_value;

      ExponentialMovingAverageInside(float value)
        : current(value)
        , nb_value(1.f)
      {
      }

      float predict() { return current; }

      void integrate(float x)
      {
        float w = 2.f / (nb_value++ + 1);
        current = current * w + x * (1.f - w);
      }
    };

    float                          x;
    ExponentialMovingAverageInside x_move;
    ExponentialMovingAverageInside thick;
    ExponentialMovingAverageInside lumi;

    ExponentialMovingAverage(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~ExponentialMovingAverage() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };
} // namespace scribo::internal