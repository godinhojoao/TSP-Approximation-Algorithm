#include <Graph/Graph.h>
#include <iostream>

int main() {
  Graph graph;
  // TODO: cin --> show filename options, user press number to choose
  graph.loadGraphFromFile("./tsp-files/tsp1_253.txt");

  // just for debug
  graph.printMatrix();
  std::cout << "distance(0,0): " << graph.getDistance(0, 0) << "\n";
  std::cout << "distance(0,1): " << graph.getDistance(0, 1) << "\n";

  std::cout << "Vertices: " << graph.getVertices() << "\n";

  graph.runBruteForceTSPMetric();

  return 0;
}