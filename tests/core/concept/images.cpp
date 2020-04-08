#include <mln/core/concept/archetype/image.hpp>
#include <mln/core/concept/images.hpp>

#include <gtest/gtest.h>

#include <tuple>
#include <type_traits>


TEST(Core, Concept_Image)
{
  static_assert(mln::concepts::Image<mln::archetypes::Image>);
  static_assert(mln::concepts::Image<mln::archetypes::ForwardImage>);
  static_assert(mln::concepts::Image<mln::archetypes::InputImage>);
  static_assert(mln::concepts::Image<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::Image<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::Image<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::Image<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::Image<mln::archetypes::RawImage>);
  static_assert(mln::concepts::Image<mln::archetypes::ConcreteImage>);
  static_assert(mln::concepts::Image<mln::archetypes::ViewImage>);
}

TEST(Core, Concept_ForwardImage)
{
  static_assert(mln::concepts::ForwardImage<mln::archetypes::Image>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::ForwardImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::InputImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::RawImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::ConcreteImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::ViewImage>);
}

TEST(Core, Concept_InputImage)
{
  static_assert(mln::concepts::InputImage<mln::archetypes::Image>);
  static_assert(mln::concepts::InputImage<mln::archetypes::ForwardImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::InputImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::RawImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::ConcreteImage>);
  static_assert(mln::concepts::InputImage<mln::archetypes::ViewImage>);
}

TEST(Core, Concept_IndexableImage)
{
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::InputImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::RawImage>);
}

TEST(Core, Concept_AccessibleImage)
{
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::RawImage>);
}

TEST(Core, Concept_IndexableAndAccessibleImage)
{
  static_assert(not mln::concepts::IndexableAndAccessibleImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<mln::archetypes::RawImage>);
}

TEST(Core, Concept_BidirectionalImage)
{
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::AccessibleImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::BidirectionalImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::BidirectionalImage<mln::archetypes::RawImage>);
}

TEST(Core, Concept_RawImage)
{
  static_assert(not mln::concepts::RawImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::AccessibleImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::RawImage<mln::archetypes::RawImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::ConcreteImage>);
  static_assert(mln::concepts::ForwardImage<mln::archetypes::ViewImage>);
}

TEST(Core, Concept_WithExtensionImage)
{
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::AccessibleImage>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::BidirectionalImage>);
  static_assert(not mln::concepts::WithExtensionImage<mln::archetypes::RawImage>);
  static_assert(mln::concepts::WithExtensionImage<mln::archetypes::WithExtensionImage>);
}

