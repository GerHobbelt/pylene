#include "border_detection.hpp"
#include <mln/core/neighb2d.hpp>
#include <mln/io/imsave.hpp>
#include <mln/colors/literal.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/labeling/accumulate.hpp>
#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/morpho/structural/opening.hpp>
#include <Eigen/Dense>


typedef Eigen::Array<double, Eigen::Dynamic, 2> MyMatrix;

static
void
regression(const MyMatrix& A, double& a, double& b, double& c)
{
  auto psum = A.colwise().sum();
  auto psum2 = A.square().colwise().sum();

  double sxi = psum(0);
  double syi = psum(1);
  double sxiyi = A.rowwise().prod().sum();
  double sxi2 = psum2(0);
  double syi2 = psum2(1);

  double delta = (sxi2 * syi2) - (sxiyi*sxiyi);
  double alpha = syi2; // /delta;
  double beta = -sxiyi; // /delta;
  double gamma = sxi2; // / delta;

  a = alpha * sxi + beta * syi;
  b = beta * sxi + gamma * syi;
  c = delta;

  //std::cout << "D: " << delta << std::endl;
  //std::cout << "a: " << a << " b: " << b << std::endl;
}

static
std::pair<int, int>
detect_start_end_x(const std::vector<mln::point2d>& V)
{
  int s = START_END_STEP * BORDER_IN;

  int start, end;
  int i;
  for (i = s; i < int(V.size() - s); i += BORDER_IN)
    if (abs(V[i][1] - V[i-s][1]) < START_END_STEP_T)
      break;
  start = i;

  for (i = (V.size() - s - 1); i >= 0; i -= BORDER_IN)
    if (abs(V[i][1] - V[i+s][1]) < START_END_STEP_T)
      break;
  end = i;


  //std::cout << "Start: " << start << " end: " << end << std::endl;
  return {start, end};
}

static
std::pair<int, int>
detect_start_end_y(const std::vector<mln::point2d>& V)
{
  int s = START_END_STEP * BORDER_IN;

  int start, end;
  int i;
  for (i = s; i < int(V.size() - s); i += BORDER_IN)
    if (abs(V[i][0] - V[i-s][0]) < START_END_STEP_T)
      break;
  start = i;

  for (i = (V.size() - s - 1); i >= 0; i -= BORDER_IN)
    if (abs(V[i][0] - V[i+s][0]) < START_END_STEP_T)
      break;
  end = i;

  //std::cout << "Start: " << start << " end: " << end << std::endl;
  return {start, end};
}

static
mln::point2df
compute_inter(double a1, double b1, double c1,
              double a2, double b2, double c2)
{
  float y = (a2*c1-a1*c2) / (b1*a2 - a1*b2);
  float x = (b2*c1-b1*c2) / (a1*b2 - b1*a2);
  return {y,x};
}



pquad_t
border_detection(const mln::image2d<mln::rgb8>& f,
                 const char* debug_filename)
{
  using namespace mln;

  mln_entering("border_detection");


  image2d<uint8> dist;
  resize(dist, f).border(5);

  transform(f, [](rgb8 x) -> uint8 {
      int d1 = l1norm(x - colors::literal::red);
      int d2 = l1norm(x - colors::literal::black);
      int d = std::min(d1, d2);
      return std::min<int>(d/3, 255);
    }, dist);


  //auto myse1 = make_rectangle2d(11,1);
  auto myse2 = make_rectangle2d(1,11);
  {
    auto tmp = transpose(dist);
    tmp = morpho::structural::opening(tmp, myse2);
    transpose(tmp, dist);
  }
  dist = morpho::structural::opening(dist, myse2);


  image2d<unsigned> lbls;
  unsigned nlabel;
  {
    auto bin = dist > RED_THRESHOLD;
    std::tie(lbls, nlabel) = labeling::blobs(bin, c8, 0u);
  }

  auto counts = labeling::p_accumulate(lbls, nlabel, accu::features::count<> ());

  // Compute the subdomain of interest
  box2d subdomain;
  image2d<bool> bin;
  {
    unsigned imax = 1;
    for (unsigned i = 1; i <= nlabel; ++i)
      if (counts[i] > counts[imax])
        imax = i;

    accu::accumulators::infsup<point2d> acc;
    acc.init();
    mln_foreach(auto px, lbls.pixels())
      if (px.val() == imax)
        acc.take(px.point());

    subdomain = {accu::extractor::inf(acc) - 50,
                 accu::extractor::sup(acc) + 10};

    bin = image2d<bool>(subdomain.shape()[0],
                        subdomain.shape()[1]);
    copy((lbls == imax) | subdomain, bin);
    //bin = eval((lbls == imax) | subdomain);
  }

  // Compute the equations
  // ax + by = 1

  double a1, b1, c1;
  double a2, b2, c2;
  double a3, b3, c3;
  double a4, b4, c4;
  int nrows = bin.nrows(), ncols = bin.ncols();

  {
    std::vector<point2d> data;
    MyMatrix A;
    int begin, end;
    // Left
    {
      for (int i = 0; i < nrows; ++i)
        {
          int count = 0;
          for (int j = 0; j < ncols and count < 5; ++j)
            if (bin.at(i,j))
              {
                data.push_back({(short)i,(short)j});
                ++count;
              }
        }
      std::tie(begin, end) = detect_start_end_x(data);
      //std::cout << "Start: " << (data[begin]+subdomain.pmin)  << " end: " << (data[end] + subdomain.pmin) << std::endl;

      int n = end-begin;
      A.resize(n, 2);
      for (int i = begin; i < end; ++i) {
        A(i-begin,0) = data[i][1];
        A(i-begin,1) = data[i][0];
      }
      regression(A, a1, b1, c1);
    }

    // Right
    {
      data.clear();
      for (int i = 0; i < nrows; ++i)
        {
          int count = 0;
          for (int j = ncols-1; j >= 0 and count < 5; --j)
            if (bin.at(i,j))
              {
                data.push_back({(short)i,(short)j});
                ++count;
              }
        }
      std::tie(begin, end) = detect_start_end_x(data);
      int n = end-begin;
      A.resize(n, 2);
      for (int i = begin; i < end; ++i) {
        A(i-begin,0) = data[i][1];
        A(i-begin,1) = data[i][0];
      }

      regression(A, a2, b2, c2);
    }

    // Up
    {
      data.clear();
      for (int j = 0; j < ncols; ++j)
        {
          int count = 0;
          for (int i = 0; i < nrows and count < 5; ++i)
            {
              if (bin.at(i,j))
                {
                  data.push_back({(short)i,(short)j});
                  ++count;
                }
            }
        }
      std::tie(begin, end) = detect_start_end_y(data);
      int n = end-begin;
      A.resize(n, 2);
      for (int i = begin; i < end; ++i) {
        A(i-begin,0) = data[i][1];
        A(i-begin,1) = data[i][0];
      }
      regression(A, a3, b3, c3);
    }

    // Down
    {
      data.clear();
      for (int j = 0; j < ncols; ++j)
        {
          int count = 0;
          for (int i = nrows-1; i >= 0 and count < 5; --i)
            {
              if (bin.at(i,j))
                {
                  data.push_back({(short)i,(short)j});
                  ++count;
                }
            }
        }
      std::tie(begin, end) = detect_start_end_y(data);
      int n = end-begin;
      A.resize(n, 2);
      for (int i = begin; i < end; ++i) {
        A(i-begin,0) = data[i][1];
        A(i-begin,1) = data[i][0];
      }
      regression(A, a4, b4, c4);
    }
  }

  if (debug_filename) {
    image2d<rgb8> out = clone(f);
    for (short x = 0; x < (int)f.ncols(); ++x)
      {
        short y1 = (c3 - a3 * x) / b3;
        short y2 = (c4 - a4 * x) / b4;
        point2d p = point2d{y1,x} + subdomain.pmin;
        point2d q = point2d{y2,x} + subdomain.pmin;
        if (out.domain().has(p)) out(p)[1] += 127;
        if (out.domain().has(q)) out(q)[1] += 127;
      }

    for (short y = 0; y < (int)f.nrows(); ++y)
      {
        short x1 = (c1 - b1 * y) / a1;
        short x2 = (c2 - b2 * y) / a2;
        point2d p = point2d{y,x1} + subdomain.pmin;
        point2d q = point2d{y,x2} + subdomain.pmin;
        if (out.domain().has(p)) out(p)[1] += 127;
        if (out.domain().has(q)) out(q)[1] += 127;
      }
    io::imsave(out, debug_filename);
  }

  pquad_t res;
  point2df tl = compute_inter(a1,b1,c1,a3,b3,c3) + subdomain.pmin;
  point2df tr = compute_inter(a2,b2,c2,a3,b3,c3) + subdomain.pmin;
  point2df bl = compute_inter(a1,b1,c1,a4,b4,c4) + subdomain.pmin;
  point2df br = compute_inter(a2,b2,c2,a4,b4,c4) + subdomain.pmin;

  // on est trouné à 90 degrés.
  res[0] = bl;
  res[1] = tl;
  res[2] = br;
  res[3] = tr;


  mln_exiting();
  return res;
}
