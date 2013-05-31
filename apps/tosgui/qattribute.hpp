#ifndef QATTRIBUTE_HPP
# define QATTRIBUTE_HPP

# include <mln/core/image/image2d.hpp>

#include <QEvent>
#include <QMouseEvent>
#include <QSlider>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>

#include <mln/core/algorithm/accumulate.hpp>
#include <mln/accu/accumulators/min.hpp>
#include <mln/accu/accumulators/max.hpp>

namespace mln
{

  class QAttributeBase : public QwtPlot
  {
    Q_OBJECT;
  public:
    QAttributeBase(const QwtText& name = QString("Attribute") );

  signals:
    void nodeSelected(const point2d& pt);

  public slots:
    virtual void plotNode(const point2d& pt) = 0;

  protected:
    virtual void showFilteringWindow() = 0;


    QwtPlotPicker*		picker;
    QwtPlotCurve*		m_curve;
    QVector<QPointF>		m_data;
  };


  template <typename V>
  class QAttribute : public QAttributeBase
  {
  public:
    QAttribute(const image2d<V>& attr,
	       const image2d<unsigned>& parent,
	       const QwtText& name = QString("Attribute"));

    virtual void plotNode(const point2d& pt);

  protected:
    virtual bool eventFilter(QObject* obj, QEvent *ev);

  private:
    virtual void showFilteringWindow();


    const image2d<V>&		m_attr;
    const image2d<unsigned>&	m_parent;
    unsigned			m_current;
    unsigned			m_num_nodes;

    std::pair<V,V>		m_minmax;
    QSlider*			m_slider;
  };


  /**************************/
  /***  Implementation     **/
  /**************************/

  template <typename V>
  QAttribute<V>::QAttribute(const image2d<V>& attr,
			    const image2d<unsigned>& parent,
			    const QwtText& name)
    : QAttributeBase(name),
      m_attr (attr),
      m_parent (parent),
      m_current (-1)
  {
    auto acc = accumulate(m_attr, accu::features::min<> () & accu::features::max<> ());

    m_minmax.first = accu::extractor::min(acc);
    m_minmax.second = accu::extractor::max(acc);
  }

  template <typename V>
  void
  QAttribute<V>::plotNode(const point2d& p)
  {
    unsigned x = m_parent.index_of_point(p);
    m_current = x;
    m_num_nodes = 1;

    unsigned q = x;
    while (q != m_parent[q]) {
      ++m_num_nodes;
      q = m_parent[q];
    }

    std::cout << "Add " << m_num_nodes << " nodes." << std::endl;

    this->m_data.clear();
    this->m_data.resize(m_num_nodes);

    unsigned n = m_num_nodes;
    while (x != m_parent[x]) {
      this->m_data[n-1].setX(n-1);
      this->m_data[n-1].setY(m_attr[x]);
      //std::cout << n-1 << ":" << this->m_data[n-1].y() << std::endl;
      --n;
      x = m_parent[x];
    }
    this->m_data[0].setX(0);
    this->m_data[0].setY(m_attr[x]);
    this->m_curve->setSamples(m_data);
    this->replot();
  }

  template <typename V>
  void
  QAttribute<V>::showFilteringWindow()
  {
    m_slider = new QSlider();
    m_slider->setWindowTitle("Attribute filtering");
    m_slider->show();
  }

  template <typename V>
  bool
  QAttribute<V>::eventFilter(QObject* obj,  QEvent* event)
  {
    if (obj == this->canvas() and m_current != (unsigned)-1) {
      if (event->type() == QEvent::MouseButtonPress)
	{
	  QPoint p = static_cast<QMouseEvent*>(event)->pos();
	  unsigned i = std::max<int>(0, this->invTransform(QwtPlot::xBottom, p.x()));

	  unsigned x = m_current;
	  for (; i < m_num_nodes; ++i)
	    x = m_parent[x];

	  point2d q = m_parent.point_at_index(x);
	  emit nodeSelected(q);
	  std::cout << "Selected " << q << std::endl;
	  return false;
	}
      return false;
    }
    return QAttributeBase::eventFilter(obj, event);
  }

}
#endif // ! QATTRIBUTE_HPP
