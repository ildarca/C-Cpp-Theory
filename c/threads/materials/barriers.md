# Барьер

**Барьер** - это механизм синхронизаци, который позволяет приостановить выполнение потоков у некоторой точкии программы до тех пор, пока все потоки не дойдут до этого места, и только затем продолжить выполнение дальше.

Принцип работы:

- Мы определяем количество потоков для барьера
- Потоки выполняют свою работу независимо до достижения барьера
- Когда поток достигает барьера, он блокируется и ждет
- Как только заданное количество потоков достигнет барьера, все они одновременно разблокируются и продолжают выполнение

Представьте группу туристов, идущих в горы. Они договариваются встретиться в определенной точке. Каждый идет своим темпом, но когда все соберутся в точке встречи, они продолжают путь вместе.

Для создания переменной барьера используется тип `phread_barier_t`. Рассмотрим функции доступные для работы с барьером.

<details>
<summary>
pthread_barrier_init() - Инициализация барьера
</summary>

```c
int pthread_barrier_init(pthread_barrier_t *restrict barrier,
                        const pthread_barrierattr_t *restrict attr,
                        unsigned count);
```

**Параметры:**

- `barrier` - **указатель** на структуру `pthread_barrier_t` для инициализации
- `attr` - **атрибуты** барьера (NULL для значений по умолчанию)
- `count` - **количество потоков**, которые должны достичь барьера перед продолжением

**Предназначение:**

- Динамическая инициализация барьера
- Устанавливает количество потоков, которые должны встретиться в барьере
- Должна быть вызвана перед использованием

**Возвращает:**

- `0` - успех
- `EINVAL` - неверные параметры (count = 0 или некорректные атрибуты)
- `ENOMEM` - недостаточно памяти
- `EAGAIN` - недостаточно ресурсов системы для создания барьера
- `EBUSY` - попытка переинициализации уже инициализированного барьера

</details>

<details>
<summary>
pthread_barrier_destroy() - Уничтожение барьера
</summary>

**Сигнатура:**

```c
int pthread_barrier_destroy(pthread_barrier_t *barrier);
```

**Параметры:**

- `barrier` - **барьер** для уничтожения

**Предназначение:**

- Освобождение ресурсов барьера
- Должна быть вызвана когда барьер больше не нужен
- Барьер не должен использоваться потоками во время уничтожения

**Возвращает:**

- `0` - успех
- `EBUSY` - барьер еще используется потоками
- `EINVAL` - неверный параметр (barrier не инициализирован или уже уничтожен)

</details>

<details>
<summary>
PTHREAD_BARRIER_SERIAL_THREAD - Специальное возвращаемое значение
</summary>

```c
#define PTHREAD_BARRIER_SERIAL_THREAD
```

**Предназначение:**

- Специальное значение, возвращаемое **одному произвольному потоку** после достижения барьера всеми потоками
- Обычно используется для выполнения специальных действий одним потоком (например, обновление общих данных)

**Использование:**

```c
int result = pthread_barrier_wait(&barrier);
if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
    // Этот поток выбран для выполнения специальных действий
    update_shared_data();
}
```

</details>

<details>
<summary>
pthread_barrier_wait() - Ожидание на барьере
</summary>

**Сигнатура:**

```c
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

**Параметры:**

- `barrier` - **барьер** на котором ожидать

**Предназначение:**

- Блокирует поток до тех пор, пока указанное количество потоков не вызовут эту функцию
- Когда последний поток достигает барьера, все ожидающие потоки разблокируются
- **Один произвольный поток** получает `PTHREAD_BARRIER_SERIAL_THREAD`
- **Остальные потоки** получают `0`

**Возвращает:**

- `PTHREAD_BARRIER_SERIAL_THREAD` - для одного произвольного потока
- `0` - для остальных потоков
- `EINVAL` - неверный параметр (barrier не инициализирован)

</details>

<details>
<summary>
Пример использования барьера
</summary>

Создадим два потока, барьер и функцию для потоков.

```c
#include <stdio.h>
#include <pthread.h>

#define THREAD_NUM 3

// Переменная барьера
pthread_barrier_t barier;

void* routine(void *args) {
}

