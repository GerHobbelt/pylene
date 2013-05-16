#ifndef TOSAPP_HPP
# define TOSAPP_HPP

# include <mln/core/image/image2d.hpp>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>

namespace mln
{

  class QAttributeBase : public QwtPlot
  {
    Q_OBJECT;
  public:
    QAttributeBase();

  signals:
    void nodeSelected(const point2d& pt);

  public slots:
    virtual void plotNode(const point2d& pt) = 0;

  protected:
    virtual bool eventFilter(QObject* obj, QEvent *ev);

  protected:
    QwtPlotPicker*		picker;
    QwtPlotCurve*		m_curve;
    QVector<QPointF>		m_data;
  };


  template <typename V>
  class QAttribute : public QAttributeBase
  {
  public:
    QAttribute(const image2d<V>& attr,
	       const image2d<unsigned>& parent);

    virtual void plotNode(const point2d& pt);



  private:
    const image2d<V>&		m_attr;
    const image2d<unsigned>&	m_parent;

  };

  /**************************/
  /***  Implementation     **/
  /**************************/

  template <typename V>
  QAttribute<V>::QAttribute(const image2d<V>& attr,
			    const image2d<unsigned>& parent)
    : m_attr (attr),
      m_parent (parent)
  {
  }

  template <typename V>
  void
  QAttribute<V>::plotNode(const point2d& p)
  {
    unsigned x = m_parent.index_of_point(p);
    unsigned n = 1;

    unsigned q = x;
    while (q != m_parent[q]) {
      ++n;
      q = m_parent[q];
    }

    std::cout << "Add " << n << " nodes." << std::endl;

    this->m_data.clear();
    this->m_data.resize(n);

    while (x != m_parent[x]) {
      this->m_data[n-1].setX(n-1);
      this->m_data[n-1].setY(m_attr[x]);
      std::cout << n-1 << ":" << this->m_data[n-1].y() << std::endl;
      --n;
      x = m_parent[x];
    }
    this->m_data[0].setX(0);
    this->m_data[0].setY(m_attr[x]);
    this->m_curve->setSamples(m_data);
    this->replot();

    std::cout << this->picker->isActive() << std::endl;
    std::cout << this->picker->isEnabled() << std::endl;
  }

}
#endif // ! TOSAPP_HPP
