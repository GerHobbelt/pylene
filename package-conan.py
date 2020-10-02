from cpt.packager import ConanMultiPackager
import platform

if __name__ == "__main__":
    builder = ConanMultiPackager(username="lrde", build_policy="missing")
    builder.add(settings={"arch": "x86_64", "build_type": "Release"}, options={"pylene:shared" : True}, env_vars={}, build_requires={})
    builder.add(settings={"arch": "x86_64", "build_type": "Release"}, options={"pylene:shared" : False, "pylene:fPIC" : True}, env_vars={}, build_requires={})
    builder.add(settings={"arch": "x86_64", "build_type": "Release"}, options={"pylene:shared" : False, "pylene:fPIC" : False}, env_vars={}, build_requires={})

    for settings, options, env_vars, build_requires, reference in builder.items:
        settings["compiler.cppstd"] = 20
        settings["compiler.libcxx"] = "libstdc++11"
    builder.run()
