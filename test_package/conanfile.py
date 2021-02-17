from conans import ConanFile, CMake
import os
import sys

class PyleneTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = [ "cmake_find_package", "cmake" ]


    def build(self):
        os.remove(os.path.join(self.build_folder, "Findpybind11.cmake"))
        cmake = CMake(self)
        if self.options["pylene"].pylena_numpy and self.options["pylene"].fPIC:
            cmake.definitions["WITH_PYLENA_NUMPY"] = "YES"
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")

    def test(self):
        self.run(".{}main".format(os.sep))
        self.run("{} main.py".format(sys.executable))
