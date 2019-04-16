#include <py/core/image_format.hpp>


namespace mln
{


  dyn_sample_type_id_traits::dyn_sample_type_id_traits(sample_type_id x)
    : m_id {x}
  {
    switch (m_id)
    {
    case sample_type_id::UINT8:
      m_size = sample_type_id_traits<sample_type_id::UINT8>::size();
      break;
    case sample_type_id::UINT16:
      m_size = sample_type_id_traits<sample_type_id::UINT16>::size();
      break;
    case sample_type_id::UINT32:
      m_size = sample_type_id_traits<sample_type_id::UINT32>::size();
      break;
    case sample_type_id::UINT64:
      m_size = sample_type_id_traits<sample_type_id::UINT64>::size();
      break;
    case sample_type_id::INT8:
      m_size = sample_type_id_traits<sample_type_id::INT8>::size();
      break;
    case sample_type_id::INT16:
      m_size = sample_type_id_traits<sample_type_id::INT16>::size();
      break;
    case sample_type_id::INT32:
      m_size = sample_type_id_traits<sample_type_id::INT32>::size();
      break;
    case sample_type_id::INT64:
      m_size = sample_type_id_traits<sample_type_id::INT64>::size();
      break;
    case sample_type_id::FLOAT:
      m_size = sample_type_id_traits<sample_type_id::FLOAT>::size();
      break;
    case sample_type_id::DOUBLE:
      m_size = sample_type_id_traits<sample_type_id::DOUBLE>::size();
      break;
    case sample_type_id::OTHER:
      m_size = 0;
      break;
    }
  }


  dyn_sample_type_id_traits get_sample_type_id_traits(sample_type_id st)
  {
    return {st};
  }

} // namespace mln
