#pragma once

#include <limits>
#include <string>
#include <vector>

struct TSPMetricBruteForceLvlState {
  unsigned int bestCost = std::numeric_limits<unsigned int>::max();
  unsigned int currCost = 0;
  std::vector<unsigned int> visitedNodesIndexes;
  std::vector<bool> visited;
};

struct TSPState {
  unsigned int bestCost = std::numeric_limits<unsigned int>::max();
  unsigned int currCost = 0;
  std::vector<unsigned int> path;
  std::vector<bool> visited;
};

class Graph {
public:
  Graph();

  void loadGraphFromFile(const std::string& filename);

  void addEdge(int u, int v, int distance);
  void printMatrix() const; // const here in cpp means = read only method

  int getDistance(int u, int v) const;
  int getVertices() const;

  // optimal solution (NP-hard -> too slow)
  unsigned int runTSPBruteForce(TSPMetricBruteForceLvlState input) const;
  unsigned int runTSPBranchAndBound(TSPState state) const;

private:
  int V;

  std::vector<std::vector<int>> adjMatrix;
};
