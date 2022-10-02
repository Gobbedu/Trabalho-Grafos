# Função Decompoe apresenta Erro

Feito por:
    EDUARDO GOBBO WILLI VASCONCELLOS GONÇALVES
    ERICK ECKERMANN CARDOSO

    A Função decompoe(grafo g) foi implementada seguindo o algoritmo de Kosaraju.
Neste algoritmo é realizado um total de duas buscas em profundidade, sendo que a primeira delas 
retorna o percurso dos vértices em pos-ordem. 

    A segunda é feita a partir dos vértices na sequencia reversa da pos-ordem, executando
somente para os vertices ainda não visitados nesta segunda busca, esta execução é a que retorna 
os componentes fortemente conexos.

Existem um total de quatro funções auxiliares, cujos propósitos são de:
    - Emular uma pilha (int append)
    - Emular pertencimento em conjuntos (int PertenceA)
    - Algoritmo de busca em profundidade que retorna percurso em pos-ordem (vertice *PosOrdem)
    - Função recursiva de PosOrdem (int Pos)

Os subgrafos gerados contem ambos os vertices percorridos e suas arestas pertencentes 
ao componente forte encontrado.
