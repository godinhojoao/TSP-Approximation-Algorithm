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
// Space complexity: O(V) -> stack and auxiliar path
unsigned int Graph::runTSPBruteForce(TSPState state) const {
  int pathSize = state.path.size();
  int vertices = getVertices();
  if(pathSize == vertices) {
    int lastUsedPos = state.path.back();
    int firstPos = 0; // fixed start at pos 0 since it doesn't matter in symetric TSP
    int totalCostReturningToStart = state.currCost + getDistance(lastUsedPos, firstPos); // returning to path[0]
    return totalCostReturningToStart < state.bestCost ? totalCostReturningToStart : state.bestCost;
  }

  for(int i = 1; i < vertices; i++) { // O(v)
    // Avoid mirroring (only possible because it's symetric/undirected) --> iterations/2
    // 01230 == 03210 (path[1] = 1, path[1] = 3) we need to choose which to count to avoid counting both
    // lastButOne < path[i]? accept : skip (if want to accept first side of mirror)
    // lastButOne < path[1]? accept : skip (if want to accept second side of mirror)
    bool isLastButOneOfPath = pathSize == vertices - 1;
    if(isLastButOneOfPath && i > state.path[1]) {
      continue;
    }

    if(!state.visited[i]) {
      int lastUsedPos = state.path.back();
      int distanceToAdd = getDistance(lastUsedPos, i);
      state.currCost += distanceToAdd;
      state.path.push_back(i);
      state.visited[i] = true;

      state.bestCost = Graph::runTSPBruteForce(state); // O((V-1)!) since fixed start = 0

      state.currCost -= distanceToAdd;
      state.path.pop_back();
      state.visited[i] = false;
    }
  }

  return state.bestCost;
}

// Time Complexity: O(V!)
// Space complexity: O(V)
// worst case is equal, but it is faster because we are cleaning more unnecessary paths than before
// -- Why is it called Branch and Bound? Because at every branch we compute a lower/upper bound
// -- to decide whether we should continue exploring it or prune it.
// -- branch: explore paths | bound: estimate next path and cut if is worst
// it checks (lower XOR upper) depending on the problem (minimization or maximization), TSP is minimization
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

// Encontra o vizinho mais próximo pra inserir na rota
// exemplo: começa com a cidade 0 apenas. Acha o mais próximo dela e adiciona. 
// Repete, mas agora achando o mais próximo do ciclo atual
int Graph::findNearest(int startNode, const std::vector<bool>& visited) const {
    int n = getVertices(); // número de nodos
    int bestNode = -1; // melhor nodo pra inserir
    int minCost = std::numeric_limits<int>::max(); // menor custo pra inserir
    
    for(int j = 0; j < n; j++){
        if(!visited[j]){ // se o nodo não foi visitado
            int weight = getDistance(startNode, j); // peso da aresta entre o nodo inicial e o nodo j
            if(weight >= 0 && weight < minCost){ // se o peso for maior ou igual a 0 e menor que o menor custo
                minCost = weight; // atualiza o menor custo
                bestNode = j; // atualiza o melhor nodo
            }
        }
    }
    return bestNode;
}

std::pair<std::vector<int>, int> Graph::runTSPNearestInsertion() const {
    int n = getVertices(); // numero de nodos pra visitar
    std::vector<bool> visited(n, false); // vetor pra controlar os nodos ja visitados
    std::vector<int> path; // vetor que vai guardar o caminho
    int currCost = 0; // custo atual do caminho

    // vetor que guarda a menor distancia de cada nodo até o ciclo atual
    std::vector<int> D(n, std::numeric_limits<int>::max()); // max é só pra garantir que qualquer distancia seja menor

    if (n == 0) return {path, currCost}; // se não tiver nodos, retorna vazio
    if (n == 1) { // se tiver só 1 nodo, retorna ele 
        path.push_back(0);
        return {path, 0} ;
    }

    path.push_back(0); // começa com a cidade 0
    visited[0] = true; // marca a cidade 0 como visitada

    // inicializa D com as distancias a partir da cidade 0
    for(int j = 0; j < n; j++){
        if(!visited[j]){
            D[j] = getDistance(0, j); // distancia inicial até o ciclo (que só tem o 0)
        }
    }

    int firstNearestNode = findNearest(0, visited); // acha o vizinho mais próximo da cidade 0
    path.push_back(firstNearestNode); // adiciona o vizinho mais próximo à rota
    visited[firstNearestNode] = true; // marca o vizinho mais próximo como visitado
    currCost += getDistance(0, firstNearestNode) + getDistance(firstNearestNode, 0); // soma o custo do caminho ida e volta pro 0 

    // atualiza D considerando o novo nodo inserido no ciclo
    for(int j = 0; j < n; j++){
        if(!visited[j]){
            int dist = getDistance(firstNearestNode, j);
            if(dist >= 0 && dist < D[j]){
                D[j] = dist; // atualiza se for mais próximo do novo nodo
            }
        }
    }

    while (path.size() < n) {

        int bestNode = -1;
        int minDistanceToCycle = std::numeric_limits<int>::max();

        // encontra o nodo mais próximo do ciclo usando D (sem recalcular tudo)
        for(int j = 0; j < n; j++){ // percorre todos os nodos
            if(!visited[j] && D[j] < minDistanceToCycle){ // se o nodo não foi visitado e a distancia é menor que a minima
                minDistanceToCycle = D[j]; // atualiza a distancia minima
                bestNode = j; // atualiza o melhor nodo
            }
        }

        if (bestNode == -1) break; // Segurança para grafos desconexos

        // decide onde inserir o melhor nodo no ciclo
        int bestPos = -1;
        int minInsertionIncrease = std::numeric_limits<int>::max();

        for(int i = 0; i < path.size(); i++){
            int u = path[i]; // antecessor
            int v = path[(i + 1) % path.size()]; // sucessor (% pega o resto, garantindo que o último elemento do ciclo 'aponte' de volta pro primeiro)
            
            int costUV = getDistance(u,v); // custo da aresta que será "cortada"
            int costUK = getDistance(u, bestNode); // custo da aresta de entrada
            int costKV = getDistance(bestNode, v); // custo da aresta de saída

            int increase = costUK + costKV - costUV; // quanto o custo aumenta ao inserir o nodo entre u e v

            if(increase < minInsertionIncrease){ // se o aumento for menor que o mínimo
                minInsertionIncrease = increase; // atualiza o aumento mínimo
                bestPos = i + 1; // atualiza a posição de inserção
            }
        }
        
        //insere o nodo na posição encontrada
        path.insert(path.begin() + bestPos, bestNode);  // exemplo: se path = {0, 2, 1} e bestPos = 2, path vira {0, 2, 3, 1}
        visited[bestNode] = true; // atualiza que o nodo ja foi visitado
        currCost += minInsertionIncrease; // soma o aumento no custo

        // atualiza D considerando o novo nodo inserido no ciclo
        for(int j = 0; j < n; j++){
            if(!visited[j]){
                int dist = getDistance(bestNode, j);
                if(dist >= 0 && dist < D[j]){
                    D[j] = dist; // mantém sempre a menor distancia até o ciclo
                }
            }
        }
    }

    return {path, currCost}; // retorna o caminho e o custo
}