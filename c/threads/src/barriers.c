#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define THREAD_NUM 8
#define WIN 1
#define LOST 0

int diceValues[THREAD_NUM];
int status[THREAD_NUM] = {LOST};

pthread_barrier_t barierRollerDice;
pthread_barrier_t barierDefineWinner;

void defineWinner() {
  int max = 0;
  for (int i = 0; i < THREAD_NUM; ++i) {
    if (diceValues[i] > max) {
      max = diceValues[i];
    }
  }

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (diceValues[i] == max) {
      status[i] = WIN;
    }
  }
}

void *rollDice(void* args) {
  int index = *(int*)args;
  diceValues[index] = rand() % 6 + 1;

  pthread_barrier_wait(&barierRollerDice);
  pthread_barrier_wait(&barierDefineWinner);

  if (status[index] == WIN) {
    printf("Я - (%d), получил %d. Победил!\n", index, diceValues[index]);
  } else {
    printf("Я - (%d), получил %d. Проиграл!\n", index, diceValues[index]);
  }
  free(args);
}

int main() {
  srand(time(NULL));
  pthread_t th[THREAD_NUM];
  pthread_barrier_init(&barierRollerDice, NULL, THREAD_NUM + 1);
  pthread_barrier_init(&barierDefineWinner, NULL, THREAD_NUM + 1);
  for (int i = 0; i < THREAD_NUM; ++i) {
    int *index = malloc(sizeof(int));
    *index = i;
    if (pthread_create(&th[i], NULL, &rollDice, index) != 0) {
      perror("Failed to create thread");
    }
  }

  pthread_barrier_wait(&barierRollerDice);
  defineWinner();
  pthread_barrier_wait(&barierDefineWinner);

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_join(th[i], NULL) != 0) {
       perror("Failed to joint thread");
    }
  }
  pthread_barrier_destroy(&barierRollerDice);
  pthread_barrier_destroy(&barierDefineWinner);

  return 0;
}
