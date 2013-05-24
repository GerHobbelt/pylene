#include <QApplication>
#include <QtGui>

#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/transform.hpp>

#include <mln/io/imread.hpp>
#include <mln/morpho/tos/tos.hpp>
#include <mln/qt/mainwin.hpp>
#include <apps/tos/Kinterpolate.hpp>

#include <apps/tos/mumford_shah.hpp>
#include "qattribute.hpp"
#include "dispatcher.hpp"

#include "tosapp.hpp"
#include <apps/tos/mumford_shah.hpp>
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

  auto f = interpolate_median(ima, UInt<9> ());
  image2d< UInt<9> > K;
  std::vector<unsigned> S;
  image2d<unsigned> parent;

  std::tie(K, parent, S) = morpho::ToS(f, c4);


  auto Kui8 = transform(K, [](const UInt<9>& v) -> uint8 {
      return v / 2;
    });

  image2d<float> energy = compute_energy(f, K, parent, S);
  image2d<float> cenergy = close(energy, K, parent, S);

  qt::MainWindow<uint8> w1(Kui8);
  w1.show();

  QAttribute<uint8> wattr1(Kui8, parent, QString("Gray values"));
  wattr1.show();

  QAttribute<float> wattr2(energy, parent, QString("Mumford shah"));
  wattr2.show();

  QAttribute<float> wattr3(cenergy, parent, QString("Mumford shah (clo)"));
  wattr3.show();

  image2d<float> energy = compute_energy(f, K, parent, S);
  image2d<float> cenergy = close(energy, K, parent, S);

  qt::MainWindow<uint8> w1(Kui8);
  w1.show();

  QAttribute<uint8> wattr1(Kui8, parent, QString("Gray values"));
  wattr1.show();

  QAttribute<float> wattr2(energy, parent, QString("Mumford shah"));
  wattr2.show();

  QAttribute<float> wattr3(cenergy, parent, QString("Mumford shah (clo)"));
  wattr3.show();


  QDispatcher disp(parent, S);
  disp.addImageWindow(&w1);
  disp.addAttribute(&wattr1);
  disp.addAttribute(&wattr2);
  disp.addAttribute(&wattr3);

  //  QObject::connect(&main, SIGNAL(pointSelected(const point2d&)),
  //&wattr, SLOT(plotNode(const point2d&)));

  qt::MainWindow<uint8> main(Kui8);
  main.show();

  QDispatcher disp(parent, S);
  disp.addImageWindow(&w1);
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
