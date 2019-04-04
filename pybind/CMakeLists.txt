project(Pybind CXX)

find_package(pybind11 REQUIRED)

pybind11_add_module(PyPylene MODULE 
                    src/binding.cpp
                    src/numpy_helper.cpp
                    )

target_link_libraries(PyPylene PRIVATE Pylene::Pylene)
target_include_directories(PyPylene PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>)
target_compile_features(PyPylene PRIVATE cxx_std_17)

# enable_testing()
