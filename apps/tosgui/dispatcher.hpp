#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

# include <utility>
# include <vector>
# include "qattribute.hpp"
# include <mln/qt/mainwin.hpp>

namespace mln
{

  class QDispatcher : public QObject
  {
    Q_OBJECT;

  public:
    QDispatcher(const image2d<unsigned>& parent,
		const std::vector<unsigned>& S);

    void addImageWindow(qt::MainWindowBase* win);
    void addImageWindowToFilter(qt::MainWindowBase* win, const image2d<rgb8>& mean);
    void addAttribute(QAttributeBase* attr);

  protected slots:
    void onNodeSelected(const point2d& p);
    void onPointSelected(const point2d& p);

  private:
    void doNodeSection(qt::MainWindowBase*);
    void doFiltering(std::pair<qt::MainWindowBase*, image2d<rgb8> >& obj);

    const image2d<unsigned>&	 m_parent;
    const std::vector<unsigned>& m_S;
    bool			 m_leaf_attach;

    image2d<bool>		 m_mask_selection;
    std::vector<QAttributeBase*> m_attributes;
    std::vector<qt::MainWindowBase*> m_windows;
    std::vector< std::pair<qt::MainWindowBase*, image2d<rgb8> > > m_fwins;
  };

}

#endif // ! DISPATCHER_HPP
