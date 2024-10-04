from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.files import rmdir
from conans.errors import ConanInvalidConfiguration

import os

required_conan_version = ">=2.0.1"

class Pylene(ConanFile):
    name = "pylene"
    version = "head"
    license = "MPL v2"
    url = "https://gitlab.lre.epita.fr/olena/pylene"
    description = "C++ Generic Image Processing Library."
    settings = "os", "compiler", "arch", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": False,
        "boost/*:header_only": True,
        "freeimage/*:shared": True
    }
    package_type = "library"

    exports_sources = ["pylene/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

    def build_requirements(self):
        self.test_requires("gtest/[^1.12]", options={"shared": True})
        self.test_requires("benchmark/[^1.7]", options={"shared": True})

    def requirements(self):
        # Private dependencies
        self.requires("freeimage/3.19.7@lrde")
        self.requires("cfitsio/4.2.0")
        self.requires("onetbb/2021.7.0")
        self.requires("eigen/3.4.0")

        # Public dependencies
        self.requires("range-v3/0.12.0", transitive_headers=True)
        self.requires("fmt/11.0.2", transitive_headers=True, transitive_libs=True)
        self.requires("xsimd/7.4.6", transitive_headers=True)
        self.requires("boost/1.81.0", transitive_headers=True)

    def _check_configuration(self):
        check_min_cppstd(self, "20")
        
        if self.settings.os == "Linux":
            accepted_compilers = ["gcc", "clang"]
            accepted_compilers_dict = {
                "gcc": [str(i) for i in range(10, 15)],
                "clang": [str(i) for i in range(11, 19)]
            }
            if not str(self.settings.compiler) in accepted_compilers or not str(self.settings.compiler.version) in accepted_compilers_dict[str(self.settings.compiler)]:
                accepted_e = [f"{comp}-{ver}" for comp in accepted_compilers for ver in accepted_compilers_dict[comp]]
                raise ConanInvalidConfiguration(f"Compiler {self.settings.compiler} version {self.settings.compiler} not handled on Linux (Accepted: {accepted_e})")



    def validate(self):
        self._check_configuration()

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

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

    def package(self):
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.builddirs = ["cmake"]
        self.cpp_info.set_property("cmake_target_name", "pylene::pylene")

        # Core component
        self.cpp_info.components["core"].requires = ["range-v3::range-v3", "fmt::fmt", "onetbb::onetbb", "xsimd::xsimd", "boost::headers"]
        self.cpp_info.components["core"].libs = ["Pylene-core"]
        self.cpp_info.components["core"].includedirs = ["include"]

        # Scribo component
        self.cpp_info.components["scribo"].requires = ["core", "eigen::eigen3"]
        self.cpp_info.components["scribo"].libs = ["Pylene-scribo"]
        self.cpp_info.components["scribo"].includedirs = ["include"]

        # IO component (FreeImage)
        self.cpp_info.components["io-freeimage"].requires = ["core", "freeimage::FreeImage"]
        self.cpp_info.components["io-freeimage"].libs = ["Pylene-io-freeimage"]
        self.cpp_info.components["io-freeimage"].includedirs = ["include"]

        # IO component (cfitsio)
        self.cpp_info.components["io-fits"].requires = ["core", "cfitsio::cfitsio"]
        self.cpp_info.components["io-fits"].libs = ["Pylene-io-fits"]
        self.cpp_info.components["io-fits"].includedirs = ["include"]
