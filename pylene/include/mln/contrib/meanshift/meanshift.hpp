#pragma once

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/range/foreach.hpp>

namespace mln
{
  namespace contrib
  {

    template <class V>
    mln::experimental::image2d<V> meanshift(const mln::experimental::image2d<V>& f, float hs, float hr)
    {
      int    SR    = 5;   // Spatial window radius
      int    NITER = 30;  // Maximal number of iteration
      float  eps   = 0.1; //
      double hs2   = hs * hs;
      double hr2   = hr * hr;
      double eps2  = eps * eps;

      typedef vec<double, value_traits<V>::ndim> value_t;
      typedef vec<double, 2>                     site_t;

      mln::experimental::image2d<V> out;
      resize(out, f);

      mln::experimental::se::rect2d win(2 * SR + 1, 2 * SR + 1);

      auto g = [](double x) -> double { return std::exp(-x); };
      mln_foreach_new (auto p, f.domain())
      {
        site_t   py = {static_cast<double>(p.x()), static_cast<double>(p.y())};
        value_t  vy = f(p).as_vec();

        bool stop = false;
        for (int i = 0; i <= NITER and (not stop); ++i)
        {
          site_t  s1 = literal::zero;
          value_t s2 = literal::zero;
          double  s3 = 0;

          for (auto q : win(p))
          {
            if (f.domain().has(q))
            {
              site_t vq = {static_cast<double>(q.x()), static_cast<double>(q.y())};
              double d0 = l2norm_sqr(py - vq) / hs2;
              double d1 = l2norm_sqr(vy - f(q).as_vec()) / hr2;
              double d  = g(d0 + d1);
              s1 += d * vq.as_vec();
              s2 += d * f(q).as_vec();
              s3 += d;
            }
          }
          site_t  new_py = s1 / s3;
          value_t new_vy = s2 / s3;
          stop           = (new_py == py);
          if (not stop)
          {
            double d0 = l2norm_sqr(py - new_py);
            double d1 = l2norm_sqr(vy - new_vy);
            stop      = (d0 + d1) < eps2;
            // if (stop)
            //  std::cout << "#NITER:" << i << p << new_py << std::endl;
          }
          py = new_py;
          vy = new_vy;
        }
        // std::cout << vy.as_vec() << std::endl;
        out(p) = (V)vy.as_vec();
      }
      return out;
    }

  } // namespace contrib

} // namespace mln
