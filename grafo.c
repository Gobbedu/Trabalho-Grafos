#include "grafo.h"
#include <stdio.h>  // stdin stdout stderr printf
#include <math.h>   // infinity
#include <stdlib.h>	// malloc, calloc, free

//------------------------------------------------------------------------------
// funcao auxiliar para computar conectividade de um grafo
// void DFSearch(int i, int size, int *visitado, int **matriz_adjacencia);
// // funcoes & struct auxiliar para computar bipartidade de um grafo
// int colorGraph(int **matrix, int color[], int size, int pos, int c);

//------------------------------------------------------------------------------
grafo le_grafo(void) {
	return agread(stdin, NULL); 
}
//------------------------------------------------------------------------------
void destroi_grafo(grafo g) {
	// deleta subgrafos, se algum
	for(grafo sub = agfstsubg(g); sub; sub = agnxtsubg(sub))
    	agdelsubg(g, sub);
	
	agclose(g);
}
//------------------------------------------------------------------------------
grafo escreve_grafo(grafo g) {
	agwrite(g, stdout);
	return g;
}

// -----------------------------------------------------------------------------
int n_vertices(grafo g) {
	int num_v;
	vertice n;

	num_v = 0;
	for(n = agfstnode(g); n; n=agnxtnode(g, n))
		num_v++;

	return num_v;
}

// --------------------------- aresta--------------------------------------------------
int n_arestas(grafo g) {
	int cont = 0;
	Agnode_t * v;  //Declara o vértice

	//Itera sobre os vértices
	for (v = agfstnode(g); v; v = agnxtnode(g,v)) { 
			cont += grau(v,g);
	}
	//Retorna a soma de todos os graus de vértices /2 = número de arestas
	return cont/2;
}

// -----------------------------------------------------------------------------
int grau(vertice v, grafo g) {
	// aresta-in = True && aresta-out = True para nao direcionado
	return agdegree(g, v, 1, 1);
}

// -----------------------------------------------------------------------------
int grau_maximo(grafo g)  {
	int max, grau_n;
	vertice n;
	max = 0;

	for(n = agfstnode(g); n; n = agnxtnode(g, n))
	{
		grau_n = grau(n, g);
		if(grau_n > max)
			max = grau_n;
	}
	return max;
}

// -----------------------------------------------------------------------------
int grau_minimo(grafo g)  {
	int min, grau_n;
	vertice n;
	min = (int)INFINITY;

	for(n = agfstnode(g); n; n = agnxtnode(g, n))
	{
		grau_n = grau(n, g);
		if(grau_n < min)
			min = grau_n;
	}
	return min;
}

// -----------------------------------------------------------------------------
int grau_medio(grafo g) {
	int cont = 0;
	Agnode_t * v;  //Declara o vértice
	//Soma todos os graus
	for (v = agfstnode(g); v; v = agnxtnode(g,v)) { 
			cont += grau(v,g);
	}
	//Divide a soma dos graus pelo número de vértices
	cont = cont/n_vertices(g);
	return cont;
}

// -----------------------------------------------------------------------------
int regular(grafo g) { 
	vertice v = agfstnode(g); //Declara v e atribui um vértice qualquer a ele
	int aux = grau(v,g);      //Recebe um grau genérico para comparar
	int true = 1;

	//Itera sobre os vértices
	for (v = agfstnode(g); v; v = agnxtnode(g,v)) { 
			if(aux != grau(v,g))
				return 0; // caso exista grau diferente, para de procurar
	}
	//Será tru se todos os vértices tiverem grau == aux
	return true;  
}

// -----------------------------------------------------------------------------
int completo(grafo g) {
	int Vg, Eg;

	Vg = n_vertices(g);
	Eg = n_arestas(g);

	// num combinacao de todos os vertices em 2 == num arestas
	return (Eg == (Vg*(Vg-1))/2);
}

