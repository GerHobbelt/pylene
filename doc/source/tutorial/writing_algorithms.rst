Writing algorithms
==================

Non-generic implementation
**************************

Even if Pylene has algorithms written in a generic way as much as possible, you do **not** have to write generic
algorithms yourself if it is unnecessary (nevertheless, we will see that it is not that difficult to do so).

Working on 2D/3D images
-----------------------

You can always use two/three nested loops to iterate over all positions in the domain. You can access the value
of an :cpp:any:`mln::ndimage` at a given position `p=(x,y)` or `p=(x,y,z)` with ``ima({x,y})`` or ``ima({x,y,z})``.


::

   auto roi = ima.domain();
   int x0 = roi.x();
   int y0 = roi.y();
   for (int y = 0; y < roi.height(); ++y)
     for (int x = 0; x < roi.width(); ++x)
       ima({x0 + x, y0 + y});






Reference semantic and passing images to functions
--------------------------------------------------

All images in Pylene have reference semantics. It means that the copy/move constructor and assignment operators do not
create real copies but alias the same image::

  mln::image2d<int> A = {{1,2,3},{4,5,6}};
  mln::image2d<int> B = A; // B aliases A
  B({0,0}) = 42;           // Writes on B, write on A
  assert(A({0,0}) == 42);


Therefore, copying image (in the C++ sense) are cheap and incur low overhead. Thus, images **should be passed by copy to
functions** and not by const-reference.


.. note::

   A good function signature for passing images::

     void foo(mln::image2d<uint8_t> input, mln::image<uint8_t> out)

   Bad::

     void foo(const mln::image2d<uint8_t>& input, mln::image<uint8_t>& out)


Algorithms with local information
---------------------------------

Many algorithms (especially in Mathematical Morphology) have to work with neighborhood that defines a connection between
pixels. For example, in 2D, you may want to use 4-connectivity or 8-connectivity for connected component labeling.


:ref:doc:`Neighborhoods and structuring elements </core/neighborhood>` are range generators. The expression `nbh(x)`
generates a set of points from an anchor (a point) `x`. The following snippet visits all :ref:doc:`4-connected
</core/neighborhood/c4>` neighbors of all points of an image::

  for (int y = 0; y < roi.height(); ++y)
     for (int x = 0; x < roi.width(); ++x)
       for (mln::point2d n : mln::c4({x,y}))
         if (n.x() >= 0 && n.x() < roi.width() && n.y() >= 0 && n.y() < roi.height())
           // Do something with n


Generic implementation
**********************

Traversing an image in a generic ways, i.e. that is actually independant on the number of coordinates, is actually
simpler than the hand-made loop. Most images provide range-based facilities to allow to iterate over the
points (site coordinates) or the values of an image (or both).

* `ima.domain()` returns a range of point
* `ima.values()` returns the range of values of the image
* `ima.pixels()` returns the range of pixels where a pixel is simply a pair *(point,value)* accessible with `px.point()`
  and `px.val()`.


Summing the values of an image with point-access::

   int sum = 0;
   mln_foreach(auto p, ima.domain())
     sum += ima(p);

with values::

   int sum = 0;
   mln_foreach(uint8_t v, ima.values())
     sum += v;


with pixels::

   int sum = 0;
   mln_foreach(auto px, ima.pixels())
     sum += px.val();

.. important::

   To iterate over the values of a range in Pylene, you have to use  ``mln_foreach(auto x, rng)`` which has the same
   semantics as ``for(auto x : rng)`` but supports :ref:doc:`multi-dimensional ranges </core/ranges>`.


Neighborhoods and structuring elements in a generic context
-----------------------------------------------------------

Using neighborhood in a generic context is actually easier than in a non-generic one. Compare the following
implementation with the previous one that visit every neighbor of all point in region. The following code is able
to work witn any compatible neighborhood (ie. 4-connectivity or 8-connectivity in 2D, 6-/26-connectivity in 3D...). Yet
the code is simpler, more resuable and just as efficient::

  auto roi = ima.domain();
  mln_foreach(auto p, roi)
    for (auto n : nbh(p))
      if (roi.has(n))
        // Do something with n




Accessing multiple images
-------------------------

The straightforward way to access values from multiple is to iterate over a domain and to use direct image access::

  mln_foreach(auto p, domain)
    // Use f(p) and f(g)



However, it might be more efficent to avoid a *point to index* computation at each turn. Range views enable to pack
(zip) several objects in a single object and iterate in a more efficient way::

  auto vals_1 = f.values();
  auto vals_2 = g.values();
  mln_foreach((auto [v1, v2]), mln::ranges::view::zip(vals_1, vals_2))
    ...


Generic signature and concept checking
--------------------------------------

A simple but not optimal generic implementation of a dilation would look like::


  template <class I, class SE, class J>
  requires (mln::concepts::Image<I> &&
            mln::concepts::OutputImage<J> &&
            mln::concepts::StructuringElement<SE, image_point_t<I>> &&
            std::same_as<image_point_t<I>, image_point_t<J>> &&
            std::convertible_to<image_value_t<I>, image_value_t<J>>)
  void dilation(I input, SE se, J out)
  {
    auto roi = ima.domain();
    mln_foreach(auto p, roi)
    {
      auto m = input(p);
      for (auto n : se(p))
        if (roi.has(n))
          m = std::max(m, input(n))
      out(p) = m;
    }
  }


Writing parallel algorithms
---------------------------

See :ref:doc:`Writing parallel algorithms </tutorial/parallel_algorithms>`