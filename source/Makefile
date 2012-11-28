CXX=g++
CXXFLAGS=-c -O3 -DLINUX -ISDK/amx

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) SDK/*.cpp
	$(CXX) $(CXXFLAGS) tinyxpath/*.cpp
	$(CXX) $(CXXFLAGS) *.cpp
	$(CXX) -shared *.o -o xml.so

.PHONY: clean
clean:
	rm -vf *.o *.so
