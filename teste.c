#include <stdio.h>
#include "grafo.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
int PosOrdem(grafo G, vertice v, int *tempo, vertice *visitado, vertice *Stack);


int main(void) {

  grafo g = le_grafo();

  escreve_grafo(g);
  printf("graph is directed: %d\n", agisdirected(g));
  int tempo = 0;
  int size = n_vertices(g);
  vertice *stack = calloc(size, sizeof(vertice));
  vertice *visitado = calloc(size, sizeof(vertice));
  // memset(visitado, NULL, sizeof(vertice)*size);
  printf("pos ordem retornou %d\n", PosOrdem(g, agfstnode(g), &tempo,visitado, stack));

  destroi_grafo(g);

  return 0;
}
