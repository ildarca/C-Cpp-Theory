#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_NUM 4

sem_t sem;

void* routine(void *args) {
  int index = *(int*)args;
  int sem_val;
  sem_wait(&sem);
  // Можно исправить добавив задержку, но это очень плохое решение
  // sleep(index + 1);
  sem_getvalue(&sem, &sem_val);
  printf("Sem value: %d\n", sem_val);
  sem_post(&sem);
  free(args);
}

int main() {
  pthread_t th[THREAD_NUM];
  sem_init(&sem, 0, 4);
  int i = 0;
  for (i = 0; i < THREAD_NUM; ++i) {
    int* a = malloc(sizeof(int));
    *a = i;
    if (pthread_create(&th[i], NULL, &routine, a) != 0) {
      perror("Failed to create thread");
    }
  }

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_join(th[i], NULL) != 0) {
       perror("Failed to joint thread");
    }
  }
  sem_destroy(&sem);
  return 0;
}
