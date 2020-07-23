#include <mln/morpho/private/gaussian2d_coefs.hpp>

namespace mln::morpho::details
{

  recursivefilter_coef_::recursivefilter_coef_(float a0, float a1, float b0, float b1, float c0, float c1, float w0,
                                               float w1, float s, FilterType filter_type)
    : n{0.}
    , d{0.}
    , nm{0.}
    , dm{0.}
    , sumA{0.}
    , sumC{0.}
  {
    b0 /= s;
    b1 /= s;
    w0 /= s;
    w1 /= s;

    float sin0 = std::sin(w0);
    float sin1 = std::sin(w1);
    float cos0 = std::cos(w0);
    float cos1 = std::cos(w1);

    switch (filter_type)
    {

    case DericheGaussian:
      sumA = (2.0 * a1 * exp(b0) * cos0 * cos0 - a0 * sin0 * exp(2.0 * b0) + a0 * sin0 - 2.0 * a1 * exp(b0)) /
             ((2.0 * cos0 * exp(b0) - exp(2.0 * b0) - 1) * sin0);

      sumC = (2.0 * c1 * exp(b1) * cos1 * cos1 - c0 * sin1 * exp(2.0 * b1) + c0 * sin1 - 2.0 * c1 * exp(b1)) /
             ((2.0 * cos1 * exp(b1) - exp(2.0 * b1) - 1) * sin1);
      break;

    case DericheGaussianFirstDerivative:
      sumA = -2.f *
             (a0 * cos0 - a1 * sin0 + a1 * sin0 * exp(2.0 * b0) + a0 * cos0 * exp(2.0 * b0) - 2.0 * a0 * exp(b0)) *
             exp(b0) /
             (exp(4.0 * b0) - 4.0 * cos0 * exp(3.0 * b0) + 2.0 * exp(2.0 * b0) + 4.0 * cos0 * cos0 * exp(2.0 * b0) +
              1.0 - 4.0 * cos0 * exp(b0));
      sumC = -2.f *
             (c0 * cos1 - c1 * sin1 + c1 * sin1 * exp(2.0 * b1) + c0 * cos1 * exp(2.0 * b1) - 2.0 * c0 * exp(b1)) *
             exp(b1) /
             (exp(4.0 * b1) - 4.0 * cos1 * exp(3.0 * b1) + 2.0 * exp(2.0 * b1) + 4.0 * cos1 * cos1 * exp(2.0 * b1) +
              1.0 - 4.0 * cos1 * exp(b1));
      break;

    case DericheGaussianSecondDerivative:
      float aux = 12.0 * cos0 * exp(3.0 * b0) - 3.0 * exp(2.0 * b0) + 8.0 * cos0 * cos0 * cos0 * exp(3.0 * b0) -
                  12.0 * cos0 * cos0 * exp(4.0 * b0) - (3.0 * exp(4.0 * b0)) + 6.0 * cos0 * exp(5.0 * b0) -
                  exp(6.0 * b0) + 6.0 * cos0 * exp(b0) - (1.0 + 12.0 * cos0 * cos0 * exp(2.0 * b0));
      sumA = 4.0 * a0 * sin0 * exp(3.0 * b0) + a1 * cos0 * cos0 * exp(4.0 * b0) -
             (4.0 * a0 * sin0 * exp(b0) + 6.0 * a1 * cos0 * cos0 * exp(2.0 * b0)) +
             2.0 * a1 * cos0 * cos0 * cos0 * exp(b0) - 2.0 * a1 * cos0 * exp(b0) +
             2.0 * a1 * cos0 * cos0 * cos0 * exp(3.0 * b0) - 2.0 * a1 * cos0 * exp(3.0 * b0) + a1 * cos0 * cos0 -
             a1 * exp(4.0 * b0) + 2.0 * a0 * sin0 * cos0 * cos0 * exp(b0) -
             2.0 * a0 * sin0 * cos0 * cos0 * exp(3.0 * b0) - (a0 * sin0 * cos0 * exp(4.0 * b0) + a1) +
             6.0 * a1 * exp(2.0 * b0) + a0 * cos0 * sin0 * 2.0 * exp(b0) / (aux * sin0);
      aux = 12.0 * cos1 * exp(3.0 * b1) - 3.0 * exp(2.0 * b1) + 8.0 * cos1 * cos1 * cos1 * exp(3.0 * b1) -
            12.0 * cos1 * cos1 * exp(4.0 * b1) - 3.0 * exp(4.0 * b1) + 6.0 * cos1 * exp(5.0 * b1) - exp(6.0 * b1) +
            6.0 * cos1 * exp(b1) - (1.0 + 12.0 * cos1 * cos1 * exp(2.0 * b1));
      sumC = 4.0 * c0 * sin1 * exp(3.0 * b1) + c1 * cos1 * cos1 * exp(4.0 * b1) -
             (4.0 * c0 * sin1 * exp(b1) + 6.0 * c1 * cos1 * cos1 * exp(2.0 * b1)) +
             2.0 * c1 * cos1 * cos1 * cos1 * exp(b1) - 2.0 * c1 * cos1 * exp(b1) +
             2.0 * c1 * cos1 * cos1 * cos1 * exp(3.0 * b1) - 2.0 * c1 * cos1 * exp(3.0 * b1) + c1 * cos1 * cos1 -
             c1 * exp(4.0 * b1) + 2.0 * c0 * sin1 * cos1 * cos1 * exp(b1) -
             2.0 * c0 * sin1 * cos1 * cos1 * exp(3.0 * b1) - (c0 * sin1 * cos1 * exp(4.0 * b1) + c1) +
             6.0 * c1 * exp(2.0 * b1) + c0 * cos1 * sin1 * 2.0 * exp(b1) / (aux * sin1);
      sumA /= 2;
      sumC /= 2;
      break;
    }

    a0 /= (sumA + sumC);
    a1 /= (sumA + sumC);
    c0 /= (sumA + sumC);
    c1 /= (sumA + sumC);

    n[3] = exp(-b1 - 2 * b0) * (c1 * sin1 - cos1 * c0) + exp(-b0 - 2 * b1) * (a1 * sin0 - cos0 * a0);
    n[2] = 2 * exp(-b0 - b1) * ((a0 + c0) * cos1 * cos0 - cos1 * a1 * sin0 - cos0 * c1 * sin1) + c0 * exp(-2 * b0) +
           a0 * exp(-2 * b1);
    n[1] = exp(-b1) * (c1 * sin1 - (c0 + 2 * a0) * cos1) + exp(-b0) * (a1 * sin0 - (2 * c0 + a0) * cos0);
    n[0] = a0 + c0;

    d[3] = exp(-2 * b0 - 2 * b1);
    d[2] = -2 * cos0 * exp(-b0 - 2 * b1) - 2 * cos1 * exp(-b1 - 2 * b0);
    d[1] = 4 * cos1 * cos0 * exp(-b0 - b1) + exp(-2 * b1) + exp(-2 * b0);
    d[0] = -2 * exp(-b1) * cos1 - 2 * exp(-b0) * cos0;

    switch (filter_type)
    {
    case DericheGaussian:
    case DericheGaussianSecondDerivative:

      for (unsigned i = 0; i < 3; ++i)
      {
        dm[i] = d[i];
        nm[i] = n[i + 1] - d[i] * n[0];
      }
      dm[3] = d[3];
      nm[3] = -d[3] * n[0];
      break;

    case DericheGaussianFirstDerivative:

      for (unsigned i = 0; i < 3; ++i)
      {
        dm[i] = d[i];
        nm[i] = -(n[i + 1] - d[i] * n[0]);
      }
      dm[3] = d[3];
      nm[3] = d[3] * n[0];
      break;
    }
  };
} // namespace mln::morpho::experimental::details
