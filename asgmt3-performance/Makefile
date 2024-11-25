CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -O2 -DNDEBUG

run: build_bad build_good
	/usr/bin/time -f "\tElapsed time:\t\t%e sec\n\tMaximum RSS:\t\t%M KiB" ./bad text.txt > result_bad.txt
	/usr/bin/time -f "\tElapsed time:\t\t%e sec\n\tMaximum RSS:\t\t%M KiB" ./good text.txt > result_good.txt

build_bad:
	$(CXX) $(CXXFLAGS) -o bad wordst_bad.cpp

build_good:
	$(CXX) $(CXXFLAGS) -o good wordst_good.cpp