int main() {
  pthread_t th[THREAD_NUM];

  // Инициализация барьера для двух потоков
  pthread_barrier_init(&barier, NULL, THREAD_NUM);

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

  // Уничтожение барьера
  pthread_barrier_destroy(&barier);
  return 0;
}
```

Используем функцию ожидания потоков `pthread_barrier_wait`, выведем перед этим сообщение `"Поток: я жду остальных!"`, а после `"Поток: я свободен!"`.

```c
void* routine(void *args) {
  printf("Поток: я жду остальных!\n");
  pthread_barrier_wait(&barier);
  printf("Поток: я свободен!\n");
}
```

**Вывод программы:**

```text
Поток: я жду остальных!
Поток: я жду остальных!
Поток: я жду остальных!
Поток: я свободен!
Поток: я свободен!
Поток: я свободен!
```

Как видим сначала все потоки дошли до барьера, а после были освобождены. Обратите внимание что количество потоков программы должно быть **не меньше** заданного при создании барьера. Иначе программа просто зависнет. Например, как в этом случае:

```c
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
  pthread_barrier_init(&barier, NULL, 4); // Установили ожидание 4-ех потоков
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
```

Здесь мы установили ожадение 4-ех потокв, но у нас всего их 3 запущенно с функцией `routine`.

**Вывод программы:**

```c
Поток: я жду остальных!
Поток: я жду остальных!
Поток: я жду остальных!
```

Весь исходный код доступен в файле [src/barriers_simple.c](../src/barriers_simple.c)

</details>

<details>
<summary>
Пример посложнее
</summary>

Напишем программу-игру. Каждый дочерний поток будет бросать кубик. Основной поток(функция `main`) будет определять победителя следующим образом: искать максимальное выпавшее число, и все потоки с таким числом будут победителями. Каждый поток сам будет сообщать о том, что он победил.

Для хранения выпавших очков создадаим массив `int diceValues[THREAD_NUM]`, а для статистики выигрыша массив `int status[THREAD_NUM]`. Победители будут иметь статус `WIN`, проигравшие - `LOST`

```c
#define THREAD_NUM 8
#define WIN 1
#define LOST 0