// ======= Funcao Auxiliar Conexo ======= //
static void DFSearch(int i, int size, int *visitado, int **matriz_adjacencia)
{  
	visitado[i] = 1;    // marca i como visitado

	// recorrencia para todo vertice adjacente a i
	for(int j = 0; j < size; j++){
		if (matriz_adjacencia[i][j] == 1 && !visitado[j])
			DFSearch(j, size, visitado, matriz_adjacencia);
	}
}
// -----------------------------------------------------------------------------
// grafo tem um unico componente (pedacos separados no grafo)
int conexo(grafo g) {
	int **ma, *visitado, size, n_componentes;
	n_componentes = 0;
	size = n_vertices(g);
	ma = matriz_adjacencia(g);
	visitado = malloc( ((unsigned int) size) *sizeof(int));

	// inicializa todo vertice como nao visitado
	for(int i = 0; i < size; i++)
		visitado[i] = 0;

	// recursivamente visita vertices adjacentes
	for(int i = 0; i < size; i++){
		if(visitado[i] == 0){
			DFSearch(i, size, visitado, ma);
			n_componentes++;
		}
	}

	free(ma);
	free(visitado);
	return n_componentes;
}

// ======== Funcao auxiliar Bipartido ======== //
static int colorGraph(int **matrix, int color[], int size, int pos, int c)
{
		if(color[pos] != -1 && color[pos] !=c)
				return 0;
				 
		// color this pos as c and all its neighbours and 1-c
		color[pos] = c;
		int ans = 1;
		for(int i=0;i<size;i++){
				if(matrix[pos][i]){
						if(color[i] == -1)
								ans &= colorGraph(matrix,color,size,i,1-c);
								 
						if(color[i] !=-1 && color[i] != 1-c)
								return 0;
				}
				if (!ans)
						return 0;
		}
		 
		return 1;
}
// -----------------------------------------------------------------------------
int bipartido(grafo g) {
	int **ma, *color, size, cor, pos;
	size = n_vertices(g);
	ma = matriz_adjacencia(g);
	color = malloc(((unsigned int) size) * sizeof(int));

	for(int i = 0; i < size; i++)
			color[i] = -1;
				
	//start is vertex 0;
	pos = 0;
	// two colors 1 and 0
	cor = colorGraph(ma,color,size,pos,1);

	free(color);
	return cor;
}

// -----------------------------------------------------------------------------
int n_triangulos(grafo g) {
	int **ma, **aux, **mult, size;
	unsigned int u_size;
	
	size = n_vertices(g);
	ma = matriz_adjacencia(g);
	u_size = (unsigned int) size;
	aux = malloc(u_size*sizeof(int*) + u_size*u_size*sizeof(int));
	mult = malloc(u_size*sizeof(int*) + u_size*u_size*sizeof(int));

	aux[0] = (int*) (aux + size) ;  // ajusta o ponteiro da primeira linha
	for (int i=1; i < size; i++)      // ajusta os ponteiros das demais linhas (i > 0)
		aux[i] = aux[0] + (i * size) ;

	mult[0] = (int*) (mult + size) ;
	for (int i=1; i < size; i++)
		mult[i] = mult[0] + (i * size) ;

	//Copia elementos da matriz ma para aux
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			aux[i][j] = ma[i][j];

	//Eleva ma a potencia de 2
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				mult[i][j] += ma[i][k]*aux[k][j];

	//Eleva ma a potencia de 3
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				aux[i][j] += mult[i][k]*ma[k][j];
	//Calcula o trace
	int trace = 0;
	for (int i = 0; i < size; i++)
		trace += aux[i][i];
	//Libera as matrizes
	free(aux);
	free(mult);
	free(ma);
	return trace/6;

}

// -----------------------------------------------------------------------------
int **matriz_adjacencia(grafo g) {
	// para grafos nao direcionados somente
	vertice u, v;

	unsigned int size, i, j;
	int **MA;
	size = (unsigned int) n_vertices(g);

	MA = malloc(size*sizeof(int*) + size*size*sizeof(int));
	if(!MA) 
		return NULL;
	MA[0] = (int*) (MA + size) ;  // ajusta o ponteiro da primeira linha
	for (i=1; i < size; i++)      // ajusta os ponteiros das demais linhas (i > 0)
		MA[i] = MA[0] + (i * size) ;// pra facil indexacao & somente free(ma)


	// ma[i][j] = 0, se nao existe aresta {i, j}
	// ma[i][j] = 1, se existe aresta {i, j}
	for(i = 0, u = agfstnode(g); u; u = agnxtnode(g, u), i++){
		for(j = 0, v = agfstnode(g); v; v = agnxtnode(g, v), j++){
			// se {u, v} sao nodos diferentes e exite aresta
			if(i != j && agedge(g, u, v, NULL, 0) != NULL)
				MA[i][j] = 1;
			else 
				MA[i][j] = 0;
		}
	}

	return MA;
}

