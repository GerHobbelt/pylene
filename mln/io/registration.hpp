#ifndef MLN_IO_REGISTRATION_HPP
# define MLN_IO_REGISTRATION_HPP

# include <mln/io/typeinfo.hpp>
# include <mln/core/grays.hpp>

/**
   \file \brief Registration procedure that offers static and dynamic type
   mapping during io processes.
 */

# define ADD_TYPE_REGISTRATION(TYPE, ID, NAME)			\
  namespace mln { namespace io {				\
  template <>							\
  struct type_info<TYPE> : type_info_<TYPE>			\
  {								\
    typedef type_info_<TYPE> super;				\
    static bool init_;						\
    type_info() {						\
      if (!init_){						\
	init_ = true;						\
	registration_map_[ID] = this;				\
      }								\
    }								\
  };								\
								\
  bool type_info<TYPE>::init_ = false;				\
  template <> const int type_info_<TYPE>::id_ = ID;		\
  template <> const char* const type_info_<TYPE>::name_ = NAME;	\
  const type_info<TYPE> type_info_##ID;				\
    } }


namespace mln
{
  enum {
    MLN_UNKNOWN = 0
  };


  namespace io
  {
    static std::map<int, const type_info_base*> registration_map_;
  }
}

ADD_TYPE_REGISTRATION(void,		MLN_UNKNOWN,"unknown")
ADD_TYPE_REGISTRATION(bool,		MLN_BOOL,   "bit")
ADD_TYPE_REGISTRATION(mln::uint8,  	MLN_UINT8,  "uint8")
ADD_TYPE_REGISTRATION(mln::uint16, 	MLN_UINT16, "uint16")
ADD_TYPE_REGISTRATION(mln::uint32, 	MLN_UINT32, "uint32")
ADD_TYPE_REGISTRATION(mln::int8,   	MLN_INT8,   "int8")
ADD_TYPE_REGISTRATION(mln::int16,  	MLN_INT16,  "int16")
ADD_TYPE_REGISTRATION(mln::int32,  	MLN_INT32,  "int32")
ADD_TYPE_REGISTRATION(float,            MLN_FLOAT,  "float")
ADD_TYPE_REGISTRATION(double,           MLN_DOUBLE,  "double")

# ifndef BOOST_NO_INT64_T
ADD_TYPE_REGISTRATION(mln::uint64, 	MLN_UINT64, "uint64")
ADD_TYPE_REGISTRATION(mln::int64,  	MLN_INT64,  "int64")
# endif


#endif
