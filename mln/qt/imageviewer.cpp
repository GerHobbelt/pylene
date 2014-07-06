#include <mln/qt/imageviewer.hpp>
#include <iostream>
#include <QtGui>

namespace mln
{

  namespace qt
  {


    void
    ImageViewer::_init(QtImageBase* ima)
    {
      m_ima = ima;
      m_pixmap.setPixmap(QPixmap::fromImage(m_ima->getQImage()));

      m_scene = new QGraphicsScene(this);
      QObject::connect(m_scene, SIGNAL(sceneRectChanged(const QRectF&)),
		       this, SLOT(onzoom(const QRectF&)));

      m_scene->addItem(&m_pixmap);
      m_scene->setFocus();
      m_scene->setFocusItem(&m_pixmap);

      this->setScene(m_scene);
      this->setMaximumSize(1000,1000);
      m_scene->update();
      m_scene->installEventFilter(this);
    }


    ImageViewer::~ImageViewer()
    {
      delete m_ima;
    }

    void
    ImageViewer::onzoom(const QRectF& rect)
    {
      int w = rect.right();
      int h = rect.bottom();

      this->resize(w+10, h+10);
    }

    void
    ImageViewer::reset()
    {
      m_ima->reset();
    }

    image2d<rgb8>&
    ImageViewer::getView()
    {
      return m_ima->getView();
    }

    const image2d<rgb8>&
    ImageViewer::getView() const
    {
      return m_ima->getView();
    }

    QGraphicsScene*
    ImageViewer::getScene()
    {
      return m_scene;
    }

    const QGraphicsScene*
    ImageViewer::getScene() const
    {
      return m_scene;
    }

    QGraphicsPixmapItem*
    ImageViewer::getPixmap()
    {
      return &m_pixmap;
    }

    const QGraphicsPixmapItem*
    ImageViewer::getPixmap() const
    {
      return &m_pixmap;
    }


    void
    ImageViewer::update()
    {
      std::cout << "Image updated." << std::endl;
      m_pixmap.setPixmap(QPixmap::fromImage(m_ima->getQImage()));
    }

    void
    ImageViewer::notify()
    {
      QPixmap pxmap = m_pixmap.pixmap();
      QPainter painter(&m_ima->getQImage());
      painter.drawPixmap(pxmap.rect(), pxmap);
    }

    void
    ImageViewer::_zoom_by(float factor)
    {
      m_pixmap.setScale(m_pixmap.scale() * factor);
      m_scene->setSceneRect(m_pixmap.sceneBoundingRect());
    }

    void
    ImageViewer::_zoom_at(float factor)
    {
      m_pixmap.setScale(factor);
      m_scene->setSceneRect(m_pixmap.sceneBoundingRect());
    }

    bool
    ImageViewer::eventFilter(QObject* obj, QEvent* ev)
    {
      if (obj == m_scene)
	{
          if (ev->type() == QEvent::KeyPress)
            {
              QKeyEvent* event = static_cast<QKeyEvent*>(ev);
              switch (event->key())
                {
                case '<': _zoom_by(0.5); return true;
                case '>': _zoom_by(2.0); return true;
                }
            }
	  else if (ev->type() == QEvent::GraphicsSceneMouseMove)
	    {
	      QGraphicsSceneMouseEvent* event = static_cast<QGraphicsSceneMouseEvent*>(ev);
	      QPointF p_ = m_pixmap.mapFromScene(event->scenePos());
	      point2d p = { (short)p_.y(), (short)p_.x() };
	      if (m_ima->getView().domain().has(p)) {
		//std::cout << p << std::endl;
		emit pointHover(p);
		return false;
	      }
	    }
	  else if (ev->type() == QEvent::GraphicsSceneMousePress)
	    {
	      QGraphicsSceneMouseEvent* event = static_cast<QGraphicsSceneMouseEvent*>(ev);
              if (event->button() == Qt::RightButton)
                m_selection_pstart = event->scenePos();

              QPointF p__ = event->scenePos();
	      QPointF p_ = m_pixmap.mapFromScene(event->scenePos());
	      point2d p = { (short)p_.y(), (short)p_.x() };
	      if (m_ima->getView().domain().has(p)) {
		std::cout << "Press: " << p << "," << (point2df{p__.x(), p__.y()})
                          << std::endl;
		emit pointSelected(p);
		return false;
	      }
	    }
          else if (ev->type() == QEvent::GraphicsSceneMouseRelease)
            {
              QGraphicsSceneMouseEvent* event = static_cast<QGraphicsSceneMouseEvent*>(ev);

              // Zoom on the selected area
              if (event->button() == Qt::RightButton)
                {
                  QPointF p1 = m_selection_pstart;
                  QPointF p2 = event->scenePos();
                  int w = this->width();
                  int h = this->height();
                  qreal x1, x2, y1, y2;
                  std::tie(x1,x2) = std::minmax(p1.x(), p2.x());
                  std::tie(y1,y2) = std::minmax(p1.y(), p2.y());
                  QPoint d = this->mapFromScene(x2-x1, y2-y1);
                  float wscale = w / d.x();
                  float yscale = h / d.y();
                  float ratio = float(d.y()) / d.x();
                  this->_zoom_by(yscale);
                  this->resize(w, w*ratio);
                  this->centerOn(0.5 * yscale * (x1+x2), 0.5 * yscale * (y1+y2));
                }
            }
          else if (ev->type() == QEvent::GraphicsSceneWheel)
            {
              QGraphicsSceneWheelEvent* event = static_cast<QGraphicsSceneWheelEvent*>(ev);
              int d = event->delta();
              double scale = m_pixmap.scale() + (d/120) * 0.1;
              scale = std::max(std::min(scale, 20.0), 0.05);
              this->_zoom_at(scale);
              return true; // no more processing
            }

	}
      return QGraphicsView::eventFilter(obj, ev);
    }

  }

}
