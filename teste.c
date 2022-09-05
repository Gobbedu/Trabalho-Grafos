#include <stdio.h>
#include "grafo.h"
#include <stdlib.h>

//------------------------------------------------------------------------------

int main(void) {

  grafo g = le_grafo();

  escreve_grafo(g);
  
  decompoe(g);

  destroi_grafo(g);

  return 0;
}
