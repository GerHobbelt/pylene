#include <mln/core/value/int.hpp>

#include <type_traits>

#include <gtest/gtest.h>

TEST(Value, Integers)
{
    using namespace mln;

    UInt<12> x = 13;
    UInt<12> y = 18;
    ASSERT_EQ(x + y, 31);

    Int<14> z = 12;
    y += z;
    ++y;
    ASSERT_EQ(y, 31);

    Int<7> zz = x + z;
    ASSERT_EQ(zz, 25);

    typedef typename std::common_type<UInt<12>, UInt<12>>::type T;
    ASSERT_TRUE((std::is_same<T, UInt<12>>::value));
    ASSERT_TRUE((sizeof(UInt<12>) == sizeof(short)));
}
