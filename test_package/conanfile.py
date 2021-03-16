from conans import ConanFile, CMake, tools
import os
import sys

class PyleneTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = [ "cmake_find_package", "cmake" ]
    
    # Disable python packaging until cmake_find_package generator for pybind11 does not work
    def _build_python(self):
        return (self.options["pylene"].shared or self.options["pylene"].fPIC or tools.os_info.is_windows) and False

    def build(self):
        cmake = CMake(self)
        if self._build_python():
            cmake.definitions["WITH_PYLENE_NUMPY"] = "YES"
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")

    def test(self):
        self.run(".{}main".format(os.sep))
        if self._build_python():
            self.run("{} main.py".format(sys.executable))
