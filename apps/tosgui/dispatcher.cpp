#include "dispatcher.hpp"
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/sub_image.hpp>

namespace mln
{

  QDispatcher::QDispatcher(const image2d<unsigned>& parent,
			   const std::vector<unsigned>& S)
    : QObject(),
      m_parent(parent),
      m_S (S)
  {
    resize(m_mask_selection, parent);
  }

  void
  QDispatcher::addImageWindow(qt::MainWindowBase* win)
  {
    m_windows.push_back(win);
    QObject::connect(win, SIGNAL(pointSelected(const point2d&)),
		     this, SLOT(onPointSelected(const point2d&)));
  }

  void
    QDispatcher::addImageWindowToFilter(qt::MainWindowBase* win,
                                        const image2d<rgb8>& mean)
  {
    m_fwins.push_back(std::make_pair(win, mean));
    QObject::connect(win, SIGNAL(pointSelected(const point2d&)),
		     this, SLOT(onPointSelected(const point2d&)));
  }

  void
  QDispatcher::addAttribute(QAttributeBase* attr)
  {
    m_attributes.push_back(attr);

    QObject::connect(attr, SIGNAL(nodeSelected(const point2d&)),
		     this, SLOT(onNodeSelected(const point2d&)));
  }



  void
  QDispatcher::onNodeSelected(const point2d& p)
  {
    unsigned x = m_parent.index_of_point(p);

    fill(m_mask_selection, false);
    for (unsigned p: m_S)
      if (p == x or m_mask_selection[m_parent[p]])
	m_mask_selection[p] = true;

    for (qt::MainWindowBase* win : m_windows)
      this->doNodeSection(win);

    for (auto& win : m_fwins)
      this->doFiltering(win);
  }

  void
  QDispatcher::onPointSelected(const point2d& p)
  {
    // Set true on leaves in desc(p)
    unsigned x = m_parent.index_of_point(p);
    fill(m_mask_selection, false);
    for (unsigned q: m_S)
      if (q == x or m_mask_selection[m_parent[q]]) {
	m_mask_selection[m_parent[q]] = false;
	m_mask_selection[q] = true;
      }

    // Select leaf
    auto dist = [] (const point2d& p, const point2d& q) {
      return abs(p[0] - q[0]) + abs(p[1] - q[1]);
    };
    double dmin = value_traits<double>::max();
    point2d pmin;
    mln_foreach(const auto& px, m_mask_selection.pixels())
      {
	if (px.val() and dist(px.point(), p) < dmin)
	  {
	    pmin = px.point();
	    dmin = dist(pmin, p);
	  }
      }

    // Transmit selection
    for (QAttributeBase* attr : m_attributes)
      attr->plotNode(pmin);
  }

  void
  QDispatcher::doNodeSection(qt::MainWindowBase* win)
  {
    image2d<rgb8>& view = win->getView();
    win->reset();

    auto x = view | where(m_mask_selection);
    mln_foreach(auto& v, x.values()) {
      v[0] = v[0] / 2 + 128;
    }

    win->update();
  }

  void
  QDispatcher::doFiltering(std::pair<qt::MainWindowBase*, image2d<rgb8> >& obj)
  {
    qt::MainWindowBase* win = obj.first;
    image2d<rgb8>& view = obj.first->getView();
    image2d<rgb8>& mean = obj.second;
    win->reset();

    for (unsigned x: m_S)
      if (m_mask_selection[m_parent[x]]) {
        view[x] = view[m_parent[x]];
      } else if (m_mask_selection[x]) {
        view[x] = mean[x];
      }

    win->update();
  }

}
