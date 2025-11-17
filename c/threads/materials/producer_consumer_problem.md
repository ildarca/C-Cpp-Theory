# Проблема производителя потребителя

```mermaid
%%{init: {'theme': 'dark', 'class': {'hideEmptyMembersBox': true}}}%%
classDiagram
direction LR
class Producer
class SR["Shared Resource"]
class Consumer

Producer  --> SR
SR <-- Consumer
```

Производителя и потребителя связывает один общий ресурс. Производитель что-то туда кладет, а потребитель берет. Возникают проблемы:

- Как организовать работу с общей памятью
- Что делать если заполнен буфер
- Что делать если буфер пуст

В нашем примере будут производители, которые генерируют рандомные числа и сохраняют в буфер и потребители, которые будут печатать на экран эти числа.

```c
#define BUFFER_SIZE 10
#define THREAD_NUM 2

int buffer[BUFFER_SIZE];
int count = 0;

void *producer(void *args) {
  while (1) {
    // Производим
    int x = rand() % 100;

    // Сохраняем в буфер
    buffer[count] = x;
    count++;
  }
}

void *consumer(void *args) {
  while (1) {
    // Берем результат и уменьшаем счетчик
    int y = buffer[count - 1];
    count--;

    // Как-то используем
    printf("Использую: %d\n", y);
  }
}
```

Создание и ожидание потоков:

```c
int main() {
  pthread_t th[THREAD_NUM];
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

  return 0;
}
```

На данный момент это программа будет работать с ошибкой. Исправим обращение к общему ресурсу оградив мьютексом.

```c
pthread_mutex_t mutex;

void *producer(void *args) {
  while (1) {
    // Производим
    int x = rand() % 100;

    // Сохраняем в буфер
    pthread_mutex_lock(&mutex);
    buffer[count] = x;
    count++;
    pthread_mutex_unlock(&mutex);
  }
}

void *consumer(void *args) {
  while (1) {
    // Берем результат и уменьшаем счетчик
    pthread_mutex_lock(&mutex);
    int y = buffer[count - 1];
    count--;
    pthread_mutex_unlock(&mutex);

    // Как-то используем
    printf("Использую: %d\n", y);
  }
}

int main() {
  pthread_t th[THREAD_NUM];
  pthread_mutex_init(&mutex, NULL);

  // Код создания потоков и их ожидания

  pthread_mutex_destroy(&mutex);

  return 0;
}
```

Если запустите программу в таком виде, то получите ошибку сегментации. Буфер переполнился. Добавим проверку размеров буффера перед тем как производить какие-либо операции.

```c
void *producer(void *args) {
  while (1) {
    // Производим
    int x = rand() % 100;

    // Сохраняем в буфер
    pthread_mutex_lock(&mutex);
    if (count < BUFFER_SIZE) {
      buffer[count] = x;
      count++;
    } else {
      printf("Буфер заполнен. Пропущено: %d\n",x);
    }
    pthread_mutex_unlock(&mutex);
  }
}

void *consumer(void *args) {
  while (1) {
    int y = -1;
    // Берем результат и уменьшаем счетчик
    pthread_mutex_lock(&mutex);
    if (count > 0) {
      y = buffer[count - 1];
      count--;
    }
    pthread_mutex_unlock(&mutex);

    // Как-то используем
    // Если печатается -1, то данных еще нет
    printf("Использую: %d\n", y);
  }
}
```

Из вывода мы увидим, что у нас все очень плохо, огромное количество значений просто отбрасываются из-за нехватки буфера. А что если это были ваши деньгеи? :)

Здесь лучшим решением является использование семафора. Создадим два семафора, один будет отвечать за количество свободных мест в буфере `semEmpry`, а второй за кол-во готовой продукции `semFull`.

```c
sem_t semFull;
sem_t semEmpty;

int main() {
  pthread_t th[THREAD_NUM];
  pthread_mutex_init(&mutex, NULL);
  sem_init(&semFull, 0, 0);
  sem_init(&semEmpty, 0, 10);

  // Код создания потоков и их ожидания

  sem_destroy(&semFull);
  sem_destroy(&semEmpty);
  pthread_mutex_destroy(&mutex);

  return 0;
}
```

В функциях производителя и потребителя условия уже не нужны, заменим их семафорами.
Итак, прежде чем производить что-то будем смотреть есть ли у нас свободное место через семафору `semEmpty`, если нет, то ждем когда потребитель освободит буфер. После записи значения в буффер, не забываем сообщить через семафор `semFull`, что добавлен еще один элемент.

```c
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
```

Для потребителя аналогично, но наоборот. Сначала проверяем, что буфер не пуст (ждем, если не так) и добавляем свободное место.

```c
void *consumer(void *args) {
  while (1) {
    // Берем результат и уменьшаем счетчик
    sem_wait(&semFull);producer_consumer_problem
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
```

Теперь все работает отлично, можете попробовать добавить задержку для производителя, проверяя, что потребитель ждет пока потребитель произведет.

Полный код программы доступен в файле [src/producer_consumer_problem.c](../src/producer_consumer_problem.c)
