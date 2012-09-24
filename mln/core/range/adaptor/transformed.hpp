#ifndef MLN_CORE_RANGE_ADAPTOR_TRANSFORMED_HPP
# define MLN_CORE_RANGE_ADAPTOR_TRANSFORMED_HPP


namespace mln
{

  namespace adaptors
  {

    template <typename Range, typename Function>
    struct transformed_range
    {
      typedef transform_iterator<Range::iterator, Function> iterator;


      transformed_range(const Range& rng, const Function& f)
        : rng_ (rng), f_ (f)
      {
      }

      iterator iter() const
      {
        return iterator(rng_.iter(), f_);
      }

    private:
      Range rng_;
      Function f_;
    };


    template <typename Range, typename Function>
    transformed_range<Range, Function>
    transform(const Range& rng, const Function& f)
    {
      return transformed_range<Range, Function>(rng, f);
    }

  } // end of namespace mln::adaptors

} // end of namespace mln



#endif //!MLN_CORE_RANGE_ADAPTOR_TRANSFORMED_HPP
