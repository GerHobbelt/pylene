#ifndef MLN_TRANSFORM_CHAMFER_DISTANCE_TRANSFORM_HPP
# define MLN_TRANSFORM_CHAMFER_DISTANCE_TRANSFORM_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/neighborhood/neighborhood.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/trace.hpp>

namespace mln
{

  namespace transform
  {

    namespace internal
    {
      struct add_saturate;
    }

    template <typename DistanceType = int, class I, class N, class OutputImage, class SumOp = internal::add_saturate>
    void
    chamfer_distance_transform(const Image<I>& f,
                               const Neighborhood<N>& nbh,
                               OutputImage&& out,
                               bool background_is_object = false,
                               SumOp add = SumOp());



    template <typename DistanceType = int, class I, class N>
    mln_ch_value(I, DistanceType)
    chamfer_distance_transform(const Image<I>& f,
                               const Neighborhood<N>& nbh,
                               bool background_is_object = false);



    /**********************************/
    /**   Implementation            ***/
    /**********************************/

    namespace internal
    {

      struct add_saturate
      {
        template <class T, class U>
        auto operator() (T x, U y) const
        {
          using R = decltype(std::declval<T>() +  std::declval<U>());
          if (x > (value_traits<R>::max() - y))
            return value_traits<R>::max();
          else
            return x + y;
        }
      };


    }

    template <class DistanceType, class I, class N, class OutputImage, class SumOp>
    void
    chamfer_distance_transform(const Image<I>& f_,
                               const Neighborhood<N>& nbh_,
                               OutputImage&& out,
                               bool background_is_object,
                               SumOp add)

    {
      static_assert( std::is_convertible<mln_value(I), bool>::value,
                     "Input value type must be convertible to bool");

      mln_entering("mln::transform::chamfer_distance_transform");

      using distance_t = DistanceType;

      const I& f = exact(f_);
      const N& nbh = exact(nbh_);

      if (background_is_object)
        extension::fill(out, value_traits<distance_t>::max());
      else
        extension::fill(out, 0);

      // Forward scan
      {
        mln_pixter(pxin, pxout, f, out);
        mln_iter(q, nbh(pxout));

        mln_forall(pxin, pxout) {
          if (pxin->val()) {
            distance_t vmin = value_traits<distance_t>::max();
            mln_forall(q)
              if (q->index() < pxout->index()) {
                vmin = std::min<distance_t>(vmin, add(q->val(), q->weight()));
              }
            pxout->val() = vmin;
          } else {
            pxout->val() = 0;
          }
        }
      }

      // Backward
      {
        mln_riter(pxin, f.pixels());
        mln_riter(pxout, out.pixels());
        mln_iter(q, nbh(pxout));

        mln_forall(pxin, pxout)
          if (pxin->val())
            {
              distance_t vmin = pxout->val();
              mln_forall(q)
                if (q->index() > pxout->index()) {
                  vmin = std::min<distance_t>(vmin, add(q->val(), q->weight()));
                }
              pxout->val() = vmin;
            }
      }

      mln_exiting();
    }



    template <typename DistanceType, class I, class N>
    mln_ch_value(I, DistanceType)
    chamfer_distance_transform(const Image<I>& f_,
                               const Neighborhood<N>& nbh_,
                               bool background_is_object)
    {
      const I& f = exact(f_);
      const N& nbh = exact(nbh_);

      mln_ch_value(I, DistanceType) out = imchvalue<DistanceType>(f);

      chamfer_distance_transform<DistanceType>(f, nbh, out, background_is_object);

      return out;
    }

  }

}

#endif // ! MLN_TRANSFORM_CHAMFER_DISTANCE_TRANSFORM_HPP