// -----------------------------------------------------------------------------
grafo complemento(grafo g) {
	unsigned int size = (unsigned int) n_vertices(g);
	size = (size*(size-1));	// todas as arestas possiveis

	int remove, add;
	vertice *uadd = calloc(size, sizeof(vertice));
	vertice *vadd = calloc(size, sizeof(vertice));

	add = 0;
	remove = 0;
	// guarda arestas que nao existem
	for(vertice n = agfstnode(g); n; n = agnxtnode(g, n)){
		for(vertice v = agfstnode(g); v; v = agnxtnode(g, v)){	
			if(AGID(n) != AGID(v) && !agedge(g, n, v, NULL, FALSE)){
				uadd[add] = n;
				vadd[add] = v;
				add++;
			}
		}
	}
	// remove arestas que existem
	Agedge_t *e, *aux;
	for(vertice n = agfstnode(g); n; n = agnxtnode(g, n)){
		for(e = agfstedge(g, n); e; e = aux){
			aux = agnxtedge(g, e, n);
			agdeledge(g, e);
			remove++;
		}
	}

	// cria arestas que nao existiam
	for(int i = 0; i < add; i++){
		if(!agedge(g, uadd[i], vadd[i], NULL, FALSE))
			agedge(g, uadd[i], vadd[i], NULL, TRUE);
	}

	return g;
}

// ======= Funcoes Auxiliares Decompoe ======= //
// retorna 1 se vertice u pertence a lista de vertices Stack de tamanho maximo size
// 0 cc
static int pertenceA(vertice u,vertice *Stack, int size)
{
	for(int i = 0; i < size; i++)
		if(Stack[i] == u)	{
			return 1;
		}

	return 0;
}
/*
// imprime uma lista de tamanho size de vertices
static void print_ordem(const char* name, vertice *lista, int size)
{
	printf("%s: [", name);
	for(int i = 0; i < size; i++)
		if(lista[i] != NULL)
			printf(" %s", agnameof(lista[i]));
	printf(" ]\n");
}
*/
// retorna 1 se conseguiu dar append do vertice v na lista, 0 cc
static int append(vertice v, vertice *lista, int size)
{
	if(!pertenceA(v, lista, size)){
		for(int i = 0; i < size; i++)
			if(lista[i] == NULL)
			{
				lista[i] = v;
				return 1;
			}
	}
	return 0;
}

// chamada recursiva para DFS de posOrdem
static int Pos(grafo G, vertice r, vertice *visitado, vertice *Stack, int Transpose, int *sz_scc)
{
	Agedge_t* (*first)(Agraph_t*, Agnode_t*);
	Agedge_t* (*next)(Agraph_t*, Agedge_t*);
	int size = n_vertices(G);

	if(!append(r, visitado, size))
		return 0;

	// define caminho a percorrer
	if(Transpose){
		first = &agfstin;  	// a <--
		next	= &agnxtin;
	}
	else{
		first = &agfstout;  // a -->
		next	= &agnxtout;
	}

	vertice v = r;

	for(Agedge_t *e = first(G, r); e; e = next(G, e)){
		v = (Transpose) ? agtail(e) : aghead(e);
		// printf("vizinho de r (%s) : %s\n",agnameof(r), agnameof(v));
		if(!pertenceA(v, Stack, size)){
			// printf("coloca %s na stack\n", agnameof(v));
			Pos(G, v, visitado, Stack, Transpose, sz_scc);
		}
	}
	// printf("PUSH ");
	if(append(r, Stack, size))
			*sz_scc += 1;

	return 1;
}

