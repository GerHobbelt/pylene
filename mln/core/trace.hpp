#ifndef TRACE_HPP
# define TRACE_HPP

# ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 4996 )  // MSVC unsafe getenv
# endif


# include <ctime>
# include <cstdlib>
# include <iostream>
# include <stack>
# include <string>
# include <thread>
# include <chrono>

# define mln_entering(NAME)                                     \
  mln::trace::scoped_trace __mln_trace__COUNTER__(NAME);

# define mln_exiting() ;

# define mln_scoped_entering(NAME)                              \
  mln::trace::scoped_trace __mln_trace__COUNTER__(NAME);


namespace mln
{

  namespace trace
  {

    static bool verbose = (std::getenv("TRACE") != NULL);
    static thread_local int __stack_depth = 0;

    struct scoped_trace
    {
      scoped_trace(const std::string& desc);
      scoped_trace(const scoped_trace&) = delete;
      scoped_trace& operator= (const scoped_trace&) = delete;
      ~scoped_trace();

    private:
      void entering();
      void exiting();

      int         m_depth;
      std::string m_desc;
      std::chrono::time_point<std::chrono::system_clock>   m_clock;
    };

    inline
    scoped_trace::scoped_trace(const std::string& desc)
      : m_depth (__stack_depth++),
        m_desc(desc),
        m_clock(std::chrono::system_clock::now())
    {
      entering();
    }

    inline
    scoped_trace::~scoped_trace()
    {
      exiting();
      --__stack_depth;
    }

    inline
    void
    scoped_trace::entering()
    {
      if (verbose) {
        for (int k = 0; k < m_depth; ++k)
          std::clog.put(' ');
	std::clog << "#" << std::this_thread::get_id() << " - " << m_desc << std::endl;
      }
    }

    inline
    void
    scoped_trace::exiting()
    {
      if (verbose) {
        for (int k = 0; k < m_depth; ++k)
          std::clog.put(' ');
        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_clock);
	std::clog << "#" << std::this_thread::get_id() << " - " << m_desc
		  << " in " << duration.count() << "ms" << std::endl;
      }
    }

    inline
    void warn(const std::string& msg)
    {
      if (verbose) {
        for (int k = 0; k < __stack_depth; ++k)
          std::clog.put(' ');

        std::clog << "#" << std::this_thread::get_id() << " - "
                  << msg << std::endl;
      }
    }

  }
}

# ifdef _MSVC_VER
#  pragma warning(pop)
# endif

#endif // ! TRACE_HPP
