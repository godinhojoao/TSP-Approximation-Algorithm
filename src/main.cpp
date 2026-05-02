#include <Graph/Graph.h>
#include <iostream>

std::string getUserSelectedFile() {
  std::vector<std::string> fileChoices = {"tsp1_253.txt", "tsp2_1248.txt", "tsp3_1194.txt", "tsp4_7013.txt",
                                          "tsp5_27603.txt"};
  std::cout << "Options:\n";
  for(int i = 0; i < fileChoices.size(); i++) {
    std::cout << i + 1 << " - " + fileChoices[i] + "\n";
  }
  std::cout << "Choose one file or press 0 to exit: ";

  int selectedOption = -1;
  while(selectedOption < 0 || selectedOption > fileChoices.size()) {
    std::cin >> selectedOption;
    if(std::cin.fail()) {
      std::cin.clear();             // reset error state
      std::cin.ignore(10000, '\n'); // ignore bad input
      selectedOption = -1;          // return to default option
      std::cout << "invalid input try again: ";
      continue;
    }

    if(selectedOption == 0) {
      return ""; // exit
    }

    if(selectedOption < 0 || selectedOption >= fileChoices.size()) {
      std::cout << "\ninvalid choice try again: ";
    }
  }

  std::cout << "\n";
  return fileChoices[selectedOption - 1];
}

int main() {

  while(true) {
    Graph graph;

    std::string selectedFile = getUserSelectedFile();
    if(selectedFile == "") {
      return 0; // exit
    }

    graph.loadGraphFromFile("./tsp-files/" + selectedFile);
    int V = graph.getVertices();
    graph.printMatrix();
    std::cout << "Vertices: " << V << "\n";

    // START ---------------- brute force (pure backtracking) ----------------
    TSPMetricBruteForceLvlState input;
    input.visited.assign(V, false);
    input.visitedNodesIndexes = {0};
    input.visited[0] = true;

    auto start1 = std::chrono::high_resolution_clock::now();
    unsigned int cost1 = graph.runTSPBruteForce(input);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

    std::cout << "runTSPBruteForce cost: " << cost1 << "\n";
    std::cout << "runTSPBruteForce time: " << duration1.count() << " ms\n\n";
    // END ---------------- brute force (pure backtracking) ----------------

    // START ---------------- Branch-and-bound ----------------
    TSPState state;
    state.bestCost = std::numeric_limits<unsigned int>::max();
    state.currCost = 0;
    state.visited.assign(V, false);
    state.path = {0};
    state.visited[0] = true;

    auto start2 = std::chrono::high_resolution_clock::now();
    unsigned int cost2 = graph.runTSPBranchAndBound(state);
    auto end2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);

    std::cout << "runTSPBranchAndBound cost: " << cost2 << "\n";
    std::cout << "runTSPBranchAndBound time: " << duration2.count() << " ms\n\n";

    std::cout << "difference (BruteForce - BranchAndBound): " << (duration1.count() - duration2.count()) << " ms\n\n";
    // END ---------------- Branch-and-bound ----------------
  }

  return 0;
}