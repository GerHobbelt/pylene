#ifndef MLN_IO_TYPEINFO_HPP
# define MLN_IO_TYPEINFO_HPP

# include <map>

namespace mln
{
  namespace io
  {
    struct type_info_base
    {
      virtual int id() const = 0;
      virtual const char* name() const = 0;
      bool operator == (const type_info_base& rhs)
      {
	return id() == rhs.id();
      }
    };

    template <typename T>
    struct type_info_ : type_info_base
    {
      typedef T type;
      virtual int id() const { return id_; }
      virtual const char* name() const { return name_; }
      static const int id_;
      static const char* const name_;
    };

    template <typename T>
    struct type_info : type_info_<T>
    {
      typedef type_info_<T> super;
      using super::id_;
      using super::name_;
    };


    template <typename T>
    const int type_info_<T>::id_ = 0;

    template <typename T>
    const char* const type_info_<T>::name_ = "Unknown";

  }

}

#endif
