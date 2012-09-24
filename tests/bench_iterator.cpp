#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/wrt_offset.hpp>
#include <algorithm>
#include <numeric>
#include <boost/timer.hpp>


using namespace mln;
/*
double test_viter(const image2d<int>& ima)
{
  double v = 0;
  auto tmp = ima.values();
  for(auto x: tmp) {
    v += x;
  }

  return v;
}

double test_pixter(const image2d<int>& ima)
{
  double v = 0;
  auto tmp = ima.pixels();
  for(auto x: tmp)
    v += x.val();

  return v;
}

double test_piter(const image2d<int>& ima)
{
  double v = 0;
  for(auto p: ima.domain()) {
    v += ima(p);
  }
  return v;
}
*/


double test_native(const int* ima2, int nrows, int ncols)
{
  static int u = 0;
  double v = 0;
  int end = nrows * ncols;
  for (int i = 0; i < end; ++i)
    v += ima2[i];
  u++;
  return v;
}

/*
double test_nbh(image2d<int>& ima)
{
  typedef image2d<int> I;

  double u = 0;
  for (auto pix: ima.pixels())
    {
      auto tmp = c8(pix);
      for (auto n: tmp)
        u += n.val();
    }

  return u;
}

double test_nbh_pix(const image2d<int>& ima)
{
  typedef const image2d<int> I;

  double u = 0;
  //c8_t::nbh<I>::type sliding(ima, c8_t::dpoints);

  for (auto pix: ima.pixels())
    {
      auto tmp = c8(pix);
      for (auto n: tmp)
        u += n.val();
    }

  return u;
}

// double test_nbh_piter(const image2d<int>& ima)
// {
//   double v = 0;

//   for(auto p: ima.domain())
//     for (auto n: c8(p))
//       v += ima(n);

//   return v;
// }



double test_nbh_wo_object_creation(const image2d<int>& ima)
{
  typedef const image2d<int> I;

  double u = 0;

  image2d<int>::const_pixel_type pix;
  c8_t::nbh<I::const_pixel_type> sliding(c8_t::dpoints, pix);

  for (auto pix_ : ima.pixels())
    {
      pix = pix_;
      for (auto& n: sliding)
        u += n.val();
    }


  return u;
}


double test_cstor(const image2d<int>& ima)
{
  image2d<int>::const_pixel_type pix = ima.pix_at(point2d{0,0});

  double u = 0;

  auto tmp = c8(pix);
  for (auto n: tmp)
    u += n.val();

  return u;
}



double test_native_nbh(const image2d<int>& ima)
{
  double r2 = 0;

  const int sz = 8;
  auto dpoints = c8_t::dpoints;


  const char* ptr2 = (char*) &ima(ima.domain().pmin);
  const size_t* strides = ima.strides();
  const point2d pmax = ima.domain().shape();
  ptrdiff_t offsets[8];
  wrt_offset(ima, dpoints, offsets);

  for (int x = 0; x < pmax[0]; ++x, ptr2 += strides[0])
    {
      const char* ptr = ptr2;
      for (int y = 0; y < pmax[1]; ++y, ptr += strides[1])
        for (int k = 0; k < sz; ++k)
          r2 += *(const int*)(ptr + offsets[k]);
    }

  return r2;
}
*/

#define foreach(v, rng)				\
  auto _mln_iter_ = rng.iter();			\
  for (_mln_iter_.init(); !_mln_iter_.finished(); _mln_iter_.next())	\
    if (bool _mln_break_ = false) {} else				\
      for (v = *_mln_iter_; !_mln_break_; _mln_break_ = true)


template <typename T>
void iota(image2d<T>& ima, T v)
{
  foreach(auto& x, ima.values())
    x = v++;
}


void display()
{
  const int nrows = 5, ncols = 5;
  image2d<int> ima(nrows, ncols);
  iota(ima, 0);

  {
    std::cout << "Display forward site iterator." << std::endl;
    foreach(auto p, ima.domain())
      std::cout << p << ",";
    std::cout << std::endl;
  }
  {
    std::cout << "Display forward value iterator." << std::endl;
    foreach(auto x, ima.values())
      std::cout << x << ",";
    std::cout << std::endl;
  }
  {
    std::cout << "Display forward pixel iterator." << std::endl;
    foreach(auto x, ima.pixels())
      std::cout << "(" << x.point() << ":" << x.val() << "),";
    std::cout << std::endl;
  }
}



int main()
{

  const int nrows = 5, ncols = 5;

  typedef image2d<int> I;
  image2d<int> ima(nrows, ncols);
  iota(ima, 0);

  display();

    /*;
  std::iota(std::begin(ima.values()), std::end(ima.values()), 1);

  // bench using iterators
  static const int ntest = 10;

  boost::timer t;
  double r, r1, r2, r3, thistime;

  std::cout << "Point Iterators..." << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r  = test_piter(ima);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << std::endl;
  std::cout << "Result: " << r << std::endl;

  std::cout << "Value Iterators..." << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r1  = test_viter(ima);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << std::endl;
  std::cout << "Result: " << r1 << std::endl;


  std::cout << "Pixel Iterators..." << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r3  = test_pixter(ima);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << std::endl;
  std::cout << "Result: " << r3 << std::endl;

  // defaut with pointers
  int* ima2 = new int[nrows * ncols];
  std::iota(ima2, ima2 + nrows * ncols, 1);


  std::cout << "Native..." << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r2 = test_native(ima2, nrows, ncols);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << std::endl;

  std::cout << r1 << " " << r2 << " " << r3 << std::endl;


  // bench neighborhood

  
  std::cout << "Neighborhood... piter/niter" << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r1 = test_nbh_piter(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r1 << std::endl;
  

  std::cout << "Neighborhood... v iterators" << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r1 = test_nbh(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r1 << std::endl;


  std::cout << "Neighborhood...w pixels" << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r1 = test_nbh_pix(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r1 << std::endl;

  std::cout << "Neighborhood WO object creation..." << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r1 = test_nbh_wo_object_creation(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r1 << std::endl;


  std::cout << "Neighborhood native..." << std::endl;
  t.restart();
  for (int i = 0; i < ntest; ++i)
    r1 = test_native_nbh(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r1 << std::endl;

  delete [] ima2;
  */
}


