#pragma once

#include <ano/core/box.hpp>
#include <ano/core/canvas/local_algorithm.hpp>

//#include <boost/container/small_vector.hpp>

namespace ano::canvas
{

  template <class Accu, class SE, class I, class J,
            bool __incremental__ = SE::incremental::value&& Accu::has_untake::value&&
                                                            details::is_domain_row_contiguous<image_domain_t<I>>::value>
  class LocalAccumulation;


  template <class Accu, class SE, class I, class J>
  class LocalAccumulation<Accu, SE, I, J, false>
    : public LocalAlgorithm<SE, I, J, LocalAccumulation<Accu, SE, I, J, false>>
  {
  private:
    using self_t = LocalAccumulation;
    using base_t = LocalAlgorithm<SE, I, J, self_t>;

    Accu m_accu;
    friend base_t;

  public:
    LocalAccumulation(Accu accu, SE se, I& f, J& g)
      : base_t{std::move(se), f, g}
      , m_accu{std::move(accu)}
    {
    }


  private:
    void ExecuteAtLineStart() final { m_accu.init(); }

    void EvalBeforeLocalLoop(image_reference_t<I> /*pval_i*/, image_reference_t<J> /*pval_j*/) final { m_accu.init(); }

    void EvalAfterLocalLoop(image_reference_t<I> /* pval_i */, image_reference_t<J> pval_j) final
    {
      pval_j = m_accu.to_result();
    }

    void EvalInLocalLoop(image_reference_t<I> nval_i, image_reference_t<I> /* pval_i */,
                         image_reference_t<J> /*pval_j*/) final
    {
      m_accu.take(nval_i);
    }

  public:
    /*
    void ExecuteWithIndexes()
      {
        ano_entering("LocalAlgorithm::Execute (Non-incremental)");

        std::size_t se_size = ::ranges::size(this->m_se.offsets());

        // Make a local copy to prevent aliasing
        std::vector<std::ptrdiff_t>   offsets(se_size);
        std::vector<image_point_t<I>> dps(se_size);
        {
          std::size_t i = 0;
          for (auto dp : this->m_se.offsets())
          {
            offsets[i] = this->m_i.delta_index(dp);
            dps[i]     = dp;
            i++;
          }
        }

        auto zz = ranges::view::zip(this->m_i.pixels(), this->m_j.pixels());
        for (auto rows : ranges::rows(zz))
        {
          this->ExecuteAtLineStart();
          for (auto [px_i, px_j] : rows)
          {
            this->EvalBeforeLocalLoop(px_i.val(), px_j.val());
            for (size_t i = 0; i < se_size; ++i)
            {
              auto tmp = px_i; tmp.ishift(dps[i], offsets[i]);
              this->EvalInLocalLoop(tmp.val(), px_i.val(), px_j.val());

            }
            this->EvalAfterLocalLoop(px_i.val(), px_j.val());
          }
        }
      }
    */
  };


  template <class Accu, class SE, class I, class J>
  class LocalAccumulation<Accu, SE, I, J, true>
    : public IncrementalLocalAlgorithm<SE, I, J, LocalAccumulation<Accu, SE, I, J, true>>
  {
  private:
    using self_t   = LocalAccumulation;
    using base_t   = IncrementalLocalAlgorithm<SE, I, J, LocalAccumulation>;
    using base_2_t = LocalAlgorithm<SE, I, J, LocalAccumulation>;
    Accu m_accu;

  public:
    LocalAccumulation(Accu accu, SE se, I& f, J& g)
      : base_t{std::move(se), f, g}
      , m_accu{std::move(accu)}
    {
    }

    friend base_t;
    friend base_2_t;

  private:
    void ExecuteAtLineStart() final { m_accu.init(); }

    void EvalBeforeLocalLoop(image_reference_t<I> /* pval_i */, image_reference_t<J> /* pval_j */) final
    {
      m_accu.init();
    }

    void EvalAfterLocalLoop(image_reference_t<I> /* pval_i */, image_reference_t<J> pval_j) final
    {
      pval_j = m_accu.to_result();
    }

    void EvalInLocalLoop(image_reference_t<I> nval_i, image_reference_t<I> /* pval_i */,
                         image_reference_t<J> /*pval_j*/) final
    {
      m_accu.take(nval_i);
    }


    void EvalInIncLoop(image_reference_t<I> nval_i, image_reference_t<I> /*pval_i*/,
                       image_reference_t<J> /*pval_j*/) final
    {
      m_accu.take(nval_i);
    }

    void EvalInDecLoop(image_reference_t<I> nval_i, image_reference_t<I> /*pval_i*/,
                       image_reference_t<J> /*pval_j*/) final
    {
      m_accu.untake(nval_i);
    }
  };


  template <class Accu, class SE, class I, class J>
  LocalAccumulation(Accu, SE, I&, J&) -> LocalAccumulation<Accu, SE, I, J>;

  // Clang7 doesn't deduce the template parameters correctly
  template <class Accu, class SE, class I, class J>
  auto make_LocalAccumulation(Accu&& accu, SE&& se, I&& i, J&& j)
      -> LocalAccumulation<std::remove_cv_t<std::remove_reference_t<Accu>>,
                           std::remove_cv_t<std::remove_reference_t<SE>>, std::remove_cv_t<std::remove_reference_t<I>>,
                           std::remove_cv_t<std::remove_reference_t<J>>>
  {
    return {std::forward<Accu>(accu), std::forward<SE>(se), std::forward<I>(i), std::forward<J>(j)};
  }

} // namespace ano::canvas
