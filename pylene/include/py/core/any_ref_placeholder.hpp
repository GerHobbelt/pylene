#pragma once

#include <any>
#include <typeinfo>

namespace mln::py
{
  template <typename T>
  class placeholder;

  struct placeholder_base
  {
    virtual ~placeholder_base();
    virtual const std::type_info& type() const;
    void *m_data;
  };

  template <>
  class placeholder<std::any> : public placeholder_base
  {
  public:
    placeholder(std::any& elm)
    {
      m_data = &elm;
    }
    const std::type_info& type() const override
    {
      return static_cast<std::any*>(m_data)->type();
    }
  };

  template <typename T>
  class placeholder : public placeholder_base
  {
  public:
    placeholder(T& elm)
    {
      m_data = &elm;
    }
    const std::type_info& type() const override
    {
      return typeid(T);
    }
  };
} // namespace mln::py