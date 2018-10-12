from conans import CMake, ConanFile, tools


class Pylene(ConanFile):
    name = "pylene"
    version = "2.0"
    license = "https://gitlab.lrde.epita.fr/olena/pylene/blob/dev/LICENSE"
    url = "https://gitlab.lrde.epita.fr/olena/pylene"
    description = "Image processing library targeting genericity and efficiency."
    settings = "os", "compiler", "arch", "build_type", "cppstd"
    options = {"gtest": [True, False], "benchmark": [True, False], "freeimage": [
        True, False], "boost_program_options": [True, False]}
    default_options = "gtest=True", "benchmark=True", "freeimage=False", "boost_program_options=False"
    generators = "cmake_paths"

    def source(self):
        git = tools.Git()
        git.clone("{}.git".format(self.url))

    def requirements(self):
        self.requires("range-v3/0.3.7@ericniebler/stable")

        if self.options.gtest:
            self.requires("gtest/1.8.1@bincrafters/stable")

        if self.options.benchmark:
            self.requires("benchmark/1.4.1@dutiona/stable")

        if self.options.freeimage:
            self.requires("freeimage/3.18.0@dutiona/stable")

        if self.options.boost_program_options:
            self.requires("boost_program_options/1.66.0@bincrafters/stable")
