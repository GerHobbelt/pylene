
#include <QApplication>

#include <mln/qt/imageviewer.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <QtGui>
#include "brush.hpp"


int main(int argc, char** argv)
{
  using namespace mln;

  QApplication a(argc, argv);

  image2d<rgb8> ima;
  io::imread(argv[1], ima);


  QMainWindow win;
  qt::ImageViewer* viewer = new qt::ImageViewer(ima);

  QGraphicsScene* scene = viewer->getScene();
  MyBrush brush(viewer);
  scene->installEventFilter(&brush);

  QToolBar* toolbar = new QToolBar(&win);
  QAction* action1 = toolbar->addAction("M1");
  QAction* action2 = toolbar->addAction("M2");
  QAction* action3 = toolbar->addAction("Rum");
  QObject::connect(action1, SIGNAL(triggered()),
                   &brush, SLOT(setColor1()));
  QObject::connect(action2, SIGNAL(triggered()),
                   &brush, SLOT(setColor2()));
  QObject::connect(action3, SIGNAL(triggered()),
                   &brush, SLOT(run()));


  QActionGroup agroup(&win);
  agroup.addAction(action1);
  agroup.addAction(action2);
  action1->setCheckable(true);
  action2->setCheckable(true);


  win.setCentralWidget(viewer);
  win.addToolBar(toolbar);
  win.show();

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
