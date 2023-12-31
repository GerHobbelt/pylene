#pragma once

#include <apps/tosgui/attribute.hpp>
#include <apps/tosgui/qattribute.hpp>

#include <mln/core/image/image2d.hpp>


template <typename V>
class AreaAttribute : public Attribute
{
public:
  AreaAttribute(const mln::image2d<V>& K_, const mln::image2d<unsigned>& parent_, const std::vector<unsigned>& S_)
    : K(K_)
    , parent(parent_)
    , S(S_)
    , m_attribute(NULL)
  {
  }

  virtual ~AreaAttribute()
  {
    if (m_attribute)
      delete m_attribute;
  }

  virtual std::vector<QString> names() const
  {
    std::vector<QString> nm;
    nm.push_back("Area");
    return nm;
  }

  virtual QMap<QString, Parameter>& parameters()
  {
    static QMap<QString, Parameter> params;
    return params;
  }

  virtual void run()
  {
    if (m_attribute == NULL)
    {
      mln::resize(m_area, K).init(0);
      for (int i = S.size() - 1; i > 0; --i)
      {
        unsigned x = S[i];
        ++m_area[x];
        m_area[parent[x]] += m_area[x];
      }
      m_attribute = new mln::QAttribute<unsigned>(m_area, parent, QString("Grain size"));
      this->setSignals(m_attribute);
    }
  }

  virtual mln::QAttributeBase* getPlot(const QString&) { return m_attribute; }

private:
  const mln::image2d<V>&        K;
  const mln::image2d<unsigned>& parent;
  const std::vector<unsigned>&  S;

  mln::image2d<unsigned>     m_area;
  mln::QAttribute<unsigned>* m_attribute;
};
