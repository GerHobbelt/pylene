#include <ano/morpho/private/dilation.2d.hpp>

namespace ano::morpho::details
{

  void FilterChain::ExecuteTile(ano::box2d roi) const
  {
    [[maybe_unused]] ano::box2d roi_ = roi;

    roi = this->ComputeInputRegion(roi);

    ano::ndbuffer_image aux = m_tile_in;
    ano::ndbuffer_image out = m_tile_out;

    aux.set_domain_topleft(roi.tl());
    out.set_domain_topleft(roi.tl());
    load_tile(aux.clip(roi));


    for (auto& f : this->m_filters)
    {
      roi = f->ComputeOutputRegion(roi);
      f->Execute(aux, out.clip(roi));
      std::swap(aux, out);
    }

    assert(roi == roi_);
    write_tile(aux.clip(roi));
  }


  void FilterChain::addFilter(std::unique_ptr<SimpleFilter2D> f)
  {
    m_filters.push_back(std::move(f));
  }
  void FilterChain::SetLoadFunction(std::function<void(ano::ndbuffer_image&&)> fn)
  {
    load_tile = std::move(fn);
  }
  void FilterChain::SetWriteFunction(std::function<void(ano::ndbuffer_image&&)> fn)
  {
    write_tile = std::move(fn);
  }


  ano::box2d FilterChain::ComputeInputRegion(ano::box2d roi) const
  {
    for (auto f = m_filters.rbegin(); f != m_filters.rend(); ++f)
      roi = (*f)->ComputeInputRegion(roi);
    return roi;
  }


  std::unique_ptr<ParallelLocalCanvas2DBase> FilterChain::clone() const
  {
    std::unique_ptr<FilterChain> c(new FilterChain());
    c->load_tile   = this->load_tile;
    c->write_tile  = this->write_tile;
    c->create_tile = this->create_tile;

    int w = this->m_tile_in.width();
    int h = this->m_tile_in.height();

    c->m_tile_in  = this->create_tile(w, h);
    c->m_tile_out = this->create_tile(w, h);

    c->m_filters.resize(this->m_filters.size());
    std::transform(this->m_filters.begin(), this->m_filters.end(), c->m_filters.begin(),
                   [](const auto& f) { return f->Clone(); });
    return c;
  }


} // namespace ano::morpho::details
