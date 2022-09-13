#pragma once

#include "../filter_impl.hpp"
#include "algorithm"

namespace scribo::internal
{
  struct SimpleMovingAverage final : public Filter_impl
  {
    struct SimpleMovingAverageInside
    {
      const size_t       memory;
      std::vector<float> buffer;
      size_t             current_index;
      float              current_sum;
      size_t             dividor;

      SimpleMovingAverageInside(float value, size_t memory)
        : memory(memory)
        , current_index(1)
        , current_sum(value)
        , dividor(1)
      {
        buffer = std::vector<float>({value});
      }

      float predict() { return current_sum / static_cast<float>(dividor); }

      void integrate(float x)
      {
        current_sum             = current_sum - buffer[current_index] + x;
        buffer[current_index++] = x;
        dividor                 = std::max(dividor, current_index);
        current_index %= memory;
      }
    };

    float                     x;
    SimpleMovingAverageInside x_move;
    SimpleMovingAverageInside thick;
    SimpleMovingAverageInside lumi;

    SimpleMovingAverage(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~SimpleMovingAverage() = default;

    void predict();
    void integrate(int t, const Descriptor& descriptor);
  };
} // namespace scribo::internal