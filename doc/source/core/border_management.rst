Border Management
#################

Role
====

The borer manager is a tool for the user to input the the border should be handled during the execution of an algorithm.
There are 2 main handling policies: *Native* and *Auto*.


Policies
========

For both the border manager will check wether the image is already supplied with a border whose width is large enough to handle the given structuring element.

* Native:
  
  The border is large enough: forward the image as-is to the algorithm to allow the fastest access possible.
  The border is not large enough: the border manager fails and halt the program.

* Auto:

  The border is large enough: forward the image as-is to the algorithm to allow the fastest access possible.
  The border is not large enough: decorate the image with a view whose extension will emulate what is required by the algorithm with the given structuring element.

It is important to note that some border method are not available for both policies.
The default policy is *Auto*.

Methods
=======

None
^^^^

Enforce a policy where there is no border to use. The method cannot fail as it enforces the border to vanish.
To enforce this method, the border manager decorate the structuring element in a view to check the domain inclusion of each neighbor point.

TODO: image illustration

Usage example::

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::none();

  local_algorithm(ima, disc_se, bm); // will handle the border for you


Fill
^^^^

Will fill the border with a specific value.

TODO: image illustration

Usage example::

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::fill(0); // fill border with 0 with policy auto

  local_algorithm(ima, disc_se, bm); // will handle the border for you


Mirror
^^^^^^

Will mirror the image following an axial symetry on each border. Can be given a padding to skip a few pixels on the first mirroring.

TODO: image illustration

Usage example::

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::mirror(0); // mirror border with no padding with policy auto

  local_algorithm(ima, disc_se, bm); // will handle the border for you


Periodize
^^^^^^^^^

Replicate the image like it is a mozaic.

TODO: image illustration

Usage example::

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::periodize(); // periodize border with policy auto

  local_algorithm(ima, disc_se, bm); // will handle the border for you


Clamp
^^^^^

Expand the value already on the border of the image.

TODO: image illustration

Usage example::

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::clamp(); // clamp border with policy auto

  local_algorithm(ima, disc_se, bm); // will handle the border for you


Image
^^^^^

Assume all points out of the current image's domain are to be picked inside another image.
A basic use-case is preparing tiles from a large image.
The position of the image can be offsetted in the image which will serve as extension for usage simplicity.

TODO: image illustration

Usage example::

  auto ima_base = mln::image2d<int>(7, 7); // image 7x7
  mln::fill(ima_base, 42);

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::image(ima_base, {2, 2}); // image border offsetted by 2x2 with policy auto

  local_algorithm(ima, disc_se, bm); // will handle the border for you


User
^^^^

Assume the user knows what he is doing and do not touch nor decorate the given image in any way.
Both policies lead to the same behavior: check wether the structuring element fit and then forward the image as-is if it fits. An exception is raised if it does not.

TODO: image illustration

Usage example::

  mln::image2d<int> ima = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}; // default border width is 3
  auto disc_se = mln::se::disc{1}; // radius is 1
  auto bm = mln::extension::bm::user(); // user border with policy auto

  local_algorithm(ima, disc_se, bm); // will handle the border for you



**Policy change:**

For all the above examples, all that is required to use the native policy instead of the default one (auto) is to change::

  auto bm = mln::extension::bm::method();

by::

  auto bm = mln::extension::bm::native::method();


Internals
=========

When writting your own algorithm, it is important to note what are the services offered by the border manager and how to use them.
Two services are privided by the border manager: `bm.manage(ima, se)` and `bm.prepare_output_image(ima, se, roi)`.

Manage
^^^^^^

`bm.manage(ima, se)` will return a pair of 2 variants. Those variants contain the prepared and possibly decorated image and structuring element according to the policy and method given by the user.
To use the resulting pair, all that is needed is to visit the variant. The standard way to it is::

  template <class Ima, class SE, BorderManagementMethod Method, BorderManagementPolicy Policy>
  auto my_local_algo(Ima ima, SE se, mln::extension::border_manager<Method, Policy> bm)
  {

    // ...

    auto [managed_ima, managed_se] = bm.manage(ima, se);
    
    return std::visit([](auto ima_, auto se_) 
      {
        // algorithm ...
        for(auto&& px : ima_.pixels())
        {
          for(auto&& nb : se_(px))
            tmp_px = use(px, nb);
          px.val() = use(px, tmp_px);
        } 
        // ...
      }, managed_ima, managed_se);
  }

TODO illustration

Prepare output image
^^^^^^^^^^^^^^^^^^^^

`bm.prepare_output_image(ima, se, roi)` is intended to be used when one wants, for different reasons, write a value inside the border during the execution of the algorithm.
Henceforth requiring a temporary image designed to be written in.
This is especially useful when optimizing algorithm with properties on structuring elements such as decomposability or separability.
Inded, the decomposed structuring elements can lead to the first passes of the algorithm writting inside the border for the following passes.

The way to use it is::

  template <class Ima, class SE, BorderManagementMethod Method, BorderManagementPolicy Policy>
  auto my_local_algo(Ima input_ima, SE se, mln::extension::border_manager<Method, Policy> bm)
  {

    // ...
    if constexpr(SE::decomposable::value && mln::is_se_compatible_with_v<SE, D>)
    {
      if (se.is_decomposable())
      {
        auto ima_tmp = bm.prepare_output_image(ima, se, (ima.domain());
        auto dec_ses = se.decompose();
        for(auto&& se : dec_ses)
        {
          for(auto&& px : input_ima.pixels())
            for(auto&& nb : se(px))
              tmp_px = use(px, nb);
          output_ima[px.point()] = use(px, tmp_px);
        }
      }
    }
  }

TODO illustration
