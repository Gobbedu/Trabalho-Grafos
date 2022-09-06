#include "grafo.h"
#include <stdio.h>  // stdin stdout stderr printf
#include <math.h>   // infinity
#include <stdlib.h>

//------------------------------------------------------------------------------
// funcao auxiliar para computar conectividade de um grafo
void DFSearch(int i, int size, int *visitado, int **matriz_adjacencia);
// funcoes & struct auxiliar para computar bipartidade de um grafo
int colorGraph(int **matrix, int color[], int size, int pos, int c);

int pertenceA(vertice u,vertice *Stack, int size);
void print_ordem(const char* name, vertice *lista, int size);
int append(vertice v, vertice *lista, int size);
vertice *PosOrdem(grafo G, int Transpose);
int Pos(grafo G, vertice r, vertice *visitado, vertice *Stack, int Transpose, int *sz_scc);


//------------------------------------------------------------------------------
grafo le_grafo(void) {
	return agread(stdin, NULL); 
}
//------------------------------------------------------------------------------
void destroi_grafo(grafo g) {
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

// -----------------------------------------------------------------------------
int bipartido(grafo g) {
	int **ma, *color, size;
	size = n_vertices(g);
	ma = matriz_adjacencia(g);
	color = malloc(((unsigned int) size) * sizeof(int));

	for(int i = 0; i < size; i++)
			color[i] = -1;
				
	//start is vertex 0;
	int pos = 0;
	// two colors 1 and 0
	return colorGraph(ma,color,size,pos,1);
		
	return 0;
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
	int **ma = matriz_adjacencia(g);
	int i, j, i_add, i_rm;
	
	size = (size*(size-1));
	i_add = 0;
	i_rm = 0;

	Agnode_t *u, *v, **u_add, **v_add, **u_rm, **v_rm;
	u_add = malloc(size*sizeof(Agnode_t*));
	v_add = malloc(size*sizeof(Agnode_t*));
	u_rm  = malloc(size*sizeof(Agnode_t*));
	v_rm  = malloc(size*sizeof(Agnode_t*));
	Agedge_t *e;

	// GUARDA OPERACOES
	for(u = agfstnode(g); u; u = agnxtnode(g, u)){
		for(v = agfstnode(g); v; v = agnxtnode(g, v)){
		// se {u, v} sao nodos diferentes e exite aresta
			if(AGID(u) != AGID(v))
			{
				e = agedge(g, u, v, NULL, 0);   // busca aresta
				if(!e){
					u_add[i_add] = u;      // aresta nn existe
					v_add[i_add] = v;      // aresta nn existe
					i_add++;
				}
				else{
					u_rm[i_rm] = u;      // aresta existe
					v_rm[i_rm] = v;      // aresta existe
					i_rm++;
				}
			}
		}
	}

	// REALIZA OPERACOES
	for(i = 0; i < i_add; i++)
		agedge(g, u_add[i], v_add[i], NULL, 1); // create edges
	for(j = 0; j < i_rm-1; j++){
		e = agedge(g, u_rm[j], v_rm[j], NULL, 0);
		if(e)
			agdeledge(g, e);                      // delete edges
	}

	free(ma);
	free(u_add);
	free(v_add);
	free(u_rm);
	free(v_rm);
	return g;
}

//------------------------------------------------------------------------------
grafo decompoe(grafo g) {
	int tam;
	vertice v;
	int scc = 0;
	
	// numero de vertices no grafo
	unsigned long int size = (unsigned long int) n_vertices(g);		
	if(!size)		// se size == 0, nao retorna grafo
		return NULL;																				

	if(!agisdirected(g))
		return NULL;

	// stack que guarda ordem de vertices processados em uma DFS
	vertice *stack = PosOrdem(g, 0);
	// print_ordem("\t\tSTACK POS ORDEM", stack, (int)size);

	// variaveis para re-processar vertices da stack
	// vertice **SCC = calloc(size, sizeof(vertice));
	vertice *visitados = calloc(size, sizeof(vertice));
	vertice *component = calloc(size, sizeof(vertice));

	// rodar DFS com o vertice no topo da stack
	for(int i = (int)size-1; i >= 0; i--){
		tam = 0;
		v = stack[i];																						// v = stack.pop()
		memset(component, 0, size*sizeof(vertice));							// stack contem componente forte
		if(!pertenceA(v, visitados, (int)size)){
			Pos(g, v, visitados, component, 1, &tam);							// DFS no grafo transposto
			print_ordem("COMPONENTE FORTE", component, tam);
			// >> as infos dos vertices ainda estao na lista <<
			// vertice r = component[0];
			// Agedge_t *o = agfstout(g, r);
			// Agedge_t *in = agfstin (g, r);
			// if(o && in && r)
			// printf("%s<(%s)>%s\n",agnameof(agtail(in)), agnameof(r), agnameof(aghead(o))); 
			scc++;																								// numero de componentes fortes
		}
	}

	printf("Existem %d componentes fortemente conexos\n", scc);

	return g;
}

// ======= Funcoes Auxiliares Decompoe ======= //
// retorna 1 se vertice u pertence a lista de vertices Stack de tamanho maximo size
// 0 cc
int pertenceA(vertice u,vertice *Stack, int size)
{
	for(int i = 0; i < size; i++)
		if(Stack[i] == u)	{
			return 1;
		}

	return 0;
}

// imprime uma lista de tamanho size de vertices
void print_ordem(const char* name, vertice *lista, int size)
{
	printf("%s: [", name);
	for(int i = 0; i < size; i++)
		if(lista[i] != NULL)
			printf(" %s", agnameof(lista[i]));
	printf(" ]\n");
}

// retorna 1 se conseguiu dar append do vertice v na lista, 0 cc
int append(vertice v, vertice *lista, int size)
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

vertice *PosOrdem(grafo G, int Transpose)
{
	int out = 0;
	int size = n_vertices(G);
	vertice *stack 		= calloc((unsigned long int) size, sizeof(vertice));	// cast para evitar warning
	vertice *visitado = calloc((unsigned long int) size, sizeof(vertice));	// cast para evitar warning
	
	// para todo nodo v in G nao visitado, DFS(G, v)
	for(vertice v = agfstnode(G); v; v = agnxtnode(G, v))
		if(!pertenceA(v, visitado, (int)size))
			Pos(G, v, visitado, stack, Transpose, &out);			// percurso de DFS(G, v) salvo na stack

  	// printf("pos ordem retornou %d\n", out);

	return stack;
}

int Pos(grafo G, vertice r, vertice *visitado, vertice *Stack, int Transpose, int *sz_scc)
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

// ======= Funcao Auxiliar Conexo ======= //
void DFSearch(int i, int size, int *visitado, int **matriz_adjacencia)
{  
	visitado[i] = 1;    // marca i como visitado

	// recorrencia para todo vertice adjacente a i
	for(int j = 0; j < size; j++){
		if (matriz_adjacencia[i][j] == 1 && !visitado[j])
			DFSearch(j, size, visitado, matriz_adjacencia);
	}
}


// ======== Funcao auxiliar Bipartido ======== //
int colorGraph(int **matrix, int color[], int size, int pos, int c)
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
