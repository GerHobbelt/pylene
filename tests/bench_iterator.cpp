#include <mln/core/ndimage.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/macros.hpp>
#include <mln/core/wrt_offset.hpp>
#include <algorithm>
#include <numeric>
#include <boost/timer.hpp>


using namespace mln;

double test_nbh(const image2d<int>& ima)
{
  typedef const image2d<int> I;

  double u = 0;
  typedef c4_::nbh<I>::type I2;
  for (int i = 0; i < 10; ++i) {
    //mln_pixter(I) p = ima.pixbegin(), pend = ima.pixend();



    forall(p, ima)
      {
        I2 nbh = c4(p);
        forall(n, nbh)
          {
      // {
      //   
      //   mln_pixter(I2) n = nbh.pixbegin(), nend = nbh.pixend();
      //   for (; n != nend; ++n) {
          u += n.val();
          }
      }
  }
  return u;
}

double test_native_nbh(const image2d<int>& ima)
{
  double r2 = 0;
  const point2d dpoints[4] = { {-1,0}, {0,-1}, {0,1}, {1,0} };

  for (int i = 0; i < 10; ++i) {
    const char* ptr2 = (char*) &ima(ima.domain().pmin);
    const size_t* strides = ima.strides();
    const point2d pmax = ima.domain().shape();
    ptrdiff_t offsets[4];
    wrt_offset(ima, dpoints, offsets);

    for (int x = 0; x < pmax[0]; ++x, ptr2 += strides[0])
      {
        const char* ptr = ptr2;
        for (int y = 0; y < pmax[1]; ++y, ptr += strides[1])
          for (int k = 0; k < 4; ++k)
            r2 += *(const int*)(ptr + offsets[k]);
      }
  }
  return r2;
}



int main()
{

  const int nrows = 10000, ncols = 1000;

  typedef image2d<int> I;
  image2d<int> ima(nrows, ncols);
  std::iota(ima.begin(), ima.end(), 1);

  // bench using iterators

  boost::timer t;
  double r1, r2;

  std::cout << "Itérators..." << std::endl;
  for (int i = 0; i < 10; ++i)
    r1  = std::accumulate(ima.begin(), ima.end(), (double) 0);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;

  // defaut with pointers
  int* ima2 = new int[nrows * ncols];
  std::iota(ima2, ima2 + nrows * ncols, 1);



  t.restart();
  std::cout << "Native..." << std::endl;
  for (int i = 0; i < 10; ++i)
    r2 = std::accumulate(ima2, ima2 + nrows * ncols, (double) 0);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;

  std::cout << r1 << " " << r2 << std::endl;


  // bench neighborhood

  t.restart();
  std::cout << "Neighborhood..." << std::endl;
  r1 = test_nbh(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r1 << std::endl;


  std::cout << "Neighborhood native..." << std::endl;
  t.restart();
  r2 = test_native_nbh(ima);
  std::cout << "Elapsed: " << t.elapsed() << std::endl;
  std::cout << r2 << std::endl;

  delete [] ima2;
}


