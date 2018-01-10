#ifndef TRACE_HPP
#define TRACE_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // MSVC unsafe getenv
#endif

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#define mln_concat(A,B) A ## B
#define mln_entering_expand(NAME,COUNTER) mln::trace::scoped_trace mln_concat(__mln_trace, COUNTER) (NAME);
#define mln_entering(NAME) mln_entering_expand(NAME, __COUNTER__)

#define mln_exiting() ;

#define mln_scoped_entering(NAME) mln_entering_expand(NAME, __COUNTER__)

namespace mln
{

  namespace trace
  {

    static bool verbose = (std::getenv("TRACE") != NULL);
    static thread_local int __stack_depth = 0;

    struct scoped_trace
    {
      explicit scoped_trace(const char* desc);
      explicit scoped_trace(const std::string& desc);
      scoped_trace(const scoped_trace&) = delete;
      scoped_trace& operator=(const scoped_trace&) = delete;
      ~scoped_trace();

    private:
      template <class T = void> void entering(const char* desc) __mln__attribute__((noinline));
      template <class T = void> void exiting() __mln__attribute__((noinline));

      int m_depth;
      std::string m_desc;
      std::chrono::time_point<std::chrono::system_clock> m_clock;
    };


    inline scoped_trace::scoped_trace(const char* desc)
    {
      if (verbose)
        entering(desc);
    }


    inline scoped_trace::scoped_trace(const std::string& desc)
    {
      if (verbose)
        entering(desc.c_str());
    }


    inline scoped_trace::~scoped_trace()
    {
      if (verbose)
        exiting();
    }


    template <class T>
    void scoped_trace::entering(const char* desc)
    {
      m_depth = __stack_depth++;
      m_desc = desc;
      m_clock = std::chrono::system_clock::now();

      for (int k = 0; k < m_depth; ++k)
        std::clog.put(' ');
      std::clog << "#" << std::this_thread::get_id() << " - " << m_desc << std::endl;
    }

    template <class T>
    void scoped_trace::exiting()
    {
      --__stack_depth;
      for (int k = 0; k < m_depth; ++k)
        std::clog.put(' ');
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_clock);
      std::clog << "#" << std::this_thread::get_id() << " - " << m_desc << " in " << duration.count() << "ms"
                << std::endl;
    }

    namespace impl
    {
      template <class T = void> void warn(const char* msg) __attribute__((noinline));

      template <class T>
      void warn(const char* msg)
      {
        for (int k = 0; k < __stack_depth; ++k)
          std::clog.put(' ');

        std::clog << "#" << std::this_thread::get_id() << " - " << msg << std::endl;
      }
    }


    inline void warn(const char* msg)
    {
      if (verbose)
        impl::warn(msg);
    }

    inline void warn(const std::string& msg)
    {
      if (verbose)
        impl::warn(msg.c_str());
    }

  }
}

#ifdef _MSVC_VER
#pragma warning(pop)
#endif

#endif // ! TRACE_HPP
