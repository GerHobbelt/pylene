#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/zip_image.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/io/imprint.hpp>

#include <boost/range/numeric.hpp>
#include <boost/timer.hpp>
#include <boost/tuple/tuple_io.hpp>

using namespace mln;

double test_native(const image2d<int>& a, const image2d<int>& b)
{
  const size_t* strides = a.strides();
  const char* ptra = (const char*) &(a(point2d {0,0}));
  const char* ptrb = (const char*) &(b(point2d {0,0}));

  double v = 0;
  unsigned nrows = a.nrows(), ncols = a.ncols();
  for (unsigned i = 0; i < nrows; ++i, ptra += strides[0], ptrb += strides[0])
    {
      const int* pa = (const int*) ptra, *pb = (const int*) ptrb;
      for (unsigned j = 0; j < ncols; ++j, ++pa, ++pb)
	v += *pa * *pb;
    }
  return v;
}

double test_iterator(const image2d<int>& a, const image2d<int>& b)
{
  double v = boost::inner_product(a.values(), b.values(), 0.0d);
  return v;
}

double test_zip(const image2d<int>& a, const image2d<int>& b)
{
  auto ima = imzip(a, b);

  double v = boost::accumulate(ima.values(), 0.0d, [](double v, const boost::tuple<const int&, const int&>& x)
                        { return v + boost::get<0>(x) * boost::get<1>(x); });
  return v;
}

double test_for(const image2d<int>& a, const image2d<int>& b)
{
  double v = 0;

  forall_v (int x, int y, a, b)
    v += x * y;

  return v;
}


int main()
{
  image2d<int> ima1(1000, 10000);
  image2d<int> ima2(1000, 10000);

  iota(ima1, 0);
  iota(ima2, 1);

  auto zp = imzip(ima1, ima2);
  static_assert(std::is_same<raw_image_tag, typename image_traits< decltype(zp) >::category> (), "lol");

  //io::imprint(imzip(ima1, ima2));

  boost::timer t;
  double r, thistime;

  std::cout << "Native pointer..." << std::endl;
  t.restart();
  for (int i = 0; i < 100; ++i)
    r  = test_native(ima1, ima2);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << " R:" << r <<std::endl;


  std::cout << "Normal Iterators..." << std::endl;
  t.restart();
  for (int i = 0; i < 100; ++i)
    r  = test_iterator(ima1, ima2);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << " R:" << r <<std::endl;


  std::cout << "Zip Iterators..." << std::endl;
  t.restart();
  for (int i = 0; i < 100; ++i)
    r  = test_zip(ima1, ima2);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << " R:" << r <<std::endl;

  std::cout << "Zip For loop..." << std::endl;
  t.restart();
  for (int i = 0; i < 100; ++i)
    r  = test_for(ima1, ima2);
  thistime = t.elapsed();
  std::cout << "Elapsed: " << thistime << " R:" << r <<std::endl;


}

