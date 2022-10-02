#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------
static grafo le_grafo_de_arquivo(FILE *input) {

  return agread(input, NULL); 
}
//------------------------------------------------------------------------------
// compara gabarito com resposta
// devolve 0, caso sejam iguais
//         1, se o número de vértices difere
//         2, se os nomes dos vértices não são iguais
//         3, se os graus dos vértices diferem
//         4, se os arcos diferem

static int compara(grafo gabarito, grafo resposta){

  int n_gabarito=agnnodes(gabarito),
    n_resposta=agnnodes(resposta);

  if ( n_gabarito != n_resposta ) {

    printf("resposta tem %d vértices != %d no gabarito\n", n_resposta, n_gabarito);
    return 1;
  }

  printf("10\n"); fflush(stdout);

  for (Agnode_t *v = agfstnode(gabarito); v; v = agnxtnode(gabarito, v)){

    char *nome_v = agnameof(v);

    if ( ! agnode(resposta, nome_v, 0) ) {

      printf("resposta não tem vértice %s\n", nome_v);
      return 2;
    }

    int grau_v = agdegree(gabarito, v, 0, 1),
      grau_v_resposta = agdegree(resposta, agnode(resposta, agnameof(v), 0), 0, 1);

    if ( grau_v != grau_v_resposta ) {

      printf("%s na resposta tem grau de saída %d != %d no gabarito\n", 
             nome_v, 
             grau_v_resposta, 
             grau_v
             );

      return 3;
    }

    for (Agedge_t *a = agfstout(gabarito, v); a; a = agnxtout(gabarito, a)) {

      char *tail=agnameof(v), *head=agnameof(aghead(a));

      if ( ! agedge(resposta, agnode(resposta,tail,0), agnode(resposta,head,0), NULL, 0) ) {

        printf("resposta não tem arco %s -> %s\n", tail, head);
        return 4;
      }
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
// devolve um (novo) grafo que é a união dos subgrafos em g

static grafo uniao_subgrafos(grafo g){

  grafo uniao = agopen(NULL, Agstrictdirected, NULL);

  for (grafo h=agfstsubg(g); h; h=agnxtsubg(h)) {

    for (Agnode_t *v = agfstnode(h); v; v = agnxtnode(h, v)){

      if ( agdegree(h, v, 0, 1) == 0 )
        agnode(uniao, agnameof(v), 1);
      else
        for (Agedge_t *a = agfstout(h, v); a; a = agnxtout(h, a)){
          
          char *tail=agnameof(v), *head=agnameof(aghead(a));

          agedge(uniao,agnode(uniao,tail,1),agnode(uniao,head,1),NULL,1);
        }
    }
  }
  return uniao;
}
//------------------------------------------------------------------------------
// argv[1] deve ser (o nome de) um grafo direcionado g em formato dot
// 
// argv[2] deve ser (o nome de) um grafo direcionado h em formato dot
//         cujos componentes fortes são os componentes fortes de g
//
// compara decompoe(g) e h e devolve
//
//         0, caso ambos sejam iguais
//         1, se o número de vértices difere
//         2, se os nomes dos vértices não são iguais
//         3, se os graus dos vértices diferem
//         4, se os arcos diferem
//        10, se não tem exatamente 2 argumentos na linha de comando

int main(int argc, char *argv[]) {

  if ( argc != 3 )
    return 10;

  grafo instancia=le_grafo_de_arquivo(fopen(argv[1], "r"));

  printf("\nexecutando decompoe() ...\n"); fflush(stdout);

  grafo resposta=decompoe(instancia);

  printf("decompoe() executou\n"); fflush(stdout);

  return compara(le_grafo_de_arquivo(fopen(argv[2], "r")), uniao_subgrafos(resposta));
}
