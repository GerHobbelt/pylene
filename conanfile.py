from conans import CMake, ConanFile, tools
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
        "gtest:shared": False,
        "boost:header_only": True,
    }

    generators = [ "cmake", "cmake_paths", "cmake_find_package" ]
    exports_sources = ["pylene/*", "pylene-python/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

    build_requires = [
        "gtest/[>=1.10.0]",
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

    def configure(self):
        tools.check_min_cppstd(self, "20")
        if self.options.shared:
            del self.options.fPIC
        if self._build_python():
            self.build_requires.append("pybind11/2.6.2")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["PYLENE_BUILD_LIBS_ONLY"] = "YES"
        if self._build_python():
            cmake.definitions["PYLENE_BUILD_PYTHON"] = "YES"
        else:
            self.output.warn("fPIC disabled. Skipping python bindings build...")
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("FindFreeImage.cmake", dst="", src="cmake")
        tools.rmdir(os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = "Pylene"
        self.cpp_info.names["cmake_find_package_multi"] = "Pylene"

        # Core component
        self.cpp_info.components["Core"].names["cmake_find_package"] = "Core"
        self.cpp_info.components["Core"].names["cmake_find_package_multi"] = "Core"
        self.cpp_info.components["Core"].libs = ["Pylene-core"]
        self.cpp_info.components["Core"].includedirs = ["include"]
        self.cpp_info.components["Core"].requires = ["range-v3::range-v3", "fmt::fmt", "tbb::tbb", "xsimd::xsimd", "boost::headers"]

        # Scribo component
        self.cpp_info.components["Scribo"].names["cmake_find_package"] = "Scribo"
        self.cpp_info.components["Scribo"].names["cmake_find_package_multi"] = "Scribo"
        self.cpp_info.components["Scribo"].libs = ["Pylene-scribo"]
        self.cpp_info.components["Scribo"].includedirs = ["include"]
        self.cpp_info.components["Scribo"].requires = ["Core", "eigen::eigen3"]

        # IO component (FreeImage)
        self.cpp_info.components["IO-freeimage"].system_libs.append("freeimage")
        self.cpp_info.components["IO-freeimage"].names["cmake_find_package"] = "IO-freeimage"
        self.cpp_info.components["IO-freeimage"].names["cmake_find_package_multi"] = "IO-freeimage"
        self.cpp_info.components["IO-freeimage"].libs = ["Pylene-io-freeimage"]
        self.cpp_info.components["IO-freeimage"].includedirs = ["include"]
        self.cpp_info.components["IO-freeimage"].requires = ["Core"]

        # IO component (cfitsio)
        self.cpp_info.components["IO-fits"].names["cmake_find_package"] = "IO-fits"
        self.cpp_info.components["IO-fits"].names["cmake_find_package_multi"] = "IO-fits"
        self.cpp_info.components["IO-fits"].libs = ["Pylene-io-fits"]
        self.cpp_info.components["IO-fits"].includedirs = ["include"]
        self.cpp_info.components["IO-fits"].requires = ["Core", "cfitsio::cfitsio"]


        # Pylene-numpy component
        if self._build_python():
            self.cpp_info.components["Pylene-numpy"].names["cmake_find_pakage_multi"] = "Pylene-numpy"
            self.cpp_info.components["Pylene-numpy"].names["cmake_find_pakage"] = "Pylene-numpy"
            self.cpp_info.components["Pylene-numpy"].libs = ["Pylene-numpy"]
            self.cpp_info.components["Pylene-numpy"].requires = ["Core"]
            self.cpp_info.components["Pylene-numpy"].includedirs = ["include"]

        v = tools.Version(self.settings.compiler.version)
        for comp in self.cpp_info.components:
            self.cpp_info.components[comp].cxxflags.append(tools.cppstd_flag(self.settings))
            if self.settings.compiler == "gcc" and v.major == "9":
                self.cpp_info.components[comp].cxxflags.append("-fconcepts")

    def package_id(self):
        del self.info.settings.compiler.cppstd
