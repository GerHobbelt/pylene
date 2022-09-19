
#include "linearregression.hpp"

namespace scribo::internal
{
  MemoryLinearRegressor::MemoryLinearRegressor(float x0, float y0, int memory)
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

  float MemoryLinearRegressor::predict()
  {
    count = std::min(memory, n);

    float var_x  = (float)count * Sxx - Sx * Sx;
    float cov_xy = (float)count * Sxy - Sx * Sy;
    float a      = var_x > 0 ? cov_xy / var_x : 0;
    return a;
  }

  void MemoryLinearRegressor::push(float x, float y)
  {
    count    = std::min(memory, n);
    if (n < memory)
    {
      Y.push_back(y);
      X.push_back(x);
      Sy  = Sy + y;
      Sx  = Sx + x;
      Sxx = Sxx + x * x;
      Sxy = Sxy + x * y;
    }
    else
    {
      int   i     = n % memory;
      float old_y = Y[i];
      float old_x = X[i];
      Sy          = Sy - old_y + y;
      Sx          = Sx - old_x + x;
      Sxy         = Sxy - old_x * old_y + y * x;
      Sxx         = Sxx - old_x * old_x + x * x;
      Y[i]        = y;
      X[i]        = x;
    }
    
    n += 1;
  }
} // namespace mln::contrib::segdet