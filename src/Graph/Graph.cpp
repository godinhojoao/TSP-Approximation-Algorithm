#include "Graph.h"
#include <fstream>
#include <iostream>
#include <sstream> // string stream -> to handle strings as streams in chunks (separated by blank spaces)
#include <stdexcept>
#include <string>
#include <vector>

Graph::Graph() {
  V = 0;
}

void Graph::addEdge(int u, int v, int distance) {
  if(u < 0 || v < 0 || distance < 0) {
    throw std::runtime_error("negative values for vertices or edge distance cost are not allowed");
  }

  adjMatrix[u][v] = distance;

  std::cout << "u: " << u << " v: " << v << " distance: " << distance << "\n";
}

void Graph::loadGraphFromFile(const std::string& filename) {
  if(filename.empty()) {
    throw std::runtime_error("Cannot open empty filename");
  }

  std::ifstream inFile(filename);
  if(!inFile.is_open()) {
    throw std::runtime_error(std::string("Cannot open ") + filename);
  }

  std::string line;
  while(std::getline(inFile, line)) {
    std::stringstream stringStream(line);

    int currDistanceToNodeValue;
    std::vector<int> currRow;
    // the curr chunk value goes to >> currDistanceToNodeValue (stops when no chunk - end of string)
    while(stringStream >> currDistanceToNodeValue) {
      currRow.push_back(currDistanceToNodeValue);
    }
    adjMatrix.push_back(currRow);
  }

  V = adjMatrix.size();
  inFile.close();
}

void Graph::printMatrix() const {
  std::cout << "Current loaded matrix:\n";
  for(int i = 0; i < V; i++) {
    for(int j = 0; j < V; j++) {
      std::cout << adjMatrix[i][j] << " ";
    }
    std::cout << "\n";
  }
}

int Graph::getDistance(int u, int v) const {
  return adjMatrix[u][v];
}

int Graph::getVertices() const {
  return V;
}

// Time complexity: O(v*v!) -> v! recursion and in each path does for(i=1 to v)
// Space complexity: O(v) -> stack and auxiliar visitedNodesIndexes
unsigned int Graph::runTSPMetricBacktrack(TSPMetricBacktrackInput input) const {
  int pathSize = input.visitedNodesIndexes.size();
  int vertices = getVertices();
  if(pathSize == vertices) {
    int firstPos = 0; // fixed start at pos 0 since it doesn't matter in symetric TSP
    int lastUsedPos = input.visitedNodesIndexes[pathSize - 1];
    input.currCost += getDistance(lastUsedPos, firstPos); // returning to path[0]
    input.bestCost = input.currCost < input.bestCost ? input.currCost : input.bestCost;
    return input.bestCost;
  }

  for(int i = 1; i < vertices; i++) {
    // Avoid mirroring (only possible because it's symetric/undirected) --> iterations/2
    // 01230 == 03210 (path[1] = 1, path[1] = 3) we need to choose which to count to avoid counting both
    // lastButOne < path[i]? accept : skip (if want to accept first side of mirror)
    // lastButOne < path[1]? accept : skip (if want to accept second side of mirror)
    bool isLastButOneOfPath = pathSize == vertices - 1;
    if(isLastButOneOfPath && i > input.visitedNodesIndexes[1]) {
      continue;
    }

    auto pointerToItem = std::find(input.visitedNodesIndexes.begin(), input.visitedNodesIndexes.end(), i);
    bool isNotVisitedItem = pointerToItem == input.visitedNodesIndexes.end(); // if not found point to end
    if(isNotVisitedItem) {
      int lastUsedPos = input.visitedNodesIndexes[pathSize - 1];
      int distanceToAdd = getDistance(lastUsedPos, i);
      input.currCost += distanceToAdd;
      input.visitedNodesIndexes.push_back(i);

      input.bestCost = Graph::runTSPMetricBacktrack(input);

      input.currCost -= distanceToAdd;
      input.visitedNodesIndexes.pop_back();
    }
  }

  return input.bestCost;
}
