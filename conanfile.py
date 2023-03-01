from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.scm import Version
from conan.tools.files import rmdir
from conans.errors import ConanInvalidConfiguration

import os

class Pylene(ConanFile):
    name = "pylene"
    version = "head"
    license = "MPL v2"
    url = "https://gitlab.lrde.epita.fr/olena/pylene"
    description = "C++ Generic Image Processing Library."
    settings = "os", "compiler", "arch", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": False,
        "gtest/*:shared": False,
        "boost/*:header_only": True,
        "freeimage/*:shared": True
    }

    exports_sources = ["pylene/*", "pylene-python/*", "cmake/*", "CMakeLists.txt", "LICENSE"]


    requires = [
        "range-v3/0.12.0",
        "fmt/9.0.0",
        "onetbb/2021.7.0",
        "xsimd/7.4.6",
        "eigen/3.3.9",
        "boost/1.75.0",
        "cfitsio/4.1.0@lrde/stable",
        "freeimage/3.18.0@lrde/stable"
    ]

    def build_requirements(self):
        self.test_requires("gtest/1.11.0")
        self.test_requires("benchmark/[>=1.5.0]")

    def requirements(self):
        self.requires("zlib/1.2.13", override=True)
        

    def _check_configuration(self):
        check_min_cppstd(self, "20")
        if self.settings.compiler in ["gcc", "clang"] and Version(self.settings.compiler.version) < 10:
            raise ConanInvalidConfiguration("Invalid compiler version for {} (Got {}, should be greater or equal to 10)".format(self.settings.compiler, self.settings.compiler.version))

    def configure(self):
        self._check_configuration()
        if self.options.shared:
            self.options.fPIC = True

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

    def layout(self):
        self.folders.source = "."
        self.folders.build = "build"
        self.folders.generators = "build"
        # Core component
        self.cpp.package.components["core"].libs = ["Pylene-core"]
        self.cpp.package.components["core"].libdirs = ["lib"]
        self.cpp.package.components["core"].includedirs = ["include"]
        self.cpp.source.components["core"].includedirs = ["pylene/include"]
        self.cpp.build.components["core"].libdirs = ["pylene"]

        # Scribo component
        self.cpp.package.components["scribo"].libs = ["Pylene-scribo"]
        self.cpp.package.components["scribo"].libdirs = ["lib"]
        self.cpp.package.components["scribo"].includedirs = ["include"]
        self.cpp.source.components["scribo"].includedirs = ["pylene/include"]
        self.cpp.build.components["scribo"].libdirs = ["pylene"]


        # IO component (FreeImage)
        self.cpp.package.components["io-freeimage"].libs = ["Pylene-io-freeimage"]
        self.cpp.package.components["io-freeimage"].libdirs = ["lib"]
        self.cpp.package.components["io-freeimage"].includedirs = ["include"]
        self.cpp.source.components["io-freeimage"].includedirs = ["pylene/include"]
        self.cpp.build.components["io-freeimage"].libdirs = ["pylene"]


        # IO component (cfitsio)
        self.cpp.package.components["io-fits"].libs = ["Pylene-io-fits"]
        self.cpp.package.components["io-fits"].libdirs = ["lib"]
        self.cpp.package.components["io-fits"].includedirs = ["include"]
        self.cpp.source.components["io-fits"].includedirs = ["pylene/include"]
        self.cpp.build.components["io-fits"].libdirs = ["pylene"]

    def build(self):
        variables = { "PYLENE_BUILD_LIBS_ONLY" : "YES" } # Controls what is built, not the toolchain
        cmake = CMake(self)
        cmake.configure(variables)
        cmake.build()
        cmake.install()

    def package(self):
        rmdir(os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.builddirs = ["cmake"]
        self.cpp_info.set_property("cmake_target_name", "pylene::pylene")

        # Core component
        self.cpp_info.components["core"].requires = ["range-v3::range-v3", "fmt::fmt", "tbb::tbb", "xsimd::xsimd", "boost::headers"]
        self.cpp_info.components["core"].libs = ["Pylene-core"]

        # Scribo component
        self.cpp_info.components["scribo"].requires = ["core", "eigen::eigen3"]
        self.cpp_info.components["scribo"].libs = ["Pylene-scribo"]
        self.cpp_info.components["scribo"].includedirs = ["include"]

        # IO component (FreeImage)
        self.cpp_info.components["io-freeimage"].requires = ["core", "freeimage::FreeImage"]
        self.cpp_info.components["io-freeimage"].libs = ["Pylene-io-freeimage"]

        # IO component (cfitsio)
        self.cpp_info.components["io-fits"].requires = ["core", "cfitsio::cfitsio"]
        self.cpp_info.components["io-fits"].libs = ["Pylene-io-fits"]

    def imports(self):
        self.copy("*.dll", src="bin", dst="build")
