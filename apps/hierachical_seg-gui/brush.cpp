#include "brush.hpp"

#include <mln/io/imsave.hpp>

#include <QtGui>

#include <iostream>


inline void brush(QPixmap* pixmap, const QPointF& position, QColor color, int r)
{
  QPainter painter(pixmap);
  painter.setBrush(color);
  painter.setPen(Qt::NoPen);

  QRectF rectangle(position.x() - r, position.y() - r, 2 * r, 2 * r);
  painter.drawEllipse(rectangle);
}

MyBrush::MyBrush(mln::qt::ImageViewer*                                                         viewer,
                 std::function<void(const mln::image2d<mln::rgb8>&, mln::image2d<mln::rgb8>&)> callback)
  : m_viewer(viewer)
  , m_callback(callback)
  , m_scene(viewer->getScene())
  , m_pixmap(viewer->getPixmap())
  , m_ima(m_pixmap->pixmap())
  , m_active(false)
  , m_radius(5)
{
  m_ori = mln::clone(viewer->getView());
}

bool MyBrush::eventFilter(QObject* obj, QEvent* ev)
{
  (void)obj;
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

void MyBrush::setColor1()
{
  m_active = true;
  m_color  = Qt::red;
}

void MyBrush::setColor2()
{
  m_active = true;
  m_color  = Qt::blue;
}

void MyBrush::setRadius(int k)
{
  m_radius = k;
}

void MyBrush::run()
{
  std::cout << "Running." << std::endl;
  m_viewer->notify();

  mln::image2d<mln::rgb8>& view = m_viewer->getView();
  m_callback(view, view);
  m_viewer->update();
}

void MyBrush::revert()
{
  m_pixmap->setPixmap(m_ima);
}

void MyBrush::reload()
{
  m_viewer->getView() = mln::clone(m_ori);
  m_viewer->update();
  m_ima = m_pixmap->pixmap(); // FLUSH THE MARKERS
}
