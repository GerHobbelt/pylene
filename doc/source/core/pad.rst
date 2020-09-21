Padding
#######


Include :file:`<mln/core/extension/padding.hpp>`

.. cpp:namespace:: mln


These functions enable to pad a rectangular buffer or an image either:

* inplace: the padding region is defined by some amount of border pixels
* in an output buffer: the region is copied and the missing values are filled






The following strategies 

  /// PAD_ZERO
  ///   pad each dimension with value zero `[0 0 1 2 3 0 0 0 0]`
  /// PAD_CONSTANT
  ///   pad each dimension with \p value `[9 9 1 2 3 9 9 9 9]`
  /// PAD_WRAP
  ///   pad each dimension using periodization `[2 3 1 2 3 1 2 3 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge mirroring `[2 1 1 2 3 3 2 1 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge replication `[1 1 1 2 3 3 3 3 3]`


Inplace padding
---------------

Pad a rectangular image or a buffer defined by 
