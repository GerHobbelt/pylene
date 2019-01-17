# Compiling, running, comparing benchmarks

## Fetching external data

Before compiling and running the benchmarks, external data are required:
* external image samples (that can be quite huge for testing)
* reference benchmarks
So before everything else, fetch external data with the rules:

```sh
cmake --build . --target fetch-external-data
```

## Compiling and executing benchmarks

Benchmarks are compiled automatically when the target
`run-all-benchmarks` is triggered and then run.


```sh
cmake --build . --target run-all-benchmarks --config Release
```

It will output for each benchmark, a speed report with the name:
`{BenchmarkName}-{Compiler}-{Version}.json`


## Non-regressions tests

Once the benchmarks have been run, you can compare with the references by running the tests. Note that a test is ran
only if there is a reference file.


```sh
ctest -L SpeedTests -V
```

# Adding & Updating references

* Download benchmark artifacts from the CI
* Copy artifacts to `/lrde/dload/olena/pylene/data/benchmarks`
* Create links to `/lrde/dload/olena/pylene/data/MD5` to new benchmarks (use the script `install.sh`)
* Update MD5 checksum in `CVS_ROOT/bench/references` (just move all `*.md5` that have been generated by the `install.sh` script)

