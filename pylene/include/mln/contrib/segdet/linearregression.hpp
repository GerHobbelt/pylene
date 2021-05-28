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

  template <typename X, typename Z>
  class Linear_Regression
  {

  public:
    template <typename T>
    std::vector<T> vec_multiply(std::vector<T> array, std::vector<T> array2)
    {
      std::vector<T> out(array2.size());
      for (size_t i = 0; i < array.size(); i++)
      {
        out[i] = array[i] * array2[i];
      }
      return out;
    }


    template <typename T, typename M>
    void estimate_coef(std::vector<T> indep_var, std::vector<T> dep_var, M& B_1, M& B_0)
    {
      M   X_mean = mean(indep_var, 0, 0);
      M   Y_mean = mean(dep_var, 0, 0);
      M   SS_xy  = 0;
      M   SS_xx  = 0;
      int n      = indep_var.size();
      {
        std::vector<T> temp;
        temp  = vec_multiply(indep_var, dep_var);
        SS_xy = std::accumulate(temp.begin(), temp.end(), 0);
        SS_xy = SS_xy - n * X_mean * Y_mean;
      }
      {
        std::vector<T> temp;
        temp  = vec_multiply(indep_var, indep_var);
        SS_xx = std::accumulate(temp.begin(), temp.end(), 0);
        SS_xx = SS_xx - n * X_mean * X_mean;
      }

      B_1 = SS_xy / SS_xx;
      B_0 = Y_mean - B_1 * X_mean;
    }

    void fit(std::vector<std::vector<Z>> dataset) { estimate_coef(dataset[0], dataset[1], b_1, b_0); }

    Z predict(const Z& test_data) { return b_0 + (b_1 * test_data); }
    X b_1;
    X b_0;
  };


  class MemoryLinearRegressor
  {
  public:

    float predict()
    {
      count = std::min(memory, n);
      float mean_x = (float) -(count + 1) / (float) 2;
      float mean_y = Sy / (float) count;
      float var_x = (float) (count * count - 1) / 12;
      float cov_xy = Sxy / (float) count - mean_x * mean_y;
      float a = var_x > 0  ? cov_xy / var_x : 0;
      return mean_y - a * mean_x;
    }

    void push(float y)
    {
      count = std::min(memory, n);
      if (n < memory)
      {
        Y.push_back(y);
        Sy  = Sy + (float) y;
        Sxy = Sxy - Sy;
      }
      else
      {
        int i   = n % memory;
        float old = Y[i];
        Sy  = Sy - (float) old + (float) y;
        Sxy = Sxy + (float) old * count - Sy;
        Y[i] = y;
      }
      n += 1;
    }


    MemoryLinearRegressor(float y0, int memory = 20)
        : memory(memory)
    {
      Y = std::vector<float>({y0});
      Sy  = y0;
      Sxy = -y0;
      n   = 1;
    }

  private:
    std::vector<float> Y;
    float Sy, Sxy;
    int count;
    int n;
    int memory;
  };

} // namespace mln::contrib::segdet
