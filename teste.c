#include <stdio.h>
#include "grafo.h"
#include <stdlib.h>

//------------------------------------------------------------------------------

int main(void) {

  grafo g = le_grafo();

  // escreve_grafo(g);
  
  g = decompoe(g);

  // printf("TEM %d SUBGRAFOS\n", agnsubg(g));
  for(grafo sub = agfstsubg(g); sub; sub = agnxtsubg(sub)){
    escreve_grafo(sub);
  }

  destroi_grafo(g);

  return 0;
}
