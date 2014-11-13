#ifndef TRACE_HPP
# define TRACE_HPP

//# include <boost/current_function.hpp>
# include <ctime>
# include <cstdlib>
# include <iostream>
# include <stack>
# include <string>
# include <mutex>

# define mln_entering(NAME) \
  mln::trace::entering(NAME);

# define mln_exiting()  mln::trace::exiting();

namespace mln
{

  namespace trace
  {
    struct trace_t
    {
      explicit trace_t(const std::string& fun)
	: fname (fun)
      {
	clock = std::clock();
      }

      std::string  fname;
      std::clock_t clock;
    };


    static std::stack<trace_t> callstack;
    static bool verbose = (std::getenv("TRACE") != NULL);
    static std::mutex stack_mutex;

    static inline
    void entering(const std::string& fname)
    {
      if (verbose) {
        std::lock_guard<std::mutex> lock(stack_mutex);
        callstack.emplace(fname);

	std::clog << std::string(callstack.size(), ' ') << fname << std::endl;
      }
    };


    static inline
    void exiting()
    {
      if (verbose) {
        std::lock_guard<std::mutex> lock(stack_mutex);

	trace_t tr = callstack.top();
	std::clog << std::string(callstack.size(), ' ') << tr.fname
		  << " in " << ((float)(std::clock() - tr.clock) / CLOCKS_PER_SEC) << std::endl;

        callstack.pop();
      }
    };

    inline
    void warn(const std::string& msg)
    {
      if (verbose)
        std::clog << std::string(callstack.size(), ' ') << msg << std::endl;
    }

  }

}

#endif // ! TRACE_HPP