TEST(Core, Concept_OutputImage) // ForwardImage, InputImage
{
  static_assert(mln::concepts::Image<mln::archetypes::Image> && not mln::concepts::OutputImage<mln::archetypes::Image>);
  static_assert(mln::concepts::Image<mln::archetypes::ForwardImage> &&
                not mln::concepts::OutputImage<mln::archetypes::ForwardImage>);
  static_assert(mln::concepts::Image<mln::archetypes::InputImage> &&
                not mln::concepts::OutputImage<mln::archetypes::InputImage>);
  static_assert(mln::concepts::Image<mln::archetypes::IndexableImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::Image<mln::archetypes::AccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::Image<mln::archetypes::IndexableAndAccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::Image<mln::archetypes::BidirectionalImage> &&
                not mln::concepts::OutputImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::Image<mln::archetypes::RawImage> &&
                not mln::concepts::OutputImage<mln::archetypes::RawImage>);

  static_assert(mln::concepts::Image<mln::archetypes::OutputImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputImage>);
  static_assert(mln::concepts::Image<mln::archetypes::OutputForwardImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputForwardImage>);
  static_assert(mln::concepts::Image<mln::archetypes::OutputIndexableImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableImage>);
  static_assert(mln::concepts::Image<mln::archetypes::OutputAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputAccessibleImage>);
  static_assert(mln::concepts::Image<mln::archetypes::OutputIndexableAndAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableAndAccessibleImage>);
  static_assert(mln::concepts::Image<mln::archetypes::OutputBidirectionalImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputBidirectionalImage>);
  static_assert(mln::concepts::Image<mln::archetypes::OutputRawImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputIndexableImage)
{
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::Image> &&
                not mln::concepts::OutputImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::ForwardImage> &&
                not mln::concepts::OutputImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::InputImage> &&
                not mln::concepts::OutputImage<mln::archetypes::InputImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::IndexableImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::AccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::IndexableAndAccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::BidirectionalImage> &&
                not mln::concepts::OutputImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::RawImage> &&
                not mln::concepts::OutputImage<mln::archetypes::RawImage>);

  static_assert(not mln::concepts::IndexableImage<mln::archetypes::OutputImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::OutputForwardImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputForwardImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::OutputIndexableImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::OutputAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputAccessibleImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::OutputIndexableAndAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableAndAccessibleImage>);
  static_assert(not mln::concepts::IndexableImage<mln::archetypes::OutputBidirectionalImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputBidirectionalImage>);
  static_assert(mln::concepts::IndexableImage<mln::archetypes::OutputRawImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputAccessibleImage)
{
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::Image> &&
                not mln::concepts::OutputImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::ForwardImage> &&
                not mln::concepts::OutputImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::InputImage> &&
                not mln::concepts::OutputImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::IndexableImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::AccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::IndexableAndAccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::BidirectionalImage> &&
                not mln::concepts::OutputImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::RawImage> &&
                not mln::concepts::OutputImage<mln::archetypes::RawImage>);

  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::OutputImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::OutputForwardImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputForwardImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::OutputIndexableImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::OutputAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputAccessibleImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::OutputIndexableAndAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableAndAccessibleImage>);
  static_assert(not mln::concepts::AccessibleImage<mln::archetypes::OutputBidirectionalImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputBidirectionalImage>);
  static_assert(mln::concepts::AccessibleImage<mln::archetypes::OutputRawImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputBidirectionalImage)
{
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::Image> &&
                not mln::concepts::OutputImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::ForwardImage> &&
                not mln::concepts::OutputImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::InputImage> &&
                not mln::concepts::OutputImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::IndexableImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::AccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::AccessibleImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::IndexableAndAccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::BidirectionalImage<mln::archetypes::BidirectionalImage> &&
                not mln::concepts::OutputImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::BidirectionalImage<mln::archetypes::RawImage> &&
                not mln::concepts::OutputImage<mln::archetypes::RawImage>);

  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::OutputImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::OutputForwardImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputForwardImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::OutputIndexableImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::OutputAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputAccessibleImage>);
  static_assert(not mln::concepts::BidirectionalImage<mln::archetypes::OutputIndexableAndAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableAndAccessibleImage>);
  static_assert(mln::concepts::BidirectionalImage<mln::archetypes::OutputBidirectionalImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputBidirectionalImage>);
  static_assert(mln::concepts::BidirectionalImage<mln::archetypes::OutputRawImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputRawImage>);
}

TEST(Core, Concept_OutputRawImage)
{
  static_assert(not mln::concepts::RawImage<mln::archetypes::Image> &&
                not mln::concepts::OutputImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::ForwardImage> &&
                not mln::concepts::OutputImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::InputImage> &&
                not mln::concepts::OutputImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::IndexableImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::AccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::AccessibleImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::IndexableAndAccessibleImage> &&
                not mln::concepts::OutputImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::BidirectionalImage> &&
                not mln::concepts::OutputImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::RawImage<mln::archetypes::RawImage> &&
                not mln::concepts::OutputImage<mln::archetypes::RawImage>);

  static_assert(not mln::concepts::RawImage<mln::archetypes::OutputImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::OutputForwardImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputForwardImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::OutputIndexableImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::OutputAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputAccessibleImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::OutputIndexableAndAccessibleImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputIndexableAndAccessibleImage>);
  static_assert(not mln::concepts::RawImage<mln::archetypes::OutputBidirectionalImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputBidirectionalImage>);
  static_assert(mln::concepts::RawImage<mln::archetypes::OutputRawImage> &&
                mln::concepts::OutputImage<mln::archetypes::OutputRawImage>);
}

TEST(Core, Concept_ConcreteImage)
{
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::Image>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::ForwardImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::InputImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::IndexableImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::AccessibleImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::BidirectionalImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::RawImage>);
  static_assert(mln::concepts::ConcreteImage<mln::archetypes::ConcreteImage>);
  static_assert(not mln::concepts::ConcreteImage<mln::archetypes::ViewImage>);
}

TEST(Core, Concept_ViewImage)
{
  static_assert(mln::concepts::ViewImage<mln::archetypes::Image>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::ForwardImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::InputImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::IndexableImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::AccessibleImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::IndexableAndAccessibleImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::BidirectionalImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::RawImage>);
  static_assert(not mln::concepts::ViewImage<mln::archetypes::ConcreteImage>);
  static_assert(mln::concepts::ViewImage<mln::archetypes::ViewImage>);
}
