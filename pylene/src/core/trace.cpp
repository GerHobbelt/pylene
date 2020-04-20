#include <mln/core/trace.hpp>
#include <mln/core/assert.hpp>

#include <chrono>
#include <cstdlib> //getenv
#include <iostream>
#include <memory>
#include <string>
#include <thread>


#ifdef _MSC_VER
#pragma warning(disable : 4996) // MSVC unsafe getenv
#endif


namespace mln::trace
{
  bool             verbose       = (std::getenv("TRACE") != NULL);
  thread_local int __stack_depth = 0;


  struct scoped_trace::impl_t
  {
    void entering(std::string_view view);
    void exiting();

    int                                                m_depth;
    std::string                                        m_desc;
    std::chrono::time_point<std::chrono::system_clock> m_clock;
  };



  void scoped_trace::impl_t::entering(std::string_view desc)
  {
    m_depth = __stack_depth++;
    m_desc  = desc;
    m_clock = std::chrono::system_clock::now();

    for (int k = 0; k < m_depth; ++k)
      std::clog.put(' ');
    std::clog << "#" << std::this_thread::get_id() << " - " << m_desc << std::endl;
  }


  void scoped_trace::impl_t::exiting()
  {
    --__stack_depth;
    for (int k = 0; k < m_depth; ++k)
      std::clog.put(' ');
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_clock);
    std::clog << "#" << std::this_thread::get_id() << " - " << m_desc << " in " << duration.count() << "ms"
              << std::endl;
  }

  void scoped_trace::entering(std::string_view desc)
  {
    mln_precondition(m_impl == nullptr);
    m_impl = new impl_t();
    m_impl->entering(desc);
  }

  void scoped_trace::exiting()
  {
    mln_precondition(m_impl != nullptr);
    m_impl->exiting();
    delete m_impl;
  }

  namespace impl
  {
    void warn(std::string_view msg)
    {
      for (int k = 0; k < __stack_depth; ++k)
        std::clog.put(' ');

      std::clog << "#" << std::this_thread::get_id() << " - " << msg << std::endl;
    }
  } // namespace impl

} // namespace mln::trace
