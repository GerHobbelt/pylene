override CXXFLAGS += -std=c++0x -I ../.. -W -Wall
override LDFLAGS += -lfreeimage -ltbb

all:

bin: marginalfilter simplify thicken lexico_grainfilter k12k0 rgbgrainfilter tos grainfilter
	[ -e bin ] || mkdir bin
	cp $^ bin


bin: CXXFLAGS += -O3 -DNDEBUG -march=native


tos: LDFLAGS += -lboost_program_options