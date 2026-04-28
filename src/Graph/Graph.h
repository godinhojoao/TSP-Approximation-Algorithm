#pragma once

#include <string>
#include <vector>

class Graph {
public:
  Graph();

  void loadGraphFromFile(const std::string& filename);

  void addEdge(int u, int v, int distance);
  void printMatrix() const; // const here in cpp means = read only method

  int getDistance(int u, int v) const;
  int getVertices() const;

  // optimal solutiosn (NP-hard -> too slow)
  void runBruteForceTSPMetric() const;
  // void runBranchAndBoundTSPMetric() const;

private:
  int V;

  std::vector<std::vector<int>> adjMatrix;
};