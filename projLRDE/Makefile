CC = g++
CXXFLAGS = -Wall -Werror -Wextra -std=c++17 -I ~/.conan/data/range-v3/0.4.0/ericniebler/stable/package/52137beddd093ee80959d89f66348c14a1589eb7/include/ -ggdb

all: main

bindings:
	g++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` binding.cpp -o image_lib`python3-config --extension-suffix`

clean:
	${RM} main