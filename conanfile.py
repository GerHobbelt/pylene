from conans import ConanFile, tools
import os
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.layout import cmake_layout
from conans.errors import ConanInvalidConfiguration

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
        "gtest:shared": False,
        "boost:header_only": True,
    }

    generators = "CMakeDeps"
    exports_sources = ["pylene/*", "pylene-python/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

    build_requires = [
        "gtest/[>=1.11.0]",
        "benchmark/[>=1.5.0]",
    ]

    requires = [
        "range-v3/0.10.0",
        "fmt/6.0.0",
        "tbb/2020.0",
        "xsimd/7.4.6",
        "eigen/3.3.9",
        "boost/1.75.0",
        "cfitsio/4.0.0"
    ]

    def _build_python(self):
        return self.options.shared or self.options.fPIC or tools.os_info.is_windows

    def _check_configuration(self):
        tools.check_min_cppstd(self, "20")
        if self.settings.compiler in ["gcc", "clang"] and tools.Version(self.settings.compiler.version) < 10:
            raise ConanInvalidConfiguration("Invalid compiler version for {} (Got {}, should be greater or equal to 10)".format(self.settings.compiler, self.settings.compiler.version))

    def configure(self):
        self._check_configuration()
        if self.options.shared:
            self.options.fPIC = True
        if self._build_python():
            self.build_requires.append("pybind11/2.6.2")

    def generate(self):
        cmake = CMakeDeps(self)
        cmake.generate()
        tc = CMakeToolchain(self)
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

        if self._build_python():
            self.cpp.package.components["pylene-numpy"].libs = ["Pylene-numpy"]
            self.cpp.package.components["pylene-numpy"].libdirs = ["lib"]
            self.cpp.package.components["pylene-numpy"].includedirs = ["include"]
            self.cpp.source.components["pylene-numpy"].includedirs = ["pylene-python/include"]
            self.cpp.build.components["pylene-numpy"].libdirs = ["pylene-python"]


    def build(self):
        variables = { "PYLENE_BUILD_LIBS_ONLY" : "YES" } # Controls what is built, not the toolchain
        if self._build_python():
            variables["PYLENE_BUILD_PYTHON"] = "YES"
        else:
            self.output.warn("fPIC disabled. Skipping python bindings build...")
        cmake = CMake(self)
        cmake.configure(variables)
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("FindFreeImage.cmake", dst="cmake", src="cmake")
        tools.rmdir(os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.builddirs = ["cmake"]
        self.cpp_info.set_property("cmake_target_name", "pylene::pylene")


        # Core component
        self.cpp_info.components["core"].requires = ["range-v3::range-v3", "fmt::fmt", "tbb::tbb", "xsimd::xsimd", "boost::headers"]
        self.cpp_info.components["core"].libs = ["Pylene-core"]

        # Scribo component
        self.cpp_info.components["scribo"].requires = ["core", "eigen::eigen3"]
        self.cpp_info.components["scribo"].libs = ["Pylene-scribo"]

        # IO component (FreeImage)
        self.cpp_info.components["io-freeimage"].system_libs.append("freeimage")
        self.cpp_info.components["io-freeimage"].requires = ["core"]
        self.cpp_info.components["io-freeimage"].libs = ["Pylene-io-freeimage"]

        # IO component (cfitsio)
        self.cpp_info.components["io-fits"].requires = ["core", "cfitsio::cfitsio"]
        self.cpp_info.components["io-fits"].libs = ["Pylene-io-fits"]


        # Pylene-numpy component
        if self._build_python():
            self.cpp_info.components["pylene-numpy"].requires = ["core"]
            self.cpp_info.components["pylene-numpy"].libs = ["Pylene-numpy"]

        v = tools.Version(self.settings.compiler.version)
        for comp in self.cpp_info.components:
            self.cpp_info.components[comp].cxxflags.append(tools.cppstd_flag(self.settings))
            if self.settings.compiler == "gcc" and v.major == "9":
                self.cpp_info.components[comp].cxxflags.append("-fconcepts")

    def package_id(self):
        del self.info.settings.compiler.cppstd
