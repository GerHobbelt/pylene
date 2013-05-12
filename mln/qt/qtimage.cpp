#include <mln/qt/qtimage.hpp>


namespace mln
{

  namespace qt
  {

    QtImageBase::QtImageBase(const uchar * data, int width, int height,
                             int bytesPerLine, Format format)
      : QImage(data, width, height, bytesPerLine, format)
    {
    }


    QVector<QRgb> QtImageBase::default_lut8;
  }

}
