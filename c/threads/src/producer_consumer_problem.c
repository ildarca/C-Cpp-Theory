#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 10
#define THREAD_NUM 2

int buffer[BUFFER_SIZE];
int count = 0;

pthread_mutex_t mutex;
sem_t semFull;
sem_t semEmpty;

void *producer(void *args) {
  while (1) {
    // Производим
    int x = rand() % 100;

    // Сохраняем в буфер
    sem_wait(&semEmpty);
    pthread_mutex_lock(&mutex);
    buffer[count] = x;
    count++;
    pthread_mutex_unlock(&mutex);
    sem_post(&semFull);
  }
}

void *consumer(void *args) {
  while (1) {
    // Берем результат и уменьшаем счетчик
    sem_wait(&semFull);
    pthread_mutex_lock(&mutex);
    int y = buffer[count - 1];
    count--;
    pthread_mutex_unlock(&mutex);
    sem_post(&semEmpty);

    // Как-то используем
    // Если печатается -1, то данных еще нет
    printf("Использую: %d\n", y);
  }
}

int main() {
  pthread_t th[THREAD_NUM];
  pthread_mutex_init(&mutex, NULL);
  sem_init(&semFull, 0, 0);
  sem_init(&semEmpty, 0, BUFFER_SIZE);
  if (pthread_create(&th[0], NULL, &producer, NULL) != 0) {
    perror("Failed to create thread");
  }

  if (pthread_create(&th[1], NULL, &consumer, NULL) != 0) {
    perror("Failed to create thread");
  }

  for (int i = 0; i < THREAD_NUM; ++i) {
    if (pthread_join(th[i], NULL) != 0) {
       perror("Failed to joint thread");
    }
  }

  sem_destroy(&semFull);
  sem_destroy(&semEmpty);
  pthread_mutex_destroy(&mutex);

  return 0;
}