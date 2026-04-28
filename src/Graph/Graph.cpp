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

void Graph::runBruteForceTSPMetric() const {
  
}
