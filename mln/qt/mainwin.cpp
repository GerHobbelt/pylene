#include <mln/qt/mainwin.hpp>
#include <QtGui>
#include <iostream>
#include <QSize>

namespace mln
{

  namespace qt
  {

    MainWindowBase::MainWindowBase(QtImageBase* ima)
      : m_ima(ima)
    {
      m_pixmap.setPixmap(QPixmap::fromImage(*m_ima));
      m_scene = new QGraphicsScene;
      m_scene->addItem(&m_pixmap);
      m_view = new QGraphicsView(m_scene);
      this->setCentralWidget(m_view);
    }


    MainWindowBase::~MainWindowBase()
    {
      delete m_ima;
    }

  }

}
