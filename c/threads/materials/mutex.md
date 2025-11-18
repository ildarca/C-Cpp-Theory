# Мьютекс

<details>
<summary>
  pthread_mutex_trylock
</summary>
  

</details>

<details>
<summary>
Рекурсивный мьютекс
</summary>

Что если мы захотели заблокировать мьютекс дважды? Например:

```c
#define THREAD_NUM 8

pthread_mutex_t mutexFuel;
int fuel = 50;

void* routine(void* args) {
  pthread_mutex_lock(&mutexFuel);
  pthread_mutex_lock(&mutexFuel);
  fuel += 50;
  printf("Добабвил топливо. Сейчас: %d\n", fuel);
  pthread_mutex_unlock(&mutexFuel);
  pthread_mutex_unlock(&mutexFuel);
}

int main(int argc, char *argv[]) {
  pthread_t th[THREAD_NUM];
  pthread_mutex_init(&mutexFuel, NULL);
  for (int i = 0; i < THREAD_NUM; i++) {
    if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
      perror("Failed to create thread");
    }
  }

  for (int i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(th[i], NULL) != 0) {
      perror("Failed to join thread");
    }
  }

  printf("Топливо: %d\n", fuel);
  pthread_mutex_destroy(&mutexFuel);
  return 0;
}
```

Программа ничего не распечатает потому что мьютекс уже заблокирован.

Для создания рекурсивных мьютексов используется атрибут `PTHREAD_MUTEX_RECURSIVE`.

Создадим мьютекс с атрибутом:

```c
pthread_mutexattr_t recursiveMutexAttributes;
pthread_mutexattr_init(&recursiveMutexAttributes);
pthread_mutexattr_settype(&recursiveMutexAttributes, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutexFuel, &recursiveMutexAttributes);
```

И не забываем в конце очищать ресурсы атрибута:

```c
pthread_mutexattr_destroy(&recursiveMutexAttributes);
```

**Вывод программы:**

```text
Добабвил топливо. Сейчас: 100
Добабвил топливо. Сейчас: 150
Добабвил топливо. Сейчас: 200
Добабвил топливо. Сейчас: 250
Добабвил топливо. Сейчас: 300
Добабвил топливо. Сейчас: 350
Добабвил топливо. Сейчас: 400
Добабвил топливо. Сейчас: 450
Топливо: 450
```

Мы можеме добавить и больше блокировок, все так же будет работать.

```c
void* routine(void* args) {
  pthread_mutex_lock(&mutexFuel);
  pthread_mutex_lock(&mutexFuel);
  pthread_mutex_lock(&mutexFuel);
  pthread_mutex_lock(&mutexFuel);
  fuel += 50;
  printf("Добабвил топливо. Сейчас: %d\n", fuel);
  pthread_mutex_unlock(&mutexFuel);
  pthread_mutex_unlock(&mutexFuel);
  pthread_mutex_unlock(&mutexFuel);
  pthread_mutex_unlock(&mutexFuel);
}
```

**Замечание:**

- Разблокировать мьютекс нужно столько же раз сколько и заблокировали!
- Мьютекс может быть разблокирован только заблокированным потоком!

Полный код программы доступен в файле [src/mutex_recursive.c](../src/mutex_recursive.c)

</details>
