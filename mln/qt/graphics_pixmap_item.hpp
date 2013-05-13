#ifndef GRAPHICS_PIXMAP_ITEM_HPP
# define GRAPHICS_PIXMAP_ITEM_HPP

# include <QGraphicsPixmapItem>

namespace mln
{

  namespace qt
  {


    class graphics_pixmap_item : public QGraphicsPixmapItem
    {
    public:
      graphics_pixmap_item();
      graphics_pixmap_item(const QPixmap& pixmap, QGraphicsItem * parent = 0);


    protected:
      virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
      virtual void keyPressEvent(QKeyEvent* event );

      qreal m_scale;
    };

  }

}



#endif // ! GRAPHICS_PIXMAP_ITEM_HPP
