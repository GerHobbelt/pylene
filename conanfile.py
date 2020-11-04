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
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": False,
        "gtest:shared": False,
        "boost:shared": True
    }

    generators = [ "cmake", "cmake_paths", "cmake_find_package" ]
    exports_sources = ["pylene/*", "cmake/*", "CMakeLists.txt", "LICENSE"]

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
        "xsimd/7.4.6"
    ]


    def configure(self):
        tools.check_min_cppstd(self, "20")
        if self.options.shared:
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.definitions["PYLENE_BUILD_LIBS_ONLY"] = "YES"
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("FindFreeImage.cmake", dst="", src="cmake")
        tools.rmdir(os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.system_libs.append("freeimage")
        self.cpp_info.names["cmake_find_package"] = "Pylene"
        self.cpp_info.names["cmake_find_package_multi"] = "Pylene"
        self.cpp_info.libs = [ "Pylene" ]


        self.cpp_info.cxxflags.append(tools.cppstd_flag(self.settings))


        v = tools.Version(self.settings.compiler.version)
        if self.settings.compiler == "gcc" and v.major == "9":
            self.cpp_info.cxxflags.append("-fconcepts")

    def package_id(self):
        del self.info.settings.compiler.cppstd
