#include <Graph/Graph.h>
#include <iostream>
#include <chrono>

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

// menu to choose algorithm
int getUserSelectedAlgorithm() {
  std::cout << "Choose Algorithm:\n";
  std::cout << "1 - Nearest Insertion (Heuristic)\n";
  std::cout << "2 - Brute Force (Optimal)\n";
  std::cout << "3 - Branch and Bound (Optimal)\n";
  std::cout << "0 - Go back\n";
  std::cout << "Choose one option: ";

  int selectedOption = -1;
  while(selectedOption < 0 || selectedOption > 3) {
    std::cin >> selectedOption;
    if(std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
      selectedOption = -1;
      std::cout << "invalid input try again: ";
      continue;
    }

    if(selectedOption < 0 || selectedOption > 3) {
      std::cout << "\ninvalid choice try again: ";
    }
  }

  std::cout << "\n";
  return selectedOption;
}

int main() {

  while(true) {
    Graph graph;

    std::string selectedFile = getUserSelectedFile();
    if(selectedFile == "") {
      return 0; // exit
    }

    // call menu to choose algorithm
    int selectedAlgo = getUserSelectedAlgorithm();
    if(selectedAlgo == 0) {
      continue; // back to file selection
    }

    graph.loadGraphFromFile("./tsp-files/" + selectedFile);
    int V = graph.getVertices();
    graph.printMatrix();
    std::cout << "Vertices: " << V << "\n\n";

    if (selectedAlgo == 1) {
      // START ---------------- Nearest Insertion ----------------
      auto s1 = std::chrono::high_resolution_clock::now();
      
      auto [path, heuristicCost] = graph.runTSPNearestInsertion();
      
      auto e1 = std::chrono::high_resolution_clock::now();
      auto t1 = std::chrono::duration_cast<std::chrono::microseconds>(e1 - s1).count();

      std::cout << "Algoritmo: Nearest Insertion\n";
      std::cout << "Custo: " << heuristicCost << "\n";
      std::cout << "Tempo: " << t1 << " us\n\n";
      double optimal_cost = selectedFile == "tsp1_253.txt" ? 253 : selectedFile == "tsp2_1248.txt" ? 1248 : selectedFile == "tsp3_1194.txt" ? 1194 : selectedFile == "tsp4_7013.txt" ? 7013 : selectedFile == "tsp5_27603.txt" ? 27603 : 0;
      double approximation_ratio = (double)heuristicCost / optimal_cost;
      std::cout << "Approximation ratio: " << approximation_ratio << "\n\n";
      // END ---------------- Nearest Insertion ----------------
    } 
    else if (selectedAlgo == 2) {
      // START ---------------- brute force (pure backtracking) ----------------
      TSPState input;
      input.visited.assign(V, false);
      input.path = {0};
      input.visited[0] = true;

      auto start1 = std::chrono::high_resolution_clock::now();
      unsigned int cost1 = graph.runTSPBruteForce(input);
      auto end1 = std::chrono::high_resolution_clock::now();

      auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

      std::cout << "Algoritmo: Brute Force\n";
      std::cout << "Custo: " << cost1 << "\n";
      std::cout << "Tempo: " << duration1.count() << " ms\n\n";
      // END ---------------- brute force (pure backtracking) ----------------
    } 
    else if (selectedAlgo == 3) {
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

      std::cout << "Algoritmo: Branch and Bound\n";
      std::cout << "Custo: " << cost2 << "\n";
      std::cout << "Tempo: " << duration2.count() << " ms\n\n";
      // END ---------------- Branch-and-bound ---------------- 
    }
    
  }

  return 0;
}