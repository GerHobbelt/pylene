#pragma once

#include "../filter_impl.hpp"
#include "algorithm"

namespace scribo::internal
{
  struct DoubleExponential final : public Filter_impl
  {
    struct DoubleExponentialInt
    {
      const float alpha;
      float       hatxiprev;
      float       hatxi2prev;

      DoubleExponentialInt(float value, float alpha)
        : alpha(alpha)
        , hatxiprev(value)
        , hatxi2prev(value)
      {
      }

      float predict() { return (2 + alpha / (1 - alpha)) * hatxiprev - (1 + alpha / (1 - alpha)) * hatxi2prev; }

      void integrate(int x)
      {
          hatxiprev = alpha * x + (1.0 - alpha) * hatxiprev;
          hatxi2prev = alpha * hatxiprev + (1.0 - alpha) * hatxi2prev;
      }
    };

    DoubleExponentialInt x;
    DoubleExponentialInt thick;
    DoubleExponentialInt lumi;

    DoubleExponential(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~DoubleExponential() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };
} // namespace scribo::internal