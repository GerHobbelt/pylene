#pragma once

#include "../tracker_impl.hpp"

#include <numbers>

namespace scribo::internal
{
  struct OneEuros : public Tracker_impl
  {
    struct OneEuro
    {
      double freq;
      double mincutoff;
      double beta_;
      double dcutoff;

      struct LowPassFilter
      {
        double y, a, s;
        bool   initialized;

        void setAlpha(double alpha)
        {
          // if (alpha <= 0.0 || alpha > 1.0)
          //   throw std::range_error("alpha should be in (0.0., 1.0]");
          a = alpha;
        }

        LowPassFilter(double alpha, double initval = 0.0)
        {
          y = s = initval;
          setAlpha(alpha);
          initialized = false;
        }

        double filter(double value)
        {
          double result;
          if (initialized)
            result = a * value + (1.0 - a) * s;
          else
          {
            result      = value;
            initialized = true;
          }
          y = value;
          s = result;
          return result;
        }

        double filterWithAlpha(double value, double alpha)
        {
          setAlpha(alpha);
          return filter(value);
        }

        bool hasLastRawValue(void) { return initialized; }

        double lastRawValue(void) { return y; }
      };

      LowPassFilter x;
      LowPassFilter dx;

      double obs_value;
      int    last_t;

      double alpha(double cutoff)
      {
        double te  = 1.0 / freq;
        double tau = 1.0 / (2 * std::numbers::pi * cutoff);
        return 1.0 / (1.0 + tau / te);
      }

      double predict()
      {
        double dvalue  = obs_value - x.lastRawValue(); // Pente
        double edvalue = dx.filterWithAlpha(dvalue, alpha(dcutoff));
        double cutoff  = mincutoff + beta_ * fabs(edvalue);
        return x.filterWithAlpha(obs_value, alpha(cutoff));
      }

      void integrate(double value, int t)
      {
        obs_value = value;

        freq   = 1.f / static_cast<float>(t - last_t);
        last_t = t;
      }

      OneEuro(double initval, int t, double mincutoff, double beta_, double dcutoff)
        : freq(1.0)
        , mincutoff(mincutoff)
        , beta_(beta_)
        , dcutoff(dcutoff)
        , x(LowPassFilter(alpha(mincutoff), initval))
        , dx(LowPassFilter(alpha(dcutoff), 0.0))
        , obs_value(initval)
        , last_t(t)
      {
      }
    };

    OneEuro x;
    OneEuro thick;
    OneEuro lumi;

    OneEuros(int t_integration, Eigen::Matrix<float, 3, 1> observation, const Descriptor& descriptor);
    ~OneEuros() override = default;

    void predict() override;
    void integrate(int t, const Descriptor& descriptor) override;
  };
} // namespace scribo::internal