#include "NearestInsertion.h"
#include "../Graph/Graph.h"
#include <limits> 

// Encontra o vizinho mais próximo pra inserir na rota
// exemplo: começa com a cidade 0 apenas. Acha o mais próximo dela e adiciona. 
// Repete, mas agora achando o mais próximo do ciclo atual
int NearestInsertion::findNearest(int startNode, const Graph& g, const std::vector<bool>& visited){
    int n = g.getVertices(); // número de nodos
    int bestNode = -1; // melhor nodo pra inserir
    int minCost = std::numeric_limits<int>::max(); // menor custo pra inserir
    
    for(int j = 0; j < n; j++){
        if(!visited[j]){ // se o nodo não foi visitado
            int weight = g.getDistance(startNode, j); // peso da aresta entre o nodo inicial e o nodo j
            if(weight >= 0 && weight < minCost){ // se o peso for maior ou igual a 0 e menor que o menor custo
                minCost = weight; // atualiza o menor custo
                bestNode = j; // atualiza o melhor nodo
            }
        }
    }
    return bestNode;
}

std::pair<std::vector<int>, int> NearestInsertion::run(const Graph& g) {
    int n = g.getVertices(); // numero de nodos pra visitar
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
            D[j] = g.getDistance(0, j); // distancia inicial até o ciclo (que só tem o 0)
        }
    }

    int firstNearestNode = findNearest(0, g, visited); // acha o vizinho mais próximo da cidade 0
    path.push_back(firstNearestNode); // adiciona o vizinho mais próximo à rota
    visited[firstNearestNode] = true; // marca o vizinho mais próximo como visitado
    currCost += g.getDistance(0, firstNearestNode) + g.getDistance(firstNearestNode, 0); // soma o custo do caminho ida e volta pro 0 

    // atualiza D considerando o novo nodo inserido no ciclo
    for(int j = 0; j < n; j++){
        if(!visited[j]){
            int dist = g.getDistance(firstNearestNode, j);
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
            
            int costUV = g.getDistance(u,v); // custo da aresta que será "cortada"
            int costUK = g.getDistance(u, bestNode); // custo da aresta de entrada
            int costKV = g.getDistance(bestNode, v); // custo da aresta de saída

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
                int dist = g.getDistance(bestNode, j);
                if(dist >= 0 && dist < D[j]){
                    D[j] = dist; // mantém sempre a menor distancia até o ciclo
                }
            }
        }
    }

    return {path, currCost}; // retorna o caminho e o custo
}