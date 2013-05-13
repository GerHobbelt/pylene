#ifndef MLN_QT_MAINWIN_HPP
# define MLN_QT_MAINWIN_HPP

# include <QMainWindow>
# include <QGraphicsScene>
# include <QGraphicsView>
# include <mln/qt/graphics_pixmap_item.hpp>
# include <mln/qt/qtimage.hpp>

namespace mln
{
  namespace qt
  {

    class MainWindowBase : public QMainWindow
    {
      Q_OBJECT;

    public:
      MainWindowBase(QtImageBase* image);
      virtual ~MainWindowBase();

    private:
      graphics_pixmap_item      m_pixmap;
      QGraphicsScene*           m_scene;
      QGraphicsView*            m_view;
      QtImageBase* m_ima;

    protected:
      virtual bool eventFilter(QObject* obj, QEvent* event);

    signals:
      void pointSelected(const point2d& pt);
      void pointHover(const point2d& pt);

    public slots:
      void update();

    protected slots:
      void onzoom(const QRectF& rect);
    };


    template <typename V>
    class MainWindow : public MainWindowBase
    {
    public:
      MainWindow(const image2d<V>& ima);
    };

    /******************************************/
    /****          Implementation          ****/
    /******************************************/


    template <typename V>
    MainWindow<V>::MainWindow(const image2d<V>& ima)
      : MainWindowBase(new QtImage<V>(ima))
    {
    }


  } // end of namespace mln::qt

} // end of namespace mln

#endif //!MLN_QT_MAINWIN_HPP
