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
      QObject::connect(m_scene, SIGNAL(sceneRectChanged(const QRectF&)),
		       this, SLOT(onzoom(const QRectF&)));

      m_scene->addItem(&m_pixmap);
      m_view = new QGraphicsView(m_scene);
      this->setCentralWidget(m_view);
      m_view->setFocus();
      m_scene->setFocus();
      m_scene->setFocusItem(&m_pixmap);

      this->setMaximumSize(1000,1000);
      m_scene->update();
      m_scene->installEventFilter(this);
    }


    MainWindowBase::~MainWindowBase()
    {
      delete m_ima;
    }

    void
    MainWindowBase::onzoom(const QRectF& rect)
    {
      int w = rect.right();
      int h = rect.bottom();

      this->resize(w+10, h+10);
    }


    void
    MainWindowBase::update()
    {
      std::cout << "Image updated." << std::endl;
      m_pixmap.setPixmap(QPixmap::fromImage(*m_ima));
    }

    bool
    MainWindowBase::eventFilter(QObject* obj, QEvent* ev)
    {
      if (obj == m_scene)
	{
	  if (ev->type() == QEvent::GraphicsSceneMouseMove)
	    {
	      QGraphicsSceneMouseEvent* event = static_cast<QGraphicsSceneMouseEvent*>(ev);
	      QPointF p_ = m_pixmap.mapFromScene(event->scenePos());
	      point2d p = { p_.y(), p_.x() };
	      if (m_ima->getView().domain().has(p)) {
		std::cout << p << std::endl;
		emit pointHover(p);
		return true;
	      }
	    }
	  else if (ev->type() == QEvent::GraphicsSceneMousePress)
	    {
	      QGraphicsSceneMouseEvent* event = static_cast<QGraphicsSceneMouseEvent*>(ev);
	      QPointF p_ = m_pixmap.mapFromScene(event->scenePos());
	      point2d p = { p_.y(), p_.x() };
	      if (m_ima->getView().domain().has(p)) {
		std::cout << p << std::endl;
		emit pointSelected(p);
		return true;
	      }
	    }
	}
      return QMainWindow::eventFilter(obj, ev);
    }

  }

}
