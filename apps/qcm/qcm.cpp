#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/accu/accumulators/sum.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <mln/core/win2d.hpp>
#include <mln/morpho/opening.hpp>
#include <sstream>

namespace mln
{

  point2d
  detect_offset(const image2d<uint8>& f)
  {
    rect2d se = make_rectangle2d(20, 50);

    image2d<bool> bin = eval(f < 150);

    //auto markers = morpho::opening(f, se);
    rect2d hse = make_rectangle2d(1,50);
    rect2d vse = make_rectangle2d(20,1);

    bin = morpho::erode(bin, hse);
    bin = morpho::erode(bin, vse);
    bin = morpho::dilate(bin, hse);
    bin = morpho::dilate(bin, vse);

    io::imsave(bin, "out.tiff");

    box2d dom = { {140, 2300}, {212, 2470} };
    auto sub = bin | dom;
    mln_foreach(auto px, sub.pixels())
      if (px.val())
        return px.point();

    std::cerr << "Marker not found" << std::endl;
    std::exit(1);
  }

  const rect2d qcmbox = make_rectangle2d(15, 40);
  const float qcmboxvdist = 50.2;
  const int qcmboxhdist = 60;
  const int qcmcolhdist = 481;
  const point2d pref = {168, 2389};

  static const char REPA = 0x01;
  static const char REPB = 0x02;
  static const char REPC = 0x04;
  static const char REPD = 0x08;
  static const char REPE = 0x10;
  static const char REPALL = 0x1F;

  image2d<rgb8> imdebug;

  template <class I>
  bool
  is_plain(const I& f, point2d p)
  {
    int count = accumulate(f | qcmbox(p), accu::features::sum<>());

    if (count > 250 and trace::verbose)
      {
        for (int i = p[1] - 20; i <= (p[1] + 20); ++i)
          {
            imdebug.at(p[0]-7,i) = rgb8{255,0,0};
            imdebug.at(p[0]+7,i) = rgb8{255,0,0};
          }
      }

    //std::cout << p << ":" << count << std::endl;
    return count > 250;
  }

  template <class I>
  char
  detect_question(const I& bin, point2d pos)
  {
    char response1 = 0;
    char response2 = 0;
    point2d p = pos;

    for (int i = 0; i < 5; ++i, p[1] += qcmboxhdist)
      if (is_plain(bin, p))
        response1 |= (1 << i);

    p = pos;
    p[0] += qcmboxvdist;

    for (int i = 0; i < 5; ++i, p[1] += qcmboxhdist)
      if (is_plain(bin, p))
        response2 |= (1 << i);

    if (response2 and response2 != REPALL)
      return response2;
    else if (response1 and response1 != REPALL)
      return response1;
    else
      return 0;
  }

  void
  detect_all(const image2d<uint8>& f, point2d offset = {0,0})
  {
    auto bin = f < 200;

    rect2d se = make_rectangle2d(15, 40);

    // Login
    {
      point2d plogin = point2d{228, 1895} + offset;
      point2df p = plogin;
      for (int j = 0; j < 6; ++j, p[1] += qcmboxhdist)
        {
          p[0] = plogin[0];
          for (int i = 0; i < 26; ++i, p[0] += qcmboxvdist)
            {
              if (is_plain(bin, p)) {
                std::cout << (char)('a' + i);
                break;
              }
            }
        }
      std::cout << "-";
      p[1] += qcmboxhdist;
      p[0] = plogin[0];
      for (int i = 0; i < 26; ++i, p[0] += qcmboxvdist)
        {
          if (is_plain(bin, p)) {
            std::cout << (char)('a' + i);
            break;
          }
        }
      std::cout << std::endl;
    }


    // Les questions
    {
      point2d p0 = point2d{1734, 156} + offset;

      point2df p = p0;
      for (int j = 0; j < 5; ++j, p[1] += qcmcolhdist) {
        p[0] = p0[0];
        for (int i = 0; i < 10; ++i, p[0] += 2*qcmboxvdist) {
          char res = detect_question(bin, p);
          std::cout << (j*10 + i + 1) << " ";

          for (int k = 0; k < 5; ++k)
            if (res & (1 << k))
              std::cout << (char)('a' + k);

          std::cout << std::endl;
        }
      }
    }

  }


}




int main(int argc, char** argv)
{
  using namespace mln;

  image2d<uint8> f;

  {
    image2d<rgb8> ima;
    io::imread(argv[1], ima);

    f = transform(ima, [](const rgb8& v) -> uint8 { return sum(v) / 3;});
  }

  resize(imdebug, f);
  copy(f, imdebug);

  point2d ref = detect_offset(f);
  point2d offset = ref - pref;

  detect_all(f, offset);
  io::imsave(imdebug, "out.tiff");

  std::cout << "Ref: " << ref << std::endl;
  std::cout << "Offset: " << offset << std::endl;
}
