#ifndef MLN_KERNELV3_EXPR_IMAGE_HPP
# define MLN_KERNELV3_EXPR_IMAGE_HPP

# include <mln/core/concept/image.hpp>
# include <mln/kernelv3/expr/expr.hpp>
# include <mln/kernelv3/expr/point.hpp>

namespace mln
{
  namespace kernel
  {
    /// \brief Image expression generator
    template <class I, int UID = -1>
    struct Image;

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class I, int UID = -1>
    struct image_expr_p : expr< tag::image_p, image_expr_p<I,UID> >
    {
      using Base = Expr< image_expr_p<I,UID> >;

      constexpr image_expr_p(I& f) : value(f) {}
      constexpr int id() { return UID; }

      using Base::operator=;

      auto operator= (const image_expr_p& e) { return Base::operator=(e); }

      I& value;
    };


    template <class I, int UID>
    struct Image
    {
      constexpr
      auto operator() (Point) const { return ima; }

      image_expr_p<I, UID> ima;
    };


    template <int UID, class I>
    Image<I, UID>
    make_image(mln::Image<I>& f)
    {
      return {exact(f)};
    }

    template <int UID, class I>
    Image<const I, UID>
    make_image(const mln::Image<I>& f)
    {
      return {exact(f)};
    }

    // Cannot bind to a temporary image.
    template <int UID, class I>
    Image<I, UID>
    make_image(mln::Image<I>&& f) = delete;

  } // end of namespace mln::kernel

} // end of namespace mln

#endif //!MLN_KERNELV3_EXPR_IMAGE_HPP
