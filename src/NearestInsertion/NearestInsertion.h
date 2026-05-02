#ifndef NEAREST_INSERTION_H
#define NEAREST_INSERTION_H

#include "Graph.h"
#include <vector>
#include <utility> // Necessário para usar std::pair

class NearestInsertion {
public:
    // Agora o run retorna um par: <Caminho, Custo>
    static std::pair<std::vector<int>, int> run(const Graph& g); 

private:
    // Função auxiliar que só a própria classe usa
    static int findNearest(int startNode, const Graph& g, const std::vector<bool>& visited); 
    // Função auxiliar pra encontrar o nodo mais próximo do ciclo atual
    static int findNearestToCycle(const std::vector<int>& path, const Graph& g, const std::vector<bool>& visited);
};

#endif