#include <QApplication>
#include <QtGui>


#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/accu/accumulators/mean.hpp>

#include <mln/io/imread.hpp>

#include <mln/morpho/tos/tos.hpp>
#include <mln/morpho/filtering.hpp>

#include <mln/qt/mainwin.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/addborder.hpp>
#include <apps/tos/gradient.hpp>
#include <apps/tos/mumford_shah.hpp>

#include "qattribute.hpp"
#include "dispatcher.hpp"

namespace mln
{
  template <typename T>
  image2d<T>
  interpolate_k1(const image2d<T>& ima)
  {
    image2d<T> out(2*ima.nrows()-1, 2*ima.ncols()-1);
    typedef point2d P;
    mln_foreach(point2d p, ima.domain())
      {
	T a = ima.at(p),
	  b = ima.at(p + P{0,1}),
	  c = ima.at(p + P{1,0}),
	  d = ima.at(p + P{1,1});

	point2d q = 2 * p;
	out.at(q) = ima.at(p);
	out.at(q + P{0,1}) = (a + b) / 2;
	out.at(q + P{1,0}) = (a + c) / 2;
	out.at(q + P{1,1}) = (a + b + c + d) / 4;
      }

    return out;
  }


  template <typename V, typename W>
  image2d<V>
  set_mean_on_node(const image2d<V>& ima,
                   const image2d<W>& K,
                   const std::vector<unsigned>& S,
                   const image2d<unsigned>& parent)
  {
    typedef accu::accumulators::mean<V, vec3u> Acc;

    image2d<V> mean;
    image2d<Acc> accus;

    resize(accus, ima);
    resize(mean, ima);

    // Accumulate
    {
      for (int i = S.size()-1; i > 0 ; --i)
	{
	  unsigned k = S[i];
	  if (K1::is_face_2(ima.point_at_index(k)))
            accus[k].take(ima[k]);
	  accus[parent[k]].take(accus[k]);
	}
      accus[S[0]].take(ima[S[0]]);
    }

    // reconstruct
    {
      mean[S[0]] = (V) accu::extractor::mean(accus[S[0]]);
      for (unsigned k : S)
	{
	  if (K[parent[k]] != K[k])
	    mean[k] = (V) accu::extractor::mean(accus[k]);
	  else
	    mean[k] = mean[parent[k]];
	}
    }
    return mean;
  }

}


int main(int argc, char** argv)
{
  using namespace mln;


  if (argc != 2)
    std::cerr << "Usage: " << argv[0] << " ima.pgm" << std::endl;

  QApplication a(argc, argv);

  image2d<rgb8> ima;
  io::imread(argv[1], ima);


  typedef UInt<9> V;
  typedef image2d<V> I;
  I r = transform(ima, [](rgb8 v) -> V { return v[0] * 2; });
  I g = transform(ima, [](rgb8 v) -> V { return v[1] * 2; });
  I b = transform(ima, [](rgb8 v) -> V { return v[2] * 2; });
  I rr = addborder(r);
  I gg = addborder(g);
  I bb = addborder(b);

  image2d<V> rK, gK, bK;
  image2d<unsigned> rparent, gparent, bparent;
  std::vector<unsigned> rS, gS, bS;
  std::tie(rK, rparent, rS) = morpho::ToS(rr, c4);
  std::tie(gK, gparent, gS) = morpho::ToS(gg, c4);
  std::tie(bK, bparent, bS) = morpho::ToS(bb, c4);

  auto r_area = morpho::area_compute(rK, rparent, rS, K1::is_face_2);
  auto g_area = morpho::area_compute(gK, gparent, gS, K1::is_face_2);
  auto b_area = morpho::area_compute(bK, bparent, bS, K1::is_face_2);

  int size = 7;
  auto grad_r = interpolate_k1(gradient(rr, size));
  auto grad_g = interpolate_k1(gradient(gg, size));
  auto grad_b = interpolate_k1(gradient(bb, size));

  image2d<unsigned> area;
  resize(area, r_area);
  mln_foreach(const point2d& p, area.domain())
    {
      int r = grad_r(p), g = grad_g(p), b = grad_b(p);
      int ar = r_area(p), ag = g_area(p), ab = b_area(p);

      if (r >= g) {
	area(p) = (r >= b) ? ar : ab;
      } else {
	area(p) = (g >= b) ? ag : ab;
      }
    }


  image2d<unsigned> K;
  std::vector<unsigned> S;
  image2d<unsigned> parent;
  std::tie(K, parent, S) = morpho::ToS(area, c4);

  unsigned maxarea = S[0];

  auto Kui8 = transform(K, [maxarea](const unsigned& v) -> uint8 {
      return (float) v / maxarea * 255;
    });

  auto f = interpolate_k1(addborder(ima));
  image2d<float> energy = compute_energy(f, K, parent, S);
  image2d<float> cenergy = close(energy, K, parent, S);

  qt::MainWindow<uint8> w1(Kui8);
  w1.show();

  auto f2 = interpolate_k1(f);
  qt::MainWindow<rgb8> w2(f2);
  w2.show();

  auto mean = set_mean_on_node(f2, K, S, parent);
  qt::MainWindow<rgb8> w3(f2);
  w3.show();


  QAttribute<unsigned> wattr1(K, parent, QString("Gray values"));
  wattr1.show();

  QAttribute<float> wattr2(energy, parent, QString("Mumford shah"));
  wattr2.show();

  QAttribute<float> wattr3(cenergy, parent, QString("Mumford shah (clo)"));
  wattr3.show();


  QDispatcher disp(parent, S);
  disp.addImageWindow(&w1);
  disp.addImageWindow(&w2);
  disp.addImageWindowToFilter(&w3, mean);
  disp.addAttribute(&wattr1);
  disp.addAttribute(&wattr2);
  disp.addAttribute(&wattr3);

  //  QObject::connect(&main, SIGNAL(pointSelected(const point2d&)),
  //&wattr, SLOT(plotNode(const point2d&)));


  // QImage image("../../../img/small.pgm");
  // QGraphicsPixmapItem item(QPixmap::fromImage(image));
  // QGraphicsScene* m_scene = new QGraphicsScene;
  // m_scene->addItem(&item);

  // QGraphicsView* view = new QGraphicsView(m_scene);
  // QMainWindow win; 
  // win.setCentralWidget(view);
  // win.show();

  a.exec();
}
