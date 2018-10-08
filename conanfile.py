from conans import CMake, ConanFile, tools


class Pylene(ConanFile):
    name = "pylene"
    version = "2.0"
    license = "https://gitlab.lrde.epita.fr/olena/pylene/blob/dev/LICENSE"
    url = "https://gitlab.lrde.epita.fr/olena/pylene"
    description = "Image processing library targeting genericity and efficiency."
    settings = "os", "compiler", "arch", "build_type", "cppstd"
    options = {"benchmark": [True, False]}
    default_options = "benchmark=True"
    generators = "cmake_find_package"

    def source(self):
        git = tools.Git()
        git.clone("{}.git".format(self.url))

    def requirements(self):
        if str(self.settings.compiler) == "Visual Studio":
            tag = "stable-default"
        else:
            tag = "stable-{}-{}".format(self.settings.compiler,
                                        str(self.settings.build_type).lower())

        self.requires("boost_program_options/1.66.0@bincrafters/stable")
        self.requires("range-v3/0.3.6@ericniebler/stable")
        self.requires("gtest/1.8.1@bincrafters/stable")
        self.requires("freeimage/3.18.0@dutiona/{}".format(tag))
        if self.options.benchmark:
            self.requires("benchmark/1.4.1@dutiona/{}".format(tag))
