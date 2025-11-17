#include <stdio.h>
#include <pthread.h>

#define THREAD_NUM 3

pthread_barrier_t barier;

void* routine(void *args) {
  printf("Поток: я жду остальных!\n");
  pthread_barrier_wait(&barier);
  printf("Поток: я свободен!\n");
}

int main() {
  pthread_t th[THREAD_NUM];
  pthread_barrier_init(&barier, NULL, THREAD_NUM+1);
  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
      perror("Failed to create thread");
    }
  }

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_join(th[i], NULL) != 0) {
       perror("Failed to joint thread");
    }
  }
  pthread_barrier_destroy(&barier);

  return 0;
}
