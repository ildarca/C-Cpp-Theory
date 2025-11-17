#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define THREAD_NUM 4
#define MAX_PLAYER_COUNT 3

sem_t semaphore;

void* routine(void* args) {
  printf("(%d) Жду своей очереди!\n", *(int*)args);
  sem_wait(&semaphore);
  printf("(%d) Вошел в игру!\n", *(int*)args);
  sleep(rand() % 5 + 1);
  printf("(%d) Наигрался, выхожу из игры!\n", *(int*)args);
  sem_post(&semaphore);
  free(args);
}

int main() {
  pthread_t th[THREAD_NUM];
  sem_init(&semaphore, 0, MAX_PLAYER_COUNT);

  for (int i = 0; i < THREAD_NUM; ++i) {
    int *index = malloc(sizeof(int));
    *index = i;
    if (pthread_create(&th[i], NULL, &routine, index) != 0) {
      perror("Failed to create thread");
    }
  }

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_join(th[i], NULL) != 0) {
       perror("Failed to joint thread");
    }
  }

  sem_destroy(&semaphore);
  return 0;
}
