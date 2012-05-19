%module mln
%{
  #define SWIG_FILE_WITH_INIT
  #include <mln/core/ndimage.hpp>
%}

%include "numpy.i"
%init %{
  import_array();
%}

%define(in) ndimage_tmap(ndim, type)
  %typemap(in) ndimage<ndim



%template(image2du8) image2d<uint8>
%template(image2du16) image2d<uint16>
%template(image2di8) image2d<int8>
%template(image2di16) image2d<int16>
