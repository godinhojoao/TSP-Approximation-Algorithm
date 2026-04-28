CXX = g++
CXXFLAGS = -std=c++17 -O2 -I src

SRC = src/main.cpp \
      src/Graph/Graph.cpp

OUT = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)