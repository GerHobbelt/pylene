template <class T>
image2d<float> stretch(const image2d<T>& src)
{
  auto res  = image2d<float>(src.width(), src.height());
  auto span = src.values();
  std::transform(span.begin(), span.end(), res.values().begin(),
                 [](T val) -> float { return static_cast<float>(val) / std::numeric_limits<T>::max(); });
  return res;
}

image2d<void> stretch(const image2d<>& src);