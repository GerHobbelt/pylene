override CXXFLAGS += -W -Wall -std=c++11 -fPIC -I/usr/include/python2.7 -I../..
LDFLAGS = -lfreeimage -lboost_python

all: tosGUI/tosGUI_.so


tosGUI/tosGUI_.so: loadtree.o tosgui.o filtertree.o
	$(CC) -shared -Wl,--export-dynamic $(LDFLAGS)  $^ -o $@


