#include <QApplication>
#include <QtGui>

#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/transform.hpp>

#include <mln/io/imread.hpp>
#include <mln/morpho/tos/tos.hpp>
#include <mln/morpho/filtering.hpp>

#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/mumford_shah.hpp>
#include <apps/tos/addborder.hpp>
#include <apps/tos/objdetection.hpp>
#include <mln/qt/mainwin.hpp>
#include "qattribute.hpp"
#include "dispatcher.hpp"



int main(int argc, char** argv)
{
  using namespace mln;


  if (argc != 2)
    std::cerr << "Usage: " << argv[0] << " ima.pgm" << std::endl;

  QApplication a(argc, argv);

  image2d<uint8> ima;
  io::imread(argv[1], ima);

  typedef UInt<9> V;
  image2d<uint8> bima = addborder(ima);
  image2d<uint8> f = interpolate_k1(bima);
  image2d<V> ima_ = transform(bima, [](uint8 x) -> V { return x*2; });

  //auto f = interpolate_median(addborder(ima), UInt<9> ());
  image2d< UInt<9> > K;
  std::vector<unsigned> S;
  image2d<unsigned> parent;

  std::tie(K, parent, S) = morpho::ToS(ima_, c4);


  auto Kui8 = transform(K, [](const UInt<9>& v) -> uint8 {
      return v / 2;
    });

  image2d< internal::energy_t<uint8> > imacc;
  image2d<float> energy = compute_energy(bima, K, parent, S, imacc);

  image2d<float> cenergy = clone(energy);
  {
    int lambda = 5;
    auto cK = clone(K);
    auto cParent = clone(parent);
    cenergy[S[0]] = value_traits<float>::max();
    getObjects(cenergy, cK, cParent, S, lambda);
    cenergy[S[0]] = 0;
  }

  //image2d<float> cenergy = close(energy, K, parent, S);
  image2d<unsigned> Area = morpho::area_compute(K, parent, S, K1::is_face_2);

  qt::MainWindow<uint8> w1(Kui8);
  w1.show();

  qt::MainWindow<uint8> w2(Kui8);
  w2.show();

  QAttribute<uint8> wattr1(Kui8, parent, QString("Gray values"));
  wattr1.show();

  QAttribute<float> wattr2(energy, parent, QString("Mumford shah"));
  wattr2.show();

  QAttribute<float> wattr3(cenergy, parent, QString("Mumford shah (clo)"));
  wattr3.show();

  QAttribute<unsigned> wattr4(Area, parent, QString("Grain"));
  wattr4.show();


# define IF_NOT_NULL(v, XEXPR) ((v) == 0) ? 0 : (XEXPR);

  auto attr5 = transform(imacc,[](const internal::energy_t<uint8> x) -> float {
      return IF_NOT_NULL(x.m_e_length, x.alpha * x.m_e_sumcurv / x.m_e_length); });
  QAttribute<float> wattr5(attr5, parent, QString("Internal Energy (sum_curv / e_length)"));
  wattr5.show();

  auto attr6 = transform(imacc,[](const internal::energy_t<uint8> x) -> float { return IF_NOT_NULL(x.m_v_n_int, x.external_energy()); });
  QAttribute<float> wattr6(attr6, parent, QString("External Energy"));
  wattr6.show();

  auto attr7 = transform(imacc,[](const internal::energy_t<uint8> x) -> float { return IF_NOT_NULL(x.m_e_length, x.beta / x.m_e_length); });
  QAttribute<float> wattr7(attr7, parent, QString("Constraint Energy"));
  wattr7.show();


  QDispatcher disp(parent, S);
  disp.addImageWindow(&w1);
  disp.addImageWindowToFilter(&w2,
			      transform(Kui8, [] (uint8 x) -> rgb8 { return rgb8{x,x,x}; })
			      );
  disp.addAttribute(&wattr1);
  disp.addAttribute(&wattr2);
  disp.addAttribute(&wattr3);
  disp.addAttribute(&wattr4);
  disp.addAttribute(&wattr5);
  disp.addAttribute(&wattr6);
  disp.addAttribute(&wattr7);
  //disp.addAttribute(&wattr1);

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
