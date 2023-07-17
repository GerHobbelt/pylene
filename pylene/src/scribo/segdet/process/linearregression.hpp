#pragma once

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

namespace scribo::internal
{
  template <typename T>
  float mean(std::vector<T> array, int start, int end)
  {
    auto it_end = end == 0 ? array.end() : array.begin() + end;
    return std::accumulate(array.begin() + start, it_end, 0.0) / static_cast<double>(array.size());
  }

  class MemoryLinearRegressor
  {
  private:
    std::vector<float> Y;
    std::vector<float> X;
    float              Sy, Sxy, Sx, Sxx;
    int                count;
    int                n;
    int                memory;

  public:
    MemoryLinearRegressor(float x0, float y0, int memory = 20);
    float predict();
    void  push(float x, float y);
  };

} // namespace scribo::internal
