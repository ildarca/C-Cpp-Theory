#include <stdio.h>
#include <pthread.h>

#define THREAD_NUM 4

void* routine(void *args) {

}

int main() {
  pthread_t th[THREAD_NUM];
  int i = 0;
  for (i = 0; i < THREAD_NUM; ++i) {
    if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
      perror("Failed to create thread");
    }
  }

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_join(th[i], NULL) != 0) {
       perror("Failed to joint thread");
    }
  }

  return 0;
}
