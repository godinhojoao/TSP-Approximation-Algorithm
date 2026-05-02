#pragma once

#include <limits>
#include <string>
#include <vector>

struct TSPMetricBacktrackInput {
  unsigned int bestCost = std::numeric_limits<unsigned int>::max();
  unsigned int currCost = 0;
  std::vector<unsigned int> visitedNodesIndexes = {0};
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
  unsigned int runTSPMetricBacktrack(TSPMetricBacktrackInput input) const;

private:
  int V;

  std::vector<std::vector<int>> adjMatrix;
};
