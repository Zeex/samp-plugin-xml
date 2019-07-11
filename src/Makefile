CXX=g++
CXXFLAGS=-c -O2 -m32 -DLINUX -ISDK/amx
LDFLAGS=-m32

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) SDK/*.cpp
	$(CXX) $(CXXFLAGS) tinyxpath/*.cpp
	$(CXX) $(CXXFLAGS) *.cpp
	$(CXX) $(LDFLAGS) -shared *.o -o xml.so

.PHONY: clean
clean:
	rm -vf *.o *.so
