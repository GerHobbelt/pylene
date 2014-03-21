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
  unary_image_expr<UnaryFunction, InputImage>
  lazy_transform(InputImage&& input, UnaryFunction f);

  template <typename InputImage, class UnaryFunction>
  mln_ch_value(InputImage,
	       typename std::decay<
		 typename std::result_of<UnaryFunction(mln_value(InputImage))>::type
		 >::type)
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
    mln::transform(input, f, output);
    return move_exact(output);
  }


  template <typename InputImage, class UnaryFunction>
  mln_ch_value(InputImage,
	       typename std::decay<
		 typename std::result_of<UnaryFunction(mln_value(InputImage))>::type
		 >::type)
    transform(const Image<InputImage>& input, UnaryFunction f)
  {
    typedef typename std::decay<
      typename std::result_of<UnaryFunction(mln_value(InputImage))>::type
      >::type T;

    mln_ch_value(InputImage, T) out;
    resize(out, input);

    mln::transform(input, f, out);
    return out;
  }

  template <typename InputImage, class UnaryFunction>
  unary_image_expr<UnaryFunction, InputImage>
  lazy_transform(InputImage&& input, UnaryFunction f)
  {
    return make_unary_image_expr(std::forward<InputImage>(input), f);
  }


}



#endif // ! MLN_CORE_ALGORITHM_TRANSFORM_HPP
