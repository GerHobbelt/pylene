from conan.tools.files import copy
import os


def deploy(graph, output_folder, **kwargs):
    # Note the kwargs argument is mandatory to be robust against future changes.
    for name, dep in graph.root.conanfile.dependencies.items():
        print(name, dep.package_folder, dep.cpp_info.bindirs)
        if dep.package_folder is None:
            continue
        for bindir in dep.cpp_info.bindirs:
            copy(graph.root.conanfile, "*.dll",  bindir, output_folder)