#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int chave;
} Item;

typedef struct {
  int capacidade; //quantidade máxima de itens
  int qntdElementosInseridos; //número de itens inseridos na lista
  Item *array; //vetor de itens na lista
} Lista;

Lista *criarLista(int capacidade){
  Lista *l = malloc(sizeof(Lista));
  if (l == NULL){
    printf("Erro ao alocar memoria para lista");
    free(l);
    exit(1);
  }

  l->array = malloc(capacidade * sizeof(Item));
  if(l->array == NULL){
    printf("Erro ao alocar memoria para o vetor de itens");
    free(l);
    exit(1);
  }

  l->qntdElementosInseridos = 0;
  l->capacidade = capacidade;
  return l;
}

void inicializarJogo(Lista *l, Item x){
  srand(time(NULL));

  for (int i = 0; i < 4; i++){
    l->array[l->qntdElementosInseridos] = x;
    x.chave = rand() % 9;
    l->qntdElementosInseridos++;
  }
}

void inserirItem(Lista *l, Item x){
  srand(time(NULL));

  if (l->qntdElementosInseridos == l->capacidade){
    printf("Lista cheia! Impossível inserir mais itens.\n");
    return;
  }

  l->array[l->qntdElementosInseridos] = x;
  x.chave = rand() % 9;
  l->qntdElementosInseridos++;
}

void exibirLista(Lista *l){
  for (int i = 0; i < l->qntdElementosInseridos; i++){
    printf("index: %d:\t%d\n", i, l->array[i].chave);
  }
}

int main(){

    Lista *jogo = criarLista(100);

    Item x;

    inicializarJogo(jogo, x);

    int rodadas = 0; //conta na main o número da rodada atual

    int num;

    while (rodadas < 6){ //mecanismo de alternância entre jogadores
        if (rodadas % 2 == 1){
            exibirLista(jogo);
            scanf("%i", &num);
        }else{
            exibirLista(jogo);
            scanf("%i", &num);
        }
        rodadas++;
        inserirItem(jogo, x);
    }

    // for (int i = 0; i < 4; i++) vetor[i] = rand() % 9;
    // printf("Vetor original:\n");
    // for (int i = 0; i < 4; i++) printf("%d ", vetor[i]);
    // printf("\n");
    
}