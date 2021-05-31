#pragma once
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

namespace mln::contrib::segdet
{
  template <typename T>
  double mean(std::vector<T> array, size_t start, size_t end)
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
    MemoryLinearRegressor(float x0, float y0, int memory = 20)
      : count(0)
      , memory(memory)
    {
      Y   = std::vector<float>({y0});
      X   = std::vector<float>({x0});
      Sy  = y0;
      Sx  = x0;
      Sxx = x0 * x0;
      Sxy = y0 * x0;
      n   = 1;
    }

    float predict()
    {
      count = std::min(memory, n);

      float mean_x = Sx / (float)count;
      float mean_y = Sy / (float)count;
      float var_x  = (float)count * Sxx - Sx * Sx;
      float cov_xy = (float)count * Sxy - Sx * Sy;
      float a      = var_x > 0 ? cov_xy / var_x : 0;
      auto  b      = mean_y - a * mean_x;
      (void)b;
      return a;
    }

    void push(size_t x, size_t y)
    {
      count    = std::min(memory, n);
      auto f_x = (float)x;
      auto f_y = (float)y;
      if (n < memory)
      {
        Y.push_back(f_y);
        X.push_back(f_x);
        Sy  = Sy + f_y;
        Sx  = Sx + f_x;
        Sxx = Sxx + f_x * f_x;
        Sxy = Sxy + f_x * f_y;
      }
      else
      {
        int   i     = n % memory;
        float old_y = Y[i];
        float old_x = X[i];
        Sy          = Sy - old_y + f_y;
        Sx          = Sx - old_x + f_x;
        Sxy         = Sxy - old_x * old_y + f_y * f_x;
        Sxx         = Sxx - old_x * old_x + f_x * f_x;
        Y[i]        = f_y;
        X[i]        = f_x;
      }
      n += 1;
    }
  };

} // namespace mln::contrib::segdet
