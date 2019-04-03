all:
	g++ -O3 -Wall -shared -I ~/.conan/data/range-v3/0.4.0/ericniebler/stable/package/52137beddd093ee80959d89f66348c14a1589eb7/include/ -std=c++17 -fPIC `python3 -m pybind11 --includes` binding.cpp -o milena`python3-config --extension-suffix`
test:
	g++ -O3 -Wall -shared -std=c++17 -fPIC `python3 -m pybind11 --includes` test.cpp -o example`python3-config --extension-suffix`