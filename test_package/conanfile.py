from conans import ConanFile, CMake
import os

class PyleneTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = [ "cmake_find_package" ]


    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")

    def test(self):
        self.run(".{}main".format(os.sep))
