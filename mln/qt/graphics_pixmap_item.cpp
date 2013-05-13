#include <mln/qt/graphics_pixmap_item.hpp>
#include <QtGui>
#include <iostream>
namespace mln
{

  namespace qt
  {

    graphics_pixmap_item::graphics_pixmap_item()
      : QGraphicsPixmapItem(),
	m_scale(1.0)
    {
      this->setFlag(QGraphicsItem::ItemIsFocusable);
      this->setAcceptHoverEvents(true);
    }

    graphics_pixmap_item::graphics_pixmap_item(const QPixmap & pixmap, QGraphicsItem * parent)
      : QGraphicsPixmapItem(pixmap, parent),
	m_scale(1.0)
    {
      this->setFlag(QGraphicsItem::ItemIsFocusable);
      this->setAcceptHoverEvents(true);
    }


    void
    graphics_pixmap_item::keyPressEvent(QKeyEvent* event)
    {
      switch (event->key())
	{
	  case '<':
	    m_scale *= 0.5; this->setScale(m_scale); break;
	  case '>':
	    m_scale *= 2.0; this->setScale(m_scale); break;
	  default:
	    return;
	}
      if (this->scene()) {
	QRectF r = this->sceneBoundingRect();
	//std::cout << r.top() << ":" << r.bottom() << std::endl;
	this->scene()->setSceneRect(r);
      }
    }


    void
    graphics_pixmap_item::wheelEvent(QGraphicsSceneWheelEvent * event)
    {
      int d = event->delta();
      m_scale += (d/120) * 0.1;
      m_scale = std::max(std::min(m_scale, 20.0), 0.05);
      this->setScale(m_scale);
      if (this->scene())
	this->scene()->setSceneRect(this->sceneBoundingRect());
    }

  }

}
