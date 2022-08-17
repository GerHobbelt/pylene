from conans import ConanFile, tools
import os
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.layout import cmake_layout
from conans.errors import ConanInvalidConfiguration

# Boost options (not header only because of openimageio) (from CCI Boost conanfile.py without required options)
BOOST_CONFIGURE_OPTIONS = (
    "context",
    "contract",
    "coroutine",
    "fiber",
    "graph",
    "graph_parallel",
    "iostreams",
    "json",
    "locale",
    "log",
    "math",
    "mpi",
    "nowide",
    "program_options",
    "python",
    "random",
    #    "regex",
    "serialization",
    "stacktrace",
    "test",
    "timer",
    "type_erasure",
    "wave",
)


class Anonymous(ConanFile):
    name = "anonymous"
    version = "head"
    license = "MPL v2"
    url = ""
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
        "freeimage:shared": True,
        # OIIO options
        "openimageio:shared": True,
        "openimageio:with_opencolorio": False,
        "openimageio:with_opencv": False,
        "openimageio:with_ffmpeg": False,
    }
    default_options.update({"boost:without_{}".format(
        bl): True for bl in BOOST_CONFIGURE_OPTIONS})

    generators = "CMakeDeps"
    exports_sources = ["anonymous/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

    build_requires = [
        "gtest/[>=1.11.0]",
        "benchmark/[>=1.5.0]",
    ]

    requires = [
        "range-v3/0.10.0",
        "fmt/6.0.0",
        "onetbb/2020.3",
        "xsimd/7.4.6",
        "eigen/3.3.9",
        "boost/1.78.0",
        "cfitsio/4.1.0",
        "freeimage/3.18.0",
        "openimageio/2.3.7.2",
        "openjpeg/2.5.0"  # To solve dependency conflict between freeimage and openimageio
    ]

    def _build_python(self):
        return self.options.shared or self.options.fPIC or tools.os_info.is_windows

    def _check_configuration(self):
        tools.check_min_cppstd(self, "20")
        if self.settings.compiler in ["gcc", "clang"] and tools.Version(self.settings.compiler.version) < 10:
            raise ConanInvalidConfiguration("Invalid compiler version for {} (Got {}, should be greater or equal to 10)".format(
                self.settings.compiler, self.settings.compiler.version))

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
        self.cpp.package.components["core"].libs = ["Anonymous-core"]
        self.cpp.package.components["core"].libdirs = ["lib"]
        self.cpp.package.components["core"].includedirs = ["include"]
        self.cpp.source.components["core"].includedirs = ["anonymous/include"]
        self.cpp.build.components["core"].libdirs = ["anonymous"]

        # Scribo component
        self.cpp.package.components["scribo"].libs = ["Anonymous-scribo"]
        self.cpp.package.components["scribo"].libdirs = ["lib"]
        self.cpp.package.components["scribo"].includedirs = ["include"]
        self.cpp.source.components["scribo"].includedirs = [
            "anonymous/include"]
        self.cpp.build.components["scribo"].libdirs = ["anonymous"]

        # IO component (FreeImage)
        self.cpp.package.components["io-freeimage"].libs = [
            "Anonymous-io-freeimage"]
        self.cpp.package.components["io-freeimage"].libdirs = ["lib"]
        self.cpp.package.components["io-freeimage"].includedirs = ["include"]
        self.cpp.source.components["io-freeimage"].includedirs = [
            "anonymous/include"]
        self.cpp.build.components["io-freeimage"].libdirs = ["anonymous"]

        # IO component (cfitsio)
        self.cpp.package.components["io-fits"].libs = ["Anonymous-io-fits"]
        self.cpp.package.components["io-fits"].libdirs = ["lib"]
        self.cpp.package.components["io-fits"].includedirs = ["include"]
        self.cpp.source.components["io-fits"].includedirs = ["anonymous/include"]
        self.cpp.build.components["io-fits"].libdirs = ["anonymous"]

        if self._build_python():
            self.cpp.package.components["anonymous-numpy"].libs = [
                "Anonymous-numpy"]
            self.cpp.package.components["anonymous-numpy"].libdirs = ["lib"]
            self.cpp.package.components["anonymous-numpy"].includedirs = ["include"]
            self.cpp.source.components["anonymous-numpy"].includedirs = [
                "anonymous-python/include"]
            self.cpp.build.components["anonymous-numpy"].libdirs = [
                "anonymous-python"]

    def build(self):
        # Controls what is built, not the toolchain
        variables = {"ANONYMOUS_BUILD_LIBS_ONLY": "YES"}
        if self._build_python():
            variables["ANONYMOUS_BUILD_PYTHON"] = "YES"
        else:
            self.output.warn(
                "fPIC disabled. Skipping python bindings build...")
        cmake = CMake(self)
        cmake.configure(variables)
        cmake.build()
        cmake.install()

    def package(self):
        tools.rmdir(os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.builddirs = ["cmake"]
        self.cpp_info.set_property("cmake_target_name", "anonymous::anonymous")

        # Core component
        self.cpp_info.components["core"].requires = [
            "range-v3::range-v3", "fmt::fmt", "tbb::tbb", "xsimd::xsimd", "boost::headers"]
        self.cpp_info.components["core"].libs = ["Anonymous-core"]

        # Scribo component
        self.cpp_info.components["scribo"].requires = ["core", "eigen::eigen3"]
        self.cpp_info.components["scribo"].libs = ["Anonymous-scribo"]

        # IO component (FreeImage)
        self.cpp_info.components["io-freeimage"].requires = ["core",
                                                             "freeimage::FreeImage"]
        self.cpp_info.components["io-freeimage"].libs = ["Anonymous-io-freeimage"]

        # IO component (cfitsio)
        self.cpp_info.components["io-fits"].requires = ["core",
                                                        "cfitsio::cfitsio"]
        self.cpp_info.components["io-fits"].libs = ["Anonymous-io-fits"]

        # Anonymous-numpy component
        if self._build_python():
            self.cpp_info.components["anonymous-numpy"].requires = ["core"]
            self.cpp_info.components["anonymous-numpy"].libs = ["Anonymous-numpy"]

        v = tools.Version(self.settings.compiler.version)
        for comp in self.cpp_info.components:
            self.cpp_info.components[comp].cxxflags.append(
                tools.cppstd_flag(self.settings))
            if self.settings.compiler == "gcc" and v.major == "9":
                self.cpp_info.components[comp].cxxflags.append("-fconcepts")

    def package_id(self):
        del self.info.settings.compiler.cppstd
