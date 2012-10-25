#ifndef MLN_CORE_ALGORITHM_TRANSFORM_HPP
# define MLN_CORE_ALGORITHM_TRANSFORM_HPP

# include <mln/core/image/image.hpp>

namespace mln {


  template <typename InputImage, typename OutputImage, class UnaryFunction>
  OutputImage&
  transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>& output);

  template <typename InputImage, typename OutputImage, class UnaryFunction>
  OutputImage&&
  transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>&& output);


  template <typename InputImage, class UnaryFunction>
  mln_ch_value(InputImage, typename std::result_of<UnaryFunction(mln_value(InputImage))>::type)
    transform(const Image<InputImage>& input, UnaryFunction f);


/******************************************/
/****          Implementation          ****/
/******************************************/

  namespace impl
  {
    template <typename I, typename J, class UnaryFunction>
    void
    transform(const I& input, UnaryFunction f, J& output)
    {
      mln_viter(vin, vout, input, output);
      mln_forall(vin, vout)
	*vout = f(*vin);
    }

  }


  template <typename InputImage, typename OutputImage, class UnaryFunction>
  OutputImage&
  transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>& output)
  {
    OutputImage& out = exact(output);
    impl::transform(exact(input), f, exact(output));
    return out;
  }

  template <typename InputImage, typename OutputImage, class UnaryFunction>
  OutputImage&&
  transform(const Image<InputImage>& input, UnaryFunction f, Image<OutputImage>&& output)
  {
    transform(input, f, output);
    return move_exact(output);
  }


  template <typename InputImage, class UnaryFunction>
  mln_ch_value(InputImage, typename std::result_of<UnaryFunction(mln_value(InputImage))>::type)
    transform(const Image<InputImage>& input, UnaryFunction f)
  {
    typedef typename std::result_of<UnaryFunction(mln_value(InputImage))>::type T;
    mln_ch_value(InputImage, T) out;
    resize(out, input);

    transform(input, f, out);
    return out;
  }

}



#endif // ! MLN_CORE_ALGORITHM_TRANSFORM_HPP