// retorna percurso de DFS em pos-ordem
static vertice *PosOrdem(grafo G, int Transpose)
{
	// vertice v;
	// agbindrec(v, agnameof(v), )

	int out = 0;
	int size = n_vertices(G);
	vertice *stack 		= calloc((unsigned long int) size, sizeof(vertice));	// cast para evitar warning
	vertice *visitado = calloc((unsigned long int) size, sizeof(vertice));	// cast para evitar warning
	
	// para todo nodo v in G nao visitado, DFS(G, v)
	for(vertice v = agfstnode(G); v; v = agnxtnode(G, v))
		if(!pertenceA(v, visitado, (int)size))
			Pos(G, v, visitado, stack, Transpose, &out);			// percurso de DFS(G, v) salvo na stack

  	// printf("pos ordem retornou %d\n", out);
	free(visitado);
	return stack;
}

//------------------------------------------------------------------------------
grafo decompoe(grafo g) {
	int tam;
	vertice v;
	
	// numero de vertices no grafo
	unsigned long int size = (unsigned long int) n_vertices(g);		

	// se tem 0 vertices ou nao direcionado
	if(!size || !agisdirected(g))
		return g;																				

	// stack que guarda ordem de vertices processados em uma DFS
	vertice *stack = PosOrdem(g, 0);
	// print_ordem("STACK POS ORDEM", stack, (int)size);

	vertice *visitados = calloc(size, sizeof(vertice));
	vertice *component = calloc(size, sizeof(vertice));

	// aloca subgrafos para cada componente forte
	grafo *sub = calloc((long unsigned int)size, sizeof(grafo));

	// rodar DFS com o vertice no topo da stack
	for(int i = (int)size-1; i >= 0; i--){
		tam = 0;
		v = stack[i];						// v = stack.pop()
		// stack contem componente forte
		memset(component, 0, size*sizeof(vertice));		
		if(!pertenceA(v, visitados, (int)size))
		{	// se v nao foi visitado ainda
			// DFS no grafo transposto
			Pos(g, v, visitados, component, 1, &tam);	
			// cria subgrafo
			sub[i] = agsubg(g, NULL, TRUE);				

			// monta subgrafo com 'tam' vertices no componente forte
			printf("size of subset is %d contains nodes:\n", tam);
			for(int j = 0; j < tam; j++)
			{	// adiciona vertices componente
				printf("\t%s\n", agnameof(component[j]));
				agsubnode(sub[i], component[j], TRUE);
				// adiciona aresta sse vizinho pertence ao componente
				// if(agedge(g, component[j], component[j], NULL, FALSE)) agedge(sub[i], component[j], component[j], NULL, TRUE);
				for(vertice x = agfstnode(g); x && pertenceA(x, component, tam); x = agnxtnode(g, x))
				{
					if(agedge(g, component[j], x, NULL, FALSE)) agsubedge(sub[i], agedge(g, component[j], x, NULL, FALSE), TRUE);												
					if(agedge(g, x, component[j], NULL, FALSE)) agsubedge(sub[i], agedge(g, x, component[j], NULL, FALSE), TRUE);

				}
				// for(Agedge_t *e = agfstout(g, component[j]); e && pertenceA(aghead(e), component, tam); e = agnxtout(g, e))
			}
		}
	}
	
	free(visitados);
	free(component);
	free(stack);
	free(sub);

	return g;
}


// testes para decifrar a lib cgraph

// vertice r = agfstnode(g);
// r = agnxtnode(g, r);
// Agedge_t *o = agfstout(g, r);
// Agedge_t *i = agfstin (g, r);
// printf("%s<(%s)>%s\n",agnameof(agtail(i)), agnameof(r), agnameof(aghead(o))); 

// printf("r: (%s) tail: (%s) head: (%s)\n",agnameof(r), agnameof(agtail(e)), agnameof(aghead(e)));
// printf("r: out neighbours is %d\n", agdegree(g, r, 0, 1));
// printf("r: in neighbours is %d\n", agdegree(g, r, 1, 0));
// return 0;

	// for(vertice j = agfstnode(G); j; j = aghead(agnxtout(G, j)))
	// 	printf("vizinhos de r: %s\n", agnameof(j));
