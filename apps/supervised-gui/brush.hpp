#ifndef BRUSH_HPP
# define BRUSH_HPP

# include <mln/qt/imageviewer.hpp>

class MyBrush : public QObject
{
  Q_OBJECT;
public:
  MyBrush(mln::qt::ImageViewer* viewer);
  bool eventFilter(QObject* obj, QEvent* ev);

public slots:
  void setColor1();
  void setColor2();
  void setRadius(int k);
  void run();


private:
  mln::qt::ImageViewer*  m_viewer;
  QGraphicsScene*        m_scene;
  QGraphicsPixmapItem*   m_pixmap;
  QPixmap                m_ima;
  bool                   m_active;
  QColor                 m_color;
  int                    m_radius;
};

#endif //!BRUSH_HPP
