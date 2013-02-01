#ifndef MLN_CORE_CONCEPT_ACCUMULATOR_HPP
# define MLN_CORE_CONCEPT_ACCUMULATOR_HPP

# include <mln/core/concept/check.hpp>
# include <boost/concept_check.hpp>

namespace mln
{

  template <typename Acc>
  struct Accumulator
  {

    BOOST_CONCEPT_USAGE(Accumulator)
    {
      typedef typename Acc::argument_type    argument_type;

      void (Acc::*method1) ()                     = &Acc::init;
      void (Acc::*method2) (const argument_type&) = &Acc::take;

      (void) method1;
      (void) method2;
    }
  };

} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_PIXEL_HPP
