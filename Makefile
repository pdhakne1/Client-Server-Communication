all:	info.cpp info.hpp
	g++ -o P0 info.cpp
debug:	info.cpp info.hpp
	g++ -g -o P0 info.cpp
clean:
	rm -f *.o *~ P0 core
