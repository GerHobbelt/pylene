#include "brush.hpp"
#include <QtGui>
#include <iostream>
#include <mln/io/imsave.hpp>

inline
void brush(QPixmap* pixmap, const QPointF& position, QColor color, int r)
{
  QPainter painter(pixmap);
  painter.setBrush(color);
  painter.setPen(Qt::NoPen);

  QRectF rectangle(position.x() - r, position.y() - r, 2*r, 2*r);
  painter.drawEllipse(rectangle);
}



MyBrush::MyBrush(mln::qt::ImageViewer* viewer)
    : m_viewer (viewer),
      m_scene(viewer->getScene()),
      m_pixmap(viewer->getPixmap()),
      m_ima(m_pixmap->pixmap()),
      m_active(false),
      m_radius(5)
{
}

bool
MyBrush::eventFilter(QObject* obj, QEvent* ev)
{
  if (m_active and ev->type() == QEvent::GraphicsSceneMouseMove)
    {
      QGraphicsSceneMouseEvent* event = static_cast<QGraphicsSceneMouseEvent*>(ev);
      if (event->buttons() & Qt::LeftButton)
        {
          brush(&m_ima, m_pixmap->mapFromScene(event->scenePos()), m_color, m_radius);
          m_pixmap->setPixmap(m_ima);
        }
    }
  return false;
}

void
MyBrush::setColor1()
{
  m_active = true;
  m_color = Qt::red;
}

void
MyBrush::setColor2()
{
  m_active = true;
  m_color = Qt::blue;
}


void
MyBrush::setRadius(int k)
{
  m_radius = k;
}


void
MyBrush::run()
{
  std::cout << "Running." << std::endl;
  m_viewer->notify();
  mln::io::imsave(m_viewer->getView(), "/tmp/view.tiff");
}
