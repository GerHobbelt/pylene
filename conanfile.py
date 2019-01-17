from conans import CMake, ConanFile, tools


class Pylene(ConanFile):
    name = "pylene"
    version = "2.0"
    license = "https://gitlab.lrde.epita.fr/olena/pylene/blob/dev/LICENSE"
    url = "https://gitlab.lrde.epita.fr/olena/pylene"
    description = "C++ Generic Image Processing Library."
    settings = "os", "compiler", "arch", "build_type", "cppstd"
    options = {"gtest": [True, False], "benchmark": [True, False], "freeimage": [
        True, False], "boost_program_options": [True, False]}
    default_options = "gtest=False", "benchmark=False", "freeimage=False", "boost_program_options=False"
    generators = "cmake_paths"
    exports_sources = ["pylene/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

    def get_cmake_config(self):
        cmake = CMake(self)
        cmake.definitions["PYLENE_BUILD_LIBS_ONLY"] = "YES"
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.get_cmake_config()
        cmake.build()

    def package(self):
        cmake = self.get_cmake_config()
        cmake.install()
        self.copy("*", dst="", src="cmake")

    def package_info(self):
        if self.settings.compiler in ["gcc", "clang"]:
            self.cpp_info.cppflags = ["-std=c++17"]

    # Requirements part of the INTERFACE
    def requirements(self):
        self.requires("range-v3/0.4.0@ericniebler/stable")
        self.requires("cmcstl2/0.1@dutiona/testing")

        if self.options.freeimage:
            self.requires("freeimage/3.18.0@dutiona/stable")

        if self.options.boost_program_options:
            self.requires("boost_program_options/1.66.0@bincrafters/stable")

        if self.options.gtest:
            self.requires("gtest/1.8.1@bincrafters/stable")

        if self.options.benchmark:
            self.requires("benchmark/head@dutiona/stable")