Install Pylene
###############

.. contents::





Install (for users)
===================



Install from sources
********************


Install with conan (preferred)
******************************





Integration in another project
===============================



Integration with conan
**********************


Integration with CMake External Project
***************************************




Integration with Git Submodule
******************************


Integration as a subfolder
**************************




Install (for developers)
========================

#. Clone the repository
    .. code-block:: shell

        git clone git@gitlab.lrde.epita.fr:olena/pylene.git
#. Install developer dependencies:
    * Google Test
    * Google Benchmark (by Conan)
    * cmslt2 (by Conan)
    * range v3 (by Conan)
    * FreeImage
    * Boost
    * Python with Sphinx, Matplotlib, Numpy (for the documentation)
    * Conan
#. Use *conan* to install the dependencies not provided by your system.
    This is advised to use the followings options::

        benchmark                       = True
        freeimage                       = False
        gtest                           = False
        boost_program_options           = False
        benchmark:shared                = True
        gtest:shared                    = True

    In the source directory, use:

    .. code-block:: shell

        mkdir build && cd build
        conan install ..
        cmake ..
        cmake --build .














