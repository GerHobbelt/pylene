#ifndef TRACE_HPP
# define TRACE_HPP

//# include <boost/current_function.hpp>
# include <ctime>
# include <cstdlib>
# include <iostream>
# include <stack>
# include <string>

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

    inline
    void entering(const std::string& fname)
    {
      callstack.emplace(fname);
      if (verbose)
	std::clog << std::string(callstack.size(), ' ') << fname << std::endl;
    };


    inline
    void exiting()
    {
      if (verbose) {
	trace_t tr = callstack.top();
	std::clog << std::string(callstack.size(), ' ') << tr.fname
		  << " in " << ((float)(std::clock() - tr.clock) / CLOCKS_PER_SEC) << std::endl;
      }
      callstack.pop();
    };


  }

}

#endif // ! TRACE_HPP
