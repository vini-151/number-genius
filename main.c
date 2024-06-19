#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TAMANHO_MAXIMO_SEQUENCIA 10 // define qual é o tamanho máximo que a sequência gerada poderá ter
#define TEMPO_EXIBICAO_SEQUENCIA 1 // Tempo de exibição da sequência em segundos
#define TEMPO_MAX_REPOSTA 30 // Tempo limite para resposta do usuário em segundos

// Definições para identificar os jogadores
#define PLAYER_1 2
#define PLAYER_2 1

int sequence[TAMANHO_MAXIMO_SEQUENCIA]; //sequência de números que será gerada ao longo do jogo
int user_input[TAMANHO_MAXIMO_SEQUENCIA]; //entradas do usuário
int tamanhoSequencia = 0;
int contaRodadas = 0;        // Variável global para contar rodadas
int jogadorAtual = PLAYER_1; // Inicia com o jogador 1
sem_t sem_sequence_ready;
sem_t sem_user_ready;
pthread_mutex_t mutex;

// Função de tratamento de sinal para SIGALRM (timeout)

void *gerarSequencia(void *arg) {
    srand(time(NULL));
    while (1) {
        sem_wait(&sem_user_ready);

        pthread_mutex_lock(&mutex);
        sequence[tamanhoSequencia] = rand() % 10; // Gera número aleatório entre 0 e 9 e armazena no vetor de sequência
        tamanhoSequencia++;
        contaRodadas++; // Incrementa o contador de rodadas completas

        // Alternar entre os jogadores a cada rodada
        jogadorAtual = (contaRodadas % 2 == 0) ? PLAYER_1 : PLAYER_2;

        pthread_mutex_unlock(&mutex);
        printf("Sequencia: ");
        sleep(1);
        for (int i = 0; i < tamanhoSequencia; i++) {
            printf("%d ", sequence[i]);
            fflush(stdout);
            sleep(1); // Espera um segundo entre números
        }
        printf("\n");

        // Exibir a sequência por um tempo determinado
        sleep(TEMPO_EXIBICAO_SEQUENCIA);

        // Limpar a tela
        system("clear");

        sem_post(&sem_sequence_ready);
  }
  return NULL;
}

void ultimaChance() {
    // Mudar para o próximo jogador
    jogadorAtual = (jogadorAtual == PLAYER_1) ? PLAYER_2 : PLAYER_1;

    // Permitir que o próximo jogador entre com a sequência
    printf("Jogador %d, hora de mostrar que voce será o vencedor!\nInsira a sequencia que o seu adversário errou: \n", jogadorAtual);
    fflush(stdout);

    for (int i = 0; i < tamanhoSequencia; i++) {
        scanf("%d", &user_input[i]);
    }

    // Verificar se o próximo jogador acertou ou errou
    int correct = 1;
    for (int i = 0; i < tamanhoSequencia; i++) {
        if (user_input[i] != sequence[i]) {
            correct = 0;
            break;
        }
    }

    if (correct) {
        printf("Jogador %d está correto! Jogador %d vence o jogo!\n", jogadorAtual, jogadorAtual);
        exit(0);
    }else{
        printf("Ambos jogadores perderam :(\n Tentem novamente!\n");
        exit(0);
    }
}

void SIGALRM_handler(int sig) {
    printf("\nTempo acabou! :(.\n");

    exit(0);
}

void *get_user_input(void *arg) {
    while (1) {
        sem_wait(&sem_sequence_ready);

        // Configurar o temporizador de 30 segundos
        alarm(TEMPO_MAX_REPOSTA);

        printf("Jogador %d, insira a sequencia (número por número): \n", jogadorAtual);
        fflush(stdout);

        pthread_mutex_lock(&mutex);

        for (int i = 0; i < tamanhoSequencia; i++) {
            scanf("%d", &user_input[i]);
        }

        pthread_mutex_unlock(&mutex);

        // Cancelar o temporizador após receber a entrada do usuário
        alarm(0);

        int correct = 1;
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < tamanhoSequencia; i++) {
            if (user_input[i] != sequence[i]) {
            correct = 0;
            break;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (correct) {
            printf("Jogador %d acertou!\n", jogadorAtual);
            sleep(2);
            system("clear");
        }else{
            system("clear");
            printf("Jogador %d errou!\n", jogadorAtual);

            ultimaChance(); // inicializa a função para dar a última chance para o
                      // próximo jogador (que não errou a seq)
        }

        sem_post(&sem_user_ready);
    }
    return NULL;
}

int main() {
    int regra; //determina se o jogador gostaria de conferir regras de funcionamento

    printf("=== NUMBER GENIUS. ===\n\n");
    printf("0 - Iniciar jogo\n1 - Funcionamento\n");
    scanf("%i", &regra);
    if(regra == 1){
        system("clear");
        printf("\n === Funcionamento ===\n\n");
        printf("- Number Genius é uma releitura do classico Genius, o jogo de memorização.\n");
        printf("- Number Genius funciona a base de números de 0 a 9 do teclado.\n");
        printf("- O jogo é compartilhado com um amigo. Memorizem a sequência que será construída ao longo do jogo para vencerem!\n");
        printf("- A leitura de numeros é feita de uma inserção por vez (insere número -> [enter] -> insere número -> [enter]...\n");
        printf("- Caso um jogador errar, o outro terá a chance de se mostrar o verdadeiro gênio da memorização para ganhar o jogo!\n\n");
        printf("Insira qualquer número quando estiver pronto.\n");
        scanf("%i", &regra);
    }
    system("clear");
    printf("Atencao! O jogo vai começar!\n");
    sleep(3);
    system("clear");
    pthread_t thread1, thread2;

    // Configuração inicial
    sem_init(&sem_sequence_ready, 0, 0);
    sem_init(&sem_user_ready, 0, 1);
    pthread_mutex_init(&mutex, NULL);

    // Configurar tratamento de sinal para SIGALRM
    signal(SIGALRM, SIGALRM_handler);

    // Criar threads
    pthread_create(&thread1, NULL, gerarSequencia, NULL);
    pthread_create(&thread2, NULL, get_user_input, NULL);

    // Loop principal para manter o jogo em execução
    while (1) {
        // Esperar que as threads terminem
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

        // Reiniciar o jogo (reinicializando variáveis, semáforos, mutex, etc.)
        tamanhoSequencia = 0;
        contaRodadas = 0;
        jogadorAtual = PLAYER_1;

        sem_destroy(&sem_sequence_ready);
        sem_destroy(&sem_user_ready);
        pthread_mutex_destroy(&mutex);

        sem_init(&sem_sequence_ready, 0, 0);
        sem_init(&sem_user_ready, 0, 1);
        pthread_mutex_init(&mutex, NULL);

        // Criar novas threads para reiniciar o jogo
        pthread_create(&thread1, NULL, gerarSequencia, NULL);
        pthread_create(&thread2, NULL, get_user_input, NULL);
    }

    return 0;
    
}