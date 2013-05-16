#include "tosapp.hpp"
#include <QEvent>
#include <QMouseEvent>
#include <qwt_picker_machine.h>

namespace mln
{


  QAttributeBase::QAttributeBase()
  {

    m_curve = new QwtPlotCurve("Energy");
    m_curve->setSamples(m_data);
    m_curve->setStyle(QwtPlotCurve::Lines);
    m_curve->attach(this);

    std::cout << this->canvas() << std::endl;

    picker = new QwtPlotPicker(this->canvas());
    picker->setStateMachine(new  QwtPickerTrackerMachine);
    picker->setTrackerMode(QwtPicker::AlwaysOn);
    picker->setRubberBand(QwtPicker::VLineRubberBand);
    picker->installEventFilter(this);
    picker->setEnabled(true);

    this->setFocus();

    std::cout << picker->isActive() << std::endl;
    std::cout << picker->isEnabled() << std::endl;
  }

  bool
  QAttributeBase::eventFilter(QObject* obj,  QEvent* event)
  {
    std::cout << "Event" << std::endl;
    if (obj == picker) {
      if (event->type() == QEvent::MouseButtonPress)
	{
	  QPoint p = static_cast<QMouseEvent*>(event)->pos();
	  std::cout << "Selected " << p.x() << "," << p.y() << std::endl;
	  return false;
	}
      return false;
    }
    return QwtPlot::eventFilter(obj, event);
  }


}
