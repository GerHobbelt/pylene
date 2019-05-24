namespace mln
{
  /*
  ** std::any has a dynamic allocation mechanism, hence a lack of performance.
  ** to fix this, we use the custom `any_ref` class, which is a reference to a value of `any` type
  */
  class any_ref
  {
    public:
      any_ref() = default;
      any_ref(void*& elm);
      void* data();
    private:
      void*   m_data;
  };
} // namespace mln
