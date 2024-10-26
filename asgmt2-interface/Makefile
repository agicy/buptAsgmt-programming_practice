C := gcc
CFLAGS := -Wall -Wextra -O2
CXX := g++
CXXFLAGS := -Wall -Wextra -O2

libhmac.so: hmac.c hmac.h
	$(C) $(CFLAGS) -shared -o libhmac.so hmac.c -lssl -lcrypto

test: test.cpp libhmac.so
	$(CXX) $(CXXFLAGS) -o test test.cpp -L. -lhmac -Wl,-rpath,'$$ORIGIN'
	./test

clean:
	rm -f test libhmac.so
