#ifndef MLN_CORE_CONCEPT_ACCUMULATOR_HPP
# define MLN_CORE_CONCEPT_ACCUMULATOR_HPP

# include <mln/core/concept/object.hpp>
# include <mln/core/concept/check.hpp>
# include <boost/concept_check.hpp>

namespace mln
{

  template <typename Acc>
  struct AccumulatorLike : Object<Acc>
  {
  };


  template <typename Acc>
  struct Accumulator : AccumulatorLike<Acc>
  {

    BOOST_CONCEPT_USAGE(Accumulator)
    {
      typedef typename Acc::argument_type    argument_type;
      typedef typename Acc::result_type      result_type;

      void (Acc::*method1) ()                     = &Acc::init;
      void (Acc::*method2) (const argument_type&) = &Acc::take;

      (void) method1;
      (void) method2;
    }
  };

  template <typename E>
  struct FeatureSet : AccumulatorLike<E>
  {
  private:
    template <typename A>
    struct check_inner_struct_apply
    {
      template <typename T>
      using apply = typename A::template apply<T>::type;
    };

  public:
    BOOST_CONCEPT_USAGE(FeatureSet)
    {
      typedef typename E::features    features;
      check_inner_struct_apply<E> ();
    }

  };


} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_PIXEL_HPP
