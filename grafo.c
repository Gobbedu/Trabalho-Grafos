#include "grafo.h"
#include <stdio.h>  // stdin stdout stderr printf
#include <math.h>   // infinity
#include <stdlib.h>

//------------------------------------------------------------------------------
// funcao auxiliar para computar conectividade de um grafo
void DFSearch(int i, int size, int *visitado, int **matriz_adjacencia);
// funcoes & struct auxiliar para computar bipartidade de um grafo
int colorGraph(int **matrix, int color[], int size, int pos, int c);
int PosOrdem(grafo G, vertice v, int *tempo, vertice *visitado, vertice *Stack);


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
	/*  l <- reverso de pos-ordem de G^T
			C <- Grafo Vazio
			while(l != vazio){
				v <- primeiro vertice de l
				if(v in V(C))
					remove v de l
				else{
					T <- Arborecencia da busca a partir de v
					Acrescente G[V(T)] a C
				}      
			}
	*/




	return g;
}


int pertenceA(vertice u,vertice *Stack, int size)
{
	for(int i = 0; i < size; i++)
		if(Stack[i] == u)	
			return 1;

	return 0;

}

void print_ordem(char* name, vertice *lista, int size){
	printf("%s: [", name);
	for(int i = 0; i < size; i++)
		if(lista[i] != NULL)
			printf(" %s", agnameof(lista[i]));
	printf(" ]\n");

}

int append(vertice v, vertice *lista, int size)
{
	print_ordem("Dando append em", lista, size);
	if(!pertenceA(v, lista, size)){
		printf("%s nao pertence a lista\n", agnameof(v));
		for(int i = 0; i < size; i++)
			if(lista[i] == NULL)
			{
				lista[i] = v;
				printf("deu append em %s\n", agnameof(v));
				return 1;
			}
	}

	return 0;
}

int PosOrdem(grafo G, vertice v, int *tempo, vertice *visitado, vertice *Stack)
{
	// int size = n_vertices(G);
	int neigh_saida = agdegree(G, v, 0, 1);
	// printf("nodo: %s\n", agnameof(v));
	// printf("\ttem %d vizinhos de saida\n",   agdegree(G, v, 0, 1));
	// printf("\ttem %d vizinhos de entrada\n", agdegree(G, v, 1, 0));
	vertice u = v;
	int size = n_vertices(G);
	// agcopyattr(v, u);
	// visitado[*tempo] = v;
	// if(!append(v, visitado, size))
	// 	return -1;

	append(v, visitado, size);

	for(int i = 0; i < neigh_saida; i++)
	{
		Agedge_t *e = agfstout(G, u);
		u = aghead(e);
		// u = agnxtnode(G, u);
		printf("\n%d; root(%s) name of node: %s\n",i,agnameof(v), agnameof(u));
		if(!pertenceA(u, visitado, size)){
			printf("%s nao pertence a stack, recursivo\n", agnameof(u));
			PosOrdem(G, u, tempo, visitado, Stack);
		}
	}
	// Stack[*tempo] = v;
	// *tempo++;
	append(v, Stack, size);
	// for(u = v; u; agnxtout(G, u)){
	// 	if(i++ == size)
	// 		break;
	// }

	print_ordem("Stack", Stack, size);



	return 1;
}

// ======= Funcoes Auxiliares Decompoe ======= //
/*
void BuscaProfundidade(grafo G)
{
	grafo T; // copia de G ?

	int size = n_vertices(G);
	vertice *PosOrdem = calloc(size, sizeof(vertice));
	int *visitado = malloc( ((unsigned int) size) *sizeof(int));

	// inicializa todo vertice como nao visitado
	for(int i = 0; i < size; i++)
		visitado[i] = 0;


	int tempo = 0; // define ordem (pre/pos)
	for(vertice u = agfstnode(G); u; u = agnxtnode(G, u)){
		// if(!visitado[u])
			BuscaProfunda(G, u, &tempo, visitado, PosOrdem);
	}

	printf("visitado: ");
	for(int i = 0; i < size; i++)
		printf("%d ", visitado[i]);
	printf("\n");
}

void BuscaProfunda(grafo G, vertice r, int *t, int *visitado, vertice PosOrdem)
{
// Mark the current node as visited and print it
	// visited[v] = 1;

	// Recur for all the vertices adjacent to this vertex
	// list<int>::iterator i;
	// for(i = adj[v].begin(); i != adj[v].end(); ++i)
	
	// 	if(!visited[*i])
	// 		fillOrder(*i, visited, Stack);

	

	// All vertices reachable from v are processed by now, push v
  //   cout << v << " Stack\n";
	// Stack.push(v);

	// Todos os vertices alcancaveis de v foram processados, stack.append(v)
	// Stack[*tempo] = v;
	// *tempo++;
}
*/



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
