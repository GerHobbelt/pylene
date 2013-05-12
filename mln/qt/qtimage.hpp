#ifndef MLN_QT_QTIMAGE_HPP
# define MLN_QT_QTIMAGE_HPP

# include <QImage>
# include <QColor>
# include <mln/core/image/image2d.hpp>
# include <mln/core/grays.hpp>
# include <mln/core/colors.hpp>

namespace mln
{
  namespace qt
  {

    class QtImageBase : public QImage
    {
      //Q_OBJECT;

    public:
      QtImageBase(const uchar * data, int width, int height,
                  int bytesPerLine, Format format);


    protected:
      static QVector<QRgb> default_lut8;
    };


    template <typename V>
    class QtImage : public QtImageBase
    {
    public:
      QtImage(const image2d<V>& ima);

    private:
      const image2d<V>& m_ima;
    };


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace internal
    {
      // FIXME overload or specialization ?

      template <typename V>
      struct qt_format
      {
        static const QImage::Format format = QImage::Format_Invalid;
      };

      template <>
      struct qt_format<uint8>
      {
        static const QImage::Format format = QImage::Format_Indexed8;
      };

      template <>
      struct qt_format<rgb8>
      {
        static const QImage::Format format = QImage::Format_RGB888;
      };

    }


    template <typename V>
    QtImage<V>::QtImage(const image2d<V>& ima)
      : QtImageBase((uchar*)&ima(ima.domain().pmin),
                    ima.ncols(),
                    ima.nrows(),
                    ima.strides()[0],
                    internal::qt_format<V>::format),
        m_ima(ima)
    {
      mln_precondition(ima.strides()[1] == sizeof(V));

      if (internal::qt_format<V>::format == QImage::Format_Indexed8)
        {
          if (this->default_lut8.empty()) {
            this->default_lut8.resize(256);
            for (int i = 0; i < 256; ++i)
              this->default_lut8[i] = QColor(i,i,i).rgb();
          }
          this->setColorTable(this->default_lut8);
        }
    }

  } // end of namespace mln::qt

} // end of namespace mln

#endif //!MLN_QT_QTIMAGE_HPP
