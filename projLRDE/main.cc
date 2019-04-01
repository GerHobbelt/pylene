
#include "ima.hh"

struct Foo
{
    Foo()
    {
        std::cout << "Constructed.\n";
    }
    ~Foo()
    {
        std::cout << "Destroyed.\n";
    }
};

int main(void)
{
    int8_t *buffer = new int8_t[9];
    for (int i = 0; i < 9; ++i)
    {
        buffer[i] = i;
    }
    auto img = image2d<>::from_buffer(reinterpret_cast<std::byte*>(buffer), 3, 3, Info::INT8_V);
    for (int i = 0; i < 9; ++i)
    {
        std::cout << static_cast<int>(reinterpret_cast<int8_t*>(img.buffer())[i]) << '\n';
    }
    auto stretched = stretch(img);
    for (auto i : stretched.values())
    {
        std::cout << i << "\n";
    }
//    auto ima = image2d<>::from_buffer(buffer, 3, 3, Info::INT8_V);
//    std::cout << "created img from buf.\n";
//    /* buf = { 0, 1, 2, 3, 4, 5, 6, 7, 8 } */
//    auto im2 = stretch(ima);
//    for (auto i : im2.values())
//        std::cout << i << "\n";

//FIXME should properly handle error
//    auto ntima = image2d<>(3, 3, Info::OTHER);
//    auto ntima2 = stretch(ntima);
//    for (auto i : ntima2.values())
//    {
//        std::cout << i << "\n";
//    }
}