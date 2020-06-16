from conans import CMake, ConanFile, tools


class Pylene(ConanFile):
    name = "pylene"
    version = "head"
    license = "https://gitlab.lrde.epita.fr/olena/pylene/blob/dev/LICENSE"
    url = "https://gitlab.lrde.epita.fr/olena/pylene"
    description = "C++ Generic Image Processing Library."
    settings = "os", "compiler", "arch", "build_type"
    options = {
               "shared": [True, False],
               "fPIC": [True, False],
               "freeimage": [True, False],
               "boost": [True, False]}
    default_options = {
                       "shared": False,
                       "fPIC": False,
                       "freeimage": False,
                       "boost": False,
                       "gtest:shared": False
    }

    generators = [ "cmake", "cmake_paths", "cmake_find_package" ]
    exports_sources = ["pylene/*", "cmake/*", "CMakeLists.txt", "LICENSE"]


    def build(self):
        cmake = CMake(self)
        cmake.definitions["PYLENE_BUILD_LIBS_ONLY"] = "YES"
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*", dst="", src="cmake")

    def package_info(self):
        if self.settings.compiler in ["gcc", "clang"]:
            self.cpp_info.cppflags = ["-std=c++20"]

    # developer dependancies (to be removed)
    def build_requirements(self):
        self.build_requires("gtest/[>=1.10]", force_host_context=True)
        self.build_requires("benchmark/[>=1.5.0]", force_host_context=True)


    # Requirements part of the INTERFACE
    def requirements(self):
        self.requires("range-v3/0.10.0@ericniebler/stable")
        self.requires("fmt/6.0.0")


        if self.options.freeimage:
            self.requires("freeimage/3.18.0@dutiona/stable")

        if self.options.boost:
            self.requires("boost/1.69.0@conan/stable")
