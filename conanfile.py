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
    }

    generators = [ "cmake", "cmake_paths", "cmake_find_package" ]
    exports_sources = ["pylene/*", "pylene-python/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

    build_requires = [
        "gtest/[>=1.10]",
        "benchmark/[>=1.5.0]",

        # For now boost is too heavy and is not based on components
        # Such a dependancy brings linktime overhead because too many libraries are linked
        # "boost/1.73.0"
    ]

    requires = [
        "range-v3/0.10.0",
        "fmt/6.0.0",
        "tbb/2020.0",
        "xsimd/7.4.6",
    ]

    def _build_python(self):
        return self.options.shared or self.options.fPIC or tools.os_info.is_windows

    def configure(self):
        tools.check_min_cppstd(self, "20")
        if self.options.shared:
            del self.options.fPIC
        if self._build_python():
            self.requires.add("pybind11/2.6.2")

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

        self.cpp_info.components["Pylene"].system_libs.append("freeimage")
        self.cpp_info.components["Pylene"].names["cmake_find_package"] = "Pylene"
        self.cpp_info.components["Pylene"].names["cmake_find_package_multi"] = "Pylene"
        self.cpp_info.components["Pylene"].libs = ["Pylene"]
        self.cpp_info.components["Pylene"].includedirs = [os.path.join(self.package_folder, "include")]
        self.cpp_info.components["Pylene"].requires = ["range-v3::range-v3", "fmt::fmt", "tbb::tbb", "xsimd::xsimd"]

        if self._build_python():
            self.cpp_info.components["Pylene-numpy"].names["cmake_find_pakage_multi"] = "Pylene-numpy"
            self.cpp_info.components["Pylene-numpy"].names["cmake_find_pakage"] = "Pylene-numpy"
            self.cpp_info.components["Pylene-numpy"].libs = ["Pylene-numpy"]
            self.cpp_info.components["Pylene-numpy"].requires = ["Pylene", "pybind11::pybind11"]
            self.cpp_info.components["Pylene-numpy"].includedirs = [os.path.join(self.package_folder, "include")]

        v = tools.Version(self.settings.compiler.version)
        for comp in self.cpp_info.components:
            self.cpp_info.components[comp].cxxflags.append(tools.cppstd_flag(self.settings))
            if self.settings.compiler == "gcc" and v.major == "9":
                self.cpp_info.components[comp].cxxflags.append("-fconcepts")

    def package_id(self):
        del self.info.settings.compiler.cppstd
