#include <fixtures/ImageCompare/image_compare.hpp>
#include <mln/core/image/ndbuffer_image.hpp>


namespace fixtures::ImageCompare::impl
{

  // Compare two buffer-encoded images
  ::testing::AssertionResult compare(const mln::ndbuffer_image& a, const mln::ndbuffer_image& b, int comparaison_flags,
                                     std::function<int(const void* a, const void* b, std::size_t n)> linecmp_fn,
                                     std::function<void(const mln::ndbuffer_image&)> print_fn)
  {
    mln::ndbuffer_image A = a;
    mln::ndbuffer_image B = b;


    if (A.sample_type() != B.sample_type())
      return ::testing::AssertionFailure() << "A and B sample types differ.";

    if (A.pdim() != B.pdim())
      return ::testing::AssertionFailure() << "A and B dimension differ.";


    if (comparaison_flags & COMPARE_EXTENSION)
    {
      A.inflate_domain_to_extension();
      B.inflate_domain_to_extension();
    }

    if (comparaison_flags & COMPARE_DOMAIN)
      if (A.domain() != B.domain())
        return ::testing::AssertionFailure() << "A and B domains differ.";

    for (int i = 0; i < a.pdim(); ++i)
      if (A.size(i) != B.size(i))
        return ::testing::AssertionFailure() << "A and B domains extents mismatch.";


    mln::sample_type_id id = a.sample_type();
    // If the comparison function is not provided, we use memcmp (type are supposed to be trivially comparable
    if (!linecmp_fn)
      linecmp_fn = std::memcmp;

    std::size_t n     = a.width() * mln::get_sample_type_id_traits(id).size();
    auto        a_buf = A.buffer();
    auto        b_buf = B.buffer();

    for (int w = 0; w < A.size(3); ++w)
      for (int z = 0; z < A.size(2); ++z)
        for (int y = 0; y < A.size(1); ++y)
        {
          const std::byte* alineptr = a_buf + y * a.byte_stride(1) + z * a.byte_stride(2) + w * a.byte_stride(3);
          const std::byte* blineptr = b_buf + y * b.byte_stride(1) + z * b.byte_stride(2) + w * b.byte_stride(3);
          if (linecmp_fn(alineptr, blineptr, n) != 0)
          {
            if (print_fn)
            {
              fmt::print("With A = \n");
              print_fn(a);
              fmt::print("  and B = \n");
              print_fn(b);
            }
            else
            {
              fmt::print("<Input images not printable.\n>");
            }
            return ::testing::AssertionFailure() << "The lines " << y << " differ.";
          }
        }

    return ::testing::AssertionSuccess();
  }

}
