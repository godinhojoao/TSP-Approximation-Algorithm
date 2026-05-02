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

// Time Complexity: O(V!)
// fix start at 0 -> permutations reduce to (V-1)!
// each step tries V choices -> O(V * (V-1)!) = O(V!)
// Space complexity: O(V) -> stack and auxiliar visitedNodesIndexes
unsigned int Graph::runTSPBruteForce(TSPMetricBruteForceLvlState input) const {
  int pathSize = input.visitedNodesIndexes.size();
  int vertices = getVertices();
  if(pathSize == vertices) {
    int lastUsedPos = input.visitedNodesIndexes.back();
    int firstPos = 0; // fixed start at pos 0 since it doesn't matter in symetric TSP
    int totalCostReturningToStart = input.currCost + getDistance(lastUsedPos, firstPos); // returning to path[0]
    return totalCostReturningToStart < input.bestCost ? totalCostReturningToStart : input.bestCost;
  }

  for(int i = 1; i < vertices; i++) { // O(v)
    // Avoid mirroring (only possible because it's symetric/undirected) --> iterations/2
    // 01230 == 03210 (path[1] = 1, path[1] = 3) we need to choose which to count to avoid counting both
    // lastButOne < path[i]? accept : skip (if want to accept first side of mirror)
    // lastButOne < path[1]? accept : skip (if want to accept second side of mirror)
    bool isLastButOneOfPath = pathSize == vertices - 1;
    if(isLastButOneOfPath && i > input.visitedNodesIndexes[1]) {
      continue;
    }

    if(!input.visited[i]) {
      int lastUsedPos = input.visitedNodesIndexes.back();
      int distanceToAdd = getDistance(lastUsedPos, i);
      input.currCost += distanceToAdd;
      input.visitedNodesIndexes.push_back(i);
      input.visited[i] = true;

      input.bestCost = Graph::runTSPBruteForce(input); // O((V-1)!) since fixed start = 0

      input.currCost -= distanceToAdd;
      input.visitedNodesIndexes.pop_back();
      input.visited[i] = false;
    }
  }

  return input.bestCost;
}

// Time Complexity: O(V!)
// Space complexity: O(V)
// worst case is equal, but it is faster because we are cleaning more unnecessary paths than before
unsigned int Graph::runTSPBranchAndBound(TSPState state) const {
  int pathSize = state.path.size();
  int vertices = getVertices();

  if(pathSize == vertices) {
    int lastUsedPos = state.path.back();
    unsigned int totalCost = state.currCost + getDistance(lastUsedPos, 0);
    return totalCost < state.bestCost ? totalCost : state.bestCost;
  }

  for(int i = 0; i < vertices; i++) {
    if(state.visited[i]) {
      continue;
    }

    // Good for larger iterations such as on brute force but worse for small ones as in branch and bound (extra
    // comparison overhead) Avoid mirroring: iterations/2 (v!/2) -> still O(v!)
    // bool isLastButOneOfPath = pathSize == vertices - 1;
    // if(isLastButOneOfPath && i > state.path[1]) {
    //   continue;
    // }

    // key diff from brute-force version
    int currDist = getDistance(state.path.back(), i);
    int newCost = state.currCost + currDist;
    if(newCost >= state.bestCost) { // cleaning unnecessary paths (prune = Branch-and-Bound)
      continue;
      // if I use a specific graph where pruning never happens, it becomes as bad as brute force because
      // it ends up exploring the entire search space without cutting any branches
      // fortunately, this is not the case in most situations
    }

    // before: ~210ms for file1 (code looks "cleaner" but performs worse)
    // every recursion level creating a new state struct = terrible
    // TSPState nextLevelState = state;
    // nextLevelState.currCost = newCost;
    // nextLevelState.visited[i] = true;
    // nextLevelState.path.push_back(i);
    // state.bestCost = std::min(state.bestCost, runTSPBranchAndBound(nextLevelState));

    // now: ~115ms for file1
    state.currCost += currDist;
    state.visited[i] = true;
    state.path.push_back(i);

    state.bestCost = std::min(state.bestCost, runTSPBranchAndBound(state));

    state.path.pop_back();
    state.visited[i] = false;
    state.currCost -= currDist;
  }

  return state.bestCost;
}