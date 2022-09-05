#include <stdio.h>
#include "grafo.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
vertice *PosOrdem(grafo G);

int main(void) {

  grafo g = le_grafo();

  escreve_grafo(g);
  // printf("graph is directed: %d\n", agisdirected(g));
  
  int tempo = 0;
  int size = n_vertices(g);
  size++;

  PosOrdem(g);

  destroi_grafo(g);

  return 0;
}
