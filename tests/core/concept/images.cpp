#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/concept/new/images.hpp>

#include <gtest/gtest.h>

#include <tuple>
#include <type_traits>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

TEST(Core, Concept_Image)
{
  static_assert(concepts::Image<archetypes::Image>);
  static_assert(concepts::Image<archetypes::ForwardImage>);
  static_assert(concepts::Image<archetypes::InputImage>);
  static_assert(concepts::Image<archetypes::IndexableImage>);
  static_assert(concepts::Image<archetypes::AccessibleImage>);
  static_assert(concepts::Image<archetypes::BidirectionalImage>);
  static_assert(concepts::Image<archetypes::RawImage>);
}

TEST(Core, Concept_ForwardImage)
{
  static_assert(concepts::ForwardImage<archetypes::Image>);
  static_assert(concepts::ForwardImage<archetypes::ForwardImage>);
  static_assert(concepts::ForwardImage<archetypes::InputImage>);
  static_assert(concepts::ForwardImage<archetypes::IndexableImage>);
  static_assert(concepts::ForwardImage<archetypes::AccessibleImage>);
  static_assert(concepts::ForwardImage<archetypes::BidirectionalImage>);
  static_assert(concepts::ForwardImage<archetypes::RawImage>);
}

TEST(Core, Concept_InputImage)
{
  static_assert(concepts::InputImage<archetypes::Image>);
  static_assert(concepts::InputImage<archetypes::ForwardImage>);
  static_assert(concepts::InputImage<archetypes::InputImage>);
  static_assert(concepts::InputImage<archetypes::IndexableImage>);
  static_assert(concepts::InputImage<archetypes::AccessibleImage>);
  static_assert(concepts::InputImage<archetypes::BidirectionalImage>);
  static_assert(concepts::InputImage<archetypes::RawImage>);
}

TEST(Core, Concept_IndexableImage)
{
  static_assert(not concepts::IndexableImage<archetypes::Image>);
  static_assert(not concepts::IndexableImage<archetypes::ForwardImage>);
  static_assert(not concepts::IndexableImage<archetypes::InputImage>);
  static_assert(concepts::IndexableImage<archetypes::IndexableImage>);
  static_assert(not concepts::IndexableImage<archetypes::AccessibleImage>);
  static_assert(not concepts::IndexableImage<archetypes::BidirectionalImage>);
  static_assert(concepts::IndexableImage<archetypes::RawImage>);
}

TEST(Core, Concept_AccessibleImage)
{
  static_assert(not concepts::AccessibleImage<archetypes::Image>);
  static_assert(not concepts::AccessibleImage<archetypes::ForwardImage>);
  static_assert(not concepts::AccessibleImage<archetypes::InputImage>);
  static_assert(not concepts::AccessibleImage<archetypes::IndexableImage>);
  static_assert(concepts::AccessibleImage<archetypes::AccessibleImage>);
  static_assert(not concepts::AccessibleImage<archetypes::BidirectionalImage>);
  static_assert(concepts::AccessibleImage<archetypes::RawImage>);
}

TEST(Core, Concept_BidirectionalImage)
{
  static_assert(not concepts::BidirectionalImage<archetypes::Image>);
  static_assert(not concepts::BidirectionalImage<archetypes::ForwardImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::InputImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::IndexableImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::AccessibleImage>);
  static_assert(concepts::BidirectionalImage<archetypes::BidirectionalImage>);
  static_assert(concepts::BidirectionalImage<archetypes::RawImage>);
}

TEST(Core, Concept_RawImage)
{
  static_assert(not concepts::RawImage<archetypes::Image>);
  static_assert(not concepts::RawImage<archetypes::ForwardImage>);
  static_assert(not concepts::RawImage<archetypes::InputImage>);
  static_assert(not concepts::RawImage<archetypes::IndexableImage>);
  static_assert(not concepts::RawImage<archetypes::AccessibleImage>);
  static_assert(not concepts::RawImage<archetypes::BidirectionalImage>);
  static_assert(concepts::RawImage<archetypes::RawImage>);
}

TEST(Core, Concept_ExtendedImage)
{
  static_assert(not concepts::ExtendedImage<archetypes::Image>);
  static_assert(not concepts::ExtendedImage<archetypes::ForwardImage>);
  static_assert(not concepts::ExtendedImage<archetypes::InputImage>);
  static_assert(not concepts::ExtendedImage<archetypes::IndexableImage>);
  static_assert(not concepts::ExtendedImage<archetypes::AccessibleImage>);
  static_assert(not concepts::ExtendedImage<archetypes::BidirectionalImage>);
  static_assert(not concepts::ExtendedImage<archetypes::RawImage>);
  static_assert(concepts::ExtendedImage<archetypes::ExtendedImage>);
}

TEST(Core, Concept_OutputImage) // ForwardImage, InputImage
{
  static_assert(concepts::Image<archetypes::Image> && not concepts::OutputImage<archetypes::Image>);
  static_assert(concepts::Image<archetypes::ForwardImage> && not concepts::OutputImage<archetypes::ForwardImage>);
  static_assert(concepts::Image<archetypes::InputImage> && not concepts::OutputImage<archetypes::InputImage>);
  static_assert(concepts::Image<archetypes::IndexableImage> && not concepts::OutputImage<archetypes::IndexableImage>);
  static_assert(concepts::Image<archetypes::AccessibleImage> && not concepts::OutputImage<archetypes::AccessibleImage>);
  static_assert(concepts::Image<archetypes::BidirectionalImage> &&
                not concepts::OutputImage<archetypes::BidirectionalImage>);
  static_assert(concepts::Image<archetypes::RawImage> && not concepts::OutputImage<archetypes::RawImage>);

  static_assert(concepts::Image<archetypes::OutputImage> && concepts::OutputImage<archetypes::OutputImage>);
  static_assert(concepts::Image<archetypes::OutputForwardImage> &&
                concepts::OutputImage<archetypes::OutputForwardImage>);
  static_assert(concepts::Image<archetypes::OutputIndexableImage> &&
                concepts::OutputImage<archetypes::OutputIndexableImage>);
  static_assert(concepts::Image<archetypes::OutputAccessibleImage> &&
                concepts::OutputImage<archetypes::OutputAccessibleImage>);
  static_assert(concepts::Image<archetypes::OutputBidirectionalImage> &&
                concepts::OutputImage<archetypes::OutputBidirectionalImage>);
  static_assert(concepts::Image<archetypes::OutputRawImage> && concepts::OutputImage<archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputIndexableImage)
{
  static_assert(not concepts::IndexableImage<archetypes::Image> && not concepts::OutputImage<archetypes::Image>);
  static_assert(not concepts::IndexableImage<archetypes::ForwardImage> &&
                not concepts::OutputImage<archetypes::ForwardImage>);
  static_assert(not concepts::IndexableImage<archetypes::InputImage> &&
                not concepts::OutputImage<archetypes::InputImage>);
  static_assert(concepts::IndexableImage<archetypes::IndexableImage> &&
                not concepts::OutputImage<archetypes::IndexableImage>);
  static_assert(not concepts::IndexableImage<archetypes::AccessibleImage> &&
                not concepts::OutputImage<archetypes::AccessibleImage>);
  static_assert(not concepts::IndexableImage<archetypes::BidirectionalImage> &&
                not concepts::OutputImage<archetypes::BidirectionalImage>);
  static_assert(concepts::IndexableImage<archetypes::RawImage> && not concepts::OutputImage<archetypes::RawImage>);

  static_assert(not concepts::IndexableImage<archetypes::OutputImage> &&
                concepts::OutputImage<archetypes::OutputImage>);
  static_assert(not concepts::IndexableImage<archetypes::OutputForwardImage> &&
                concepts::OutputImage<archetypes::OutputForwardImage>);
  static_assert(concepts::IndexableImage<archetypes::OutputIndexableImage> &&
                concepts::OutputImage<archetypes::OutputIndexableImage>);
  static_assert(not concepts::IndexableImage<archetypes::OutputAccessibleImage> &&
                concepts::OutputImage<archetypes::OutputAccessibleImage>);
  static_assert(not concepts::IndexableImage<archetypes::OutputBidirectionalImage> &&
                concepts::OutputImage<archetypes::OutputBidirectionalImage>);
  static_assert(concepts::IndexableImage<archetypes::OutputRawImage> &&
                concepts::OutputImage<archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputAccessibleImage)
{
  static_assert(not concepts::AccessibleImage<archetypes::Image> && not concepts::OutputImage<archetypes::Image>);
  static_assert(not concepts::AccessibleImage<archetypes::ForwardImage> &&
                not concepts::OutputImage<archetypes::ForwardImage>);
  static_assert(not concepts::AccessibleImage<archetypes::InputImage> &&
                not concepts::OutputImage<archetypes::InputImage>);
  static_assert(not concepts::AccessibleImage<archetypes::IndexableImage> &&
                not concepts::OutputImage<archetypes::IndexableImage>);
  static_assert(concepts::AccessibleImage<archetypes::AccessibleImage> &&
                not concepts::OutputImage<archetypes::AccessibleImage>);
  static_assert(not concepts::AccessibleImage<archetypes::BidirectionalImage> &&
                not concepts::OutputImage<archetypes::BidirectionalImage>);
  static_assert(concepts::AccessibleImage<archetypes::RawImage> && not concepts::OutputImage<archetypes::RawImage>);

  static_assert(not concepts::AccessibleImage<archetypes::OutputImage> &&
                concepts::OutputImage<archetypes::OutputImage>);
  static_assert(not concepts::AccessibleImage<archetypes::OutputForwardImage> &&
                concepts::OutputImage<archetypes::OutputForwardImage>);
  static_assert(not concepts::AccessibleImage<archetypes::OutputIndexableImage> &&
                concepts::OutputImage<archetypes::OutputIndexableImage>);
  static_assert(concepts::AccessibleImage<archetypes::OutputAccessibleImage> &&
                concepts::OutputImage<archetypes::OutputAccessibleImage>);
  static_assert(not concepts::AccessibleImage<archetypes::OutputBidirectionalImage> &&
                concepts::OutputImage<archetypes::OutputBidirectionalImage>);
  static_assert(concepts::AccessibleImage<archetypes::OutputRawImage> &&
                concepts::OutputImage<archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputBidirectionalImage)
{
  static_assert(not concepts::BidirectionalImage<archetypes::Image> && not concepts::OutputImage<archetypes::Image>);
  static_assert(not concepts::BidirectionalImage<archetypes::ForwardImage> &&
                not concepts::OutputImage<archetypes::ForwardImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::InputImage> &&
                not concepts::OutputImage<archetypes::InputImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::IndexableImage> &&
                not concepts::OutputImage<archetypes::IndexableImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::AccessibleImage> &&
                not concepts::OutputImage<archetypes::AccessibleImage>);
  static_assert(concepts::BidirectionalImage<archetypes::BidirectionalImage> &&
                not concepts::OutputImage<archetypes::BidirectionalImage>);
  static_assert(concepts::BidirectionalImage<archetypes::RawImage> && not concepts::OutputImage<archetypes::RawImage>);

  static_assert(not concepts::BidirectionalImage<archetypes::OutputImage> &&
                concepts::OutputImage<archetypes::OutputImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::OutputForwardImage> &&
                concepts::OutputImage<archetypes::OutputForwardImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::OutputIndexableImage> &&
                concepts::OutputImage<archetypes::OutputIndexableImage>);
  static_assert(not concepts::BidirectionalImage<archetypes::OutputAccessibleImage> &&
                concepts::OutputImage<archetypes::OutputAccessibleImage>);
  static_assert(concepts::BidirectionalImage<archetypes::OutputBidirectionalImage> &&
                concepts::OutputImage<archetypes::OutputBidirectionalImage>);
  static_assert(concepts::BidirectionalImage<archetypes::OutputRawImage> &&
                concepts::OutputImage<archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputRawImage)
{
  static_assert(not concepts::RawImage<archetypes::Image> && not concepts::OutputImage<archetypes::Image>);
  static_assert(not concepts::RawImage<archetypes::ForwardImage> &&
                not concepts::OutputImage<archetypes::ForwardImage>);
  static_assert(not concepts::RawImage<archetypes::InputImage> && not concepts::OutputImage<archetypes::InputImage>);
  static_assert(not concepts::RawImage<archetypes::IndexableImage> &&
                not concepts::OutputImage<archetypes::IndexableImage>);
  static_assert(not concepts::RawImage<archetypes::AccessibleImage> &&
                not concepts::OutputImage<archetypes::AccessibleImage>);
  static_assert(not concepts::RawImage<archetypes::BidirectionalImage> &&
                not concepts::OutputImage<archetypes::BidirectionalImage>);
  static_assert(concepts::RawImage<archetypes::RawImage> && not concepts::OutputImage<archetypes::RawImage>);

  static_assert(not concepts::RawImage<archetypes::OutputImage> && concepts::OutputImage<archetypes::OutputImage>);
  static_assert(not concepts::RawImage<archetypes::OutputForwardImage> &&
                concepts::OutputImage<archetypes::OutputForwardImage>);
  static_assert(not concepts::RawImage<archetypes::OutputIndexableImage> &&
                concepts::OutputImage<archetypes::OutputIndexableImage>);
  static_assert(not concepts::RawImage<archetypes::OutputAccessibleImage> &&
                concepts::OutputImage<archetypes::OutputAccessibleImage>);
  static_assert(not concepts::RawImage<archetypes::OutputBidirectionalImage> &&
                concepts::OutputImage<archetypes::OutputBidirectionalImage>);
  static_assert(concepts::RawImage<archetypes::OutputRawImage> && concepts::OutputImage<archetypes::OutputRawImage>);
}
