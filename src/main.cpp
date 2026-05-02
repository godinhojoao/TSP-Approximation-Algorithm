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
    graph.printMatrix();
    std::cout << "Vertices: " << graph.getVertices() << "\n";

    TSPMetricBacktrackInput input;
    unsigned int cost = graph.runTSPMetricBacktrack(input);
    printf("cost: %d\n\n", cost);
  }
  return 0;
}