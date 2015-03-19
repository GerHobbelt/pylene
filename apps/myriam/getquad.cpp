#include "getquad.hpp"
#include <mln/accu/accumulators/covariance.hpp>
#include <mln/colors/literal.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/io/imsave.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/draw/range/line2d.hpp>
#include <Eigen/Dense>

pquad_t
getquad(const mln::image2d<bool>& bin,
        const char* debug_filename)
{
  using namespace mln;
  Eigen::Array<double, 2, 2> COV;
  Eigen::Array<double, 2, 2> P, Pinv;

  // 1. Computee covariance matrix.
  {
    accu::accumulators::covariance<point2d, double, double> accu;

    mln_foreach(auto px, bin.pixels())
      if (px.val())
        accu.take(px.point());

    COV = accu.to_result();
  }



  // 2. Compute the eigen vectors
  {
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eigensolver(COV.matrix());
    if (eigensolver.info() != Eigen::Success)
      std::abort();
    P = eigensolver.eigenvectors();
    Pinv = P.matrix().inverse();

    // Debug
    std::cout << "COV: \n" << COV << std::endl;
    std::cout << "Lambdas: \n" <<  eigensolver.eigenvalues() << std::endl;
    std::cout << "P: \n" << P << std::endl;
    std::cout << "Pinv: \n" << Pinv << std::endl;
  }


  // 3. Compute the bbox
  point2df pmin, pmax;
  {
    accu::accumulators::infsup<point2df> accu;

    mln_foreach(auto px, bin.pixels())
      if (px.val())
        {
          point2df p;
          p[0] = P(0,0) * px.point()[0] + P(1,0) * px.point()[1];
          p[1] = P(0,1) * px.point()[0] + P(1,1) * px.point()[1];
          accu.take(p);
        }

    pmin = accu::extractor::inf(accu);
    pmax = accu::extractor::sup(accu);
  }

  pquad_t res;
  res.tl[0] = Pinv(0,0) * pmin[0] + Pinv(1,0) * pmin[1];
  res.tl[1] = Pinv(0,1) * pmin[0] + Pinv(1,1) * pmin[1];

  res.tr[0] = Pinv(0,0) * pmin[0] + Pinv(1,0) * pmax[1];
  res.tr[1] = Pinv(0,1) * pmin[0] + Pinv(1,1) * pmax[1];

  res.br[0] = Pinv(0,0) * pmax[0] + Pinv(1,0) * pmax[1];
  res.br[1] = Pinv(0,1) * pmax[0] + Pinv(1,1) * pmax[1];

  res.bl[0] = Pinv(0,0) * pmax[0] + Pinv(1,0) * pmin[1];
  res.bl[1] = Pinv(0,1) * pmax[0] + Pinv(1,1) * pmin[1];

  std::cout << res.tl << ","
            << res.tr << ","
            << res.br << ","
            << res.bl << std::endl;


  if (debug_filename != NULL)
    {
      accu::accumulators::infsup<point2df> accu;
      accu.take(bin.domain().pmin);
      accu.take(bin.domain().pmax);
      accu.take(res.tl);
      accu.take(res.tr);
      accu.take(res.bl);
      accu.take(res.br);

      box2d newdomain = { (point2d) accu::extractor::inf(accu),
                          (point2d) accu::extractor::sup(accu) };


      image2d<rgb8> out(newdomain);
      fill(out | where(bin), colors::literal::white);
      fill(out | draw::line2d(res.tl, res.tr), colors::literal::red);
      fill(out | draw::line2d(res.tr, res.br), colors::literal::red);
      fill(out | draw::line2d(res.br, res.bl), colors::literal::red);
      fill(out | draw::line2d(res.bl, res.tl), colors::literal::red);

      io::imsave(out, debug_filename);
    }


  return res;
}
