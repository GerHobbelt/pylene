from conans import ConanFile, CMake, tools
import os
import sys

class PyleneTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = [ "cmake_find_package", "cmake" ]


    def build(self):
        cmake = CMake(self)
        if self.options["pylene"].shared or self.options["pylene"].fPIC or tools.os_info.is_windows:
            os.remove(os.path.join(self.build_folder, "Findpybind11.cmake")) # TO REMOVE WHEN cmake_find_package FOR PYBIND11 WORK
            cmake.definitions["WITH_PYLENE_NUMPY"] = "YES"
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")

    def test(self):
        self.run(".{}main".format(os.sep))
        if self.options["pylene"].shared or self.options["pylene"].fPIC or tools.os_info.is_windows:
            self.run("{} main.py".format(sys.executable))