int diceValues[THREAD_NUM];
int status[THREAD_NUM] = {LOST};
```

В главной функции создадим `THREAD_NUM` потоков и их ожидание.

```c
int main() {
  srand(time(NULL));
  pthread_t th[THREAD_NUM];

  for (int i = 0; i < THREAD_NUM; ++i) {
    int *index = malloc(sizeof(int));
    *index = i;
    if (pthread_create(&th[i], NULL, &rollDice, index) != 0) {
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
```

Определим функцию для дочерних потоков, которая будет кидать кубик (генерировать рандомное число от 1 до 6) и печатать выиграл ли он. В качестве параметра будет принимать индекс потока.

```c
void *rollDice(void* args) {
  int index = *(int*)args;
  diceValues[index] = rand() % 6 + 1;
  if (status[index] == 1) {
    printf("Я - (%d), получил %d. Победил!\n", index, diceValues[index]);
  } else {
    printf("Я - (%d), получил %d. Проиграл!\n", index, diceValues[index]);
  }
  free(args);
}
```

В главную функции между созданием и ожиданием потоков будем вызывать функцию определения победителей `defineWinner`.

```c
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
```

В `main` просто вызываем эту функцию.

```c
// код создания потоков

defineWinner()

// код ожидания завершения потоков
```

**Вывод программы:**

```c
Я - (0), получил 3. Проиграл!
Я - (1), получил 3. Проиграл!
Я - (2), получил 1. Проиграл!
Я - (3), получил 6. Проиграл!
Я - (4), получил 4. Проиграл!
Я - (5), получил 5. Проиграл!
Я - (6), получил 1. Проиграл!
Я - (7), получил 4. Проиграл!
```

После запуска получаем такой странный вывод. Но ведь должен был быть хотя бы один победитель. Проблема связана с гонкой данных и неправильным порядком выполнения операций. Главный поток после создания дочерних сразу вызывает функцию вычисления победителя, однако потоки еще не успели кинуть кубик. И вывод потоками сообщения должен осуществляться точно после определения статусов.

Для решения проблемы подойдут барьеры. Мы создадим барьер наа `THREAD_NUM + 1` поток (учитывая глвный `main`). Дочерние потоки после того как кинут кубик должны будут ожидать пока главный поток не посчитает статус участников. А после подсчета уже дочерние потоки продолжат свое выполнение и выведут свой статус.

```c
pthread_barrier_t barierRollerDice;

// ...

void *rollDice(void* args) {
  int index = *(int*)args;
  diceValues[index] = rand() % 6 + 1;

  // ждем когда все бросят кубик
  pthread_barrier_wait(&barierRollerDice);

  if (status[index] == WIN) {
    printf("Я - (%d), получил %d. Победил!\n", index, diceValues[index]);
  } else {
    printf("Я - (%d), получил %d. Проиграл!\n", index, diceValues[index]);
  }
  free(args);
}

int main() {
  pthread_barrier_init(&barierRollerDice, NULL, THREAD_NUM + 1);
  // код создания потоков

  // ждем когда все бросят кубик
  pthread_barrier_wait(&barierRollerDice);
  defineWinner();

  // код ожидания завершения потоков
  pthread_barrier_destroy(&barierRollerDice);
}
```

**Вывод программы, при первом запуске:**

```text
Я - (0), получил 2. Проиграл!
Я - (5), получил 1. Проиграл!
Я - (7), получил 1. Проиграл!
Я - (6), получил 2. Проиграл!
Я - (1), получил 6. Победил!
Я - (3), получил 3. Проиграл!
Я - (4), получил 4. Проиграл!
Я - (2), получил 1. Проиграл!
```

**Вывод программы, при втором запуске:**

```text
Я - (0), получил 1. Проиграл!
Я - (1), получил 6. Проиграл!
Я - (2), получил 2. Проиграл!
Я - (5), получил 1. Проиграл!
Я - (6), получил 4. Проиграл!
Я - (4), получил 1. Проиграл!
Я - (3), получил 3. Проиграл!
Я - (7), получил 1. Проиграл!
```

Мы исправили ситуацию, когда главный поток вычислял результаты до генерации чисел, но появилась новая проблема - во втором запуске поток с числом 6 проиграл, хотя должен был победить. Теперь дочерние потоки выводят статистику на экран до того как главный поток успел её посчитать.

Для решения добавим еще один барьер `pthread_barrier_t barierDefineWinner`. Дочерние потоки перед выводом будут ждать когда главный поток посчитает статистику победителей.

```c
pthread_barrier_t barierRollerDice;
pthread_barrier_t barierDefineWinner;

// ...

void *rollDice(void* args) {
  int index = *(int*)args;
  diceValues[index] = rand() % 6 + 1;

  // ждем когда все бросят кубик
  pthread_barrier_wait(&barierRollerDice);

  // ждем когда главный поток посчитает статистику
  pthread_barrier_wait(&barierRollerDice);

  if (status[index] == WIN) {
    printf("Я - (%d), получил %d. Победил!\n", index, diceValues[index]);
  } else {
    printf("Я - (%d), получил %d. Проиграл!\n", index, diceValues[index]);
  }
  free(args);
}

int main() {
  pthread_barrier_init(&barierRollerDice, NULL, THREAD_NUM + 1);
  pthread_barrier_init(&barierDefineWinner, NULL, THREAD_NUM + 1);
  // код создания потоков

  // ждем когда все бросят кубик
  pthread_barrier_wait(&barierRollerDice);
  defineWinner();

  // досчитали статистику
  pthread_barrier_wait(&barierRollerDice);

  // код ожидания завершения потоков
  pthread_barrier_destroy(&barierRollerDice);
  pthread_barrier_destroy(&barierDefineWinner);
}
```

**Вывод программы:**

```text
Я - (0), получил 2. Проиграл!
Я - (5), получил 1. Проиграл!
Я - (6), получил 5. Победил!
Я - (2), получил 3. Проиграл!
Я - (3), получил 3. Проиграл!
Я - (1), получил 2. Проиграл!
Я - (7), получил 4. Проиграл!
Я - (4), получил 4. Проиграл!
```

Теперь все в порядке и у нас программа работает корректно.

Полный код программы доступен в файле [src/barriers.c](../src/barriers.c)

</details>
