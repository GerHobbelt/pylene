#pragma once


#include <cmath>

namespace mln::morpho::details
{

  struct recursivefilter_coef_
  {
    enum FilterType
    {
      DericheGaussian,
      DericheGaussianFirstDerivative,
      DericheGaussianSecondDerivative
    };

    recursivefilter_coef_(float a0, float a1, float b0, float b1, float c0, float c1, float w0, float w1, float s,
                          FilterType filter_type);

    float n[4];
    float d[4];
    float nm[4];
    float dm[4];
    float sumA, sumC;
  };

} // namespace mln::morpho::experimental::details
