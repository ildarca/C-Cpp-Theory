#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define THREAD_COUNT 2

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int fuel;

void *fuel_filling(void *arg) {
  for (int i = 0; i < 5; i++) {
    pthread_mutex_lock(&mutexFuel);
    fuel += 15;
    printf("Добавлено топливо. Сейчас: %d\n", fuel);
    pthread_cond_signal(&condFuel);  // Отправка сигнала машине
    pthread_mutex_unlock(&mutexFuel);
    sleep(1);
  }
}

void *car(void *arg) {
  pthread_mutex_lock(&mutexFuel);
  while (fuel < 40) {
    printf("Топлива недостаточно. Ждем заправки\n");
    pthread_cond_wait(&condFuel, &mutexFuel); // Добавили ожидание сигнала
  }
  fuel -= 40;
  printf("Потратил. Осталось топлива: %d\n", fuel);
  pthread_mutex_unlock(&mutexFuel);
}

int main() {
  pthread_t th[2];
  pthread_mutex_init(&mutexFuel, NULL);
  pthread_cond_init(&condFuel, NULL);

  for (int i = 0; i < THREAD_COUNT; i++) {
    if (i == 1) {
      if (pthread_create(&th[i], NULL, fuel_filling, NULL) != 0) {
        perror("Failed to create thread");
      }
    } else {
      if (pthread_create(&th[i], NULL, car, NULL) != 0) {
        perror("Failed to create thread");
      }
    }
  }

  for (int i = 0; i < THREAD_COUNT; i++) {
    if (pthread_join(th[i], NULL) != 0) {
      perror("Failed to join thread");
    }
  }

  pthread_mutex_init(&mutexFuel, NULL);
  pthread_cond_destroy(&condFuel);
  return 0;
}