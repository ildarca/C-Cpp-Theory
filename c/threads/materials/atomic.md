<details>
<summary>
_Atomic - Спецификатор атомарного типа
</summary>

**Синтаксис:**
```c
_Atomic(type)
_Atomic type_name
```

**Предназначение:**
- Гарантирует **атомарный доступ** к переменной (операция выполняется целиком без прерываний)
- Обеспечивает **порядок памяти** (memory ordering) между потоками
- Запрещает **гонки данных** (data races) при корректном использовании

**Для чего нужен `_Atomic`:**
1. **Безопасность в многопоточности** - предотвращает одновременный доступ к данным
2. **Отсутствие гонок данных** - операции чтения/записи атомарны
3. **Правильный порядок памяти** - компилятор и процессор не переупорядочивают операции
4. **Кросс-платформенность** - работает на всех архитектурах

**Примеры использования:**
```c
// Различные способы объявления атомарных переменных
_Atomic int atomic_counter;
_Atomic(long) atomic_long;
_Atomic(int*) atomic_ptr;

// С использованием макроса (более читаемый вариант)
#include <stdatomic.h>
atomic_int counter;
atomic_bool flag;
atomic_size_t size;
```

</details>

<details>
<summary>
atomic_init() - Инициализация атомарной переменной
</summary>

```c
void atomic_init(volatile A* obj, C desired);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную
- `desired` - **начальное значение**

**Предназначение:**
- Инициализация атомарной переменной
- Может использоваться для инициализации переменных с статической длительностью хранения

**Пример:**
```c
atomic_int counter;
atomic_init(&counter, 0);

// Альтернатива - макрос ATOMIC_VAR_INIT
atomic_int counter2 = ATOMIC_VAR_INIT(42);
```

**Примечание:**
- Не использовать для уже инициализированных переменных
- Для статических переменных лучше использовать `ATOMIC_VAR_INIT`

</details>

<details>
<summary>
atomic_store() - Атомарная запись значения
</summary>

```c
void atomic_store(volatile A* obj, C desired);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную  
- `desired` - **значение** для записи

**Предназначение:**
- Атомарная запись значения в переменную
- Гарантирует, что операция записи выполняется целиком

**Пример:**
```c
atomic_int counter;
atomic_store(&counter, 42);  // Атомарно устанавливает значение 42

// Эквивалент с явным указанием порядка памяти
atomic_store_explicit(&counter, 42, memory_order_seq_cst);
```

**Порядок памяти:**
- По умолчанию использует `memory_order_seq_cst`
- Можно указать явно через `atomic_store_explicit()`

</details>

<details>
<summary>
atomic_load() - Атомарное чтение значения
</summary>

```c
C atomic_load(volatile A* obj);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную

**Возвращает:**
- Текущее значение атомарной переменной

**Предназначение:**
- Атомарное чтение значения переменной
- Гарантирует, что операция чтения выполняется атомарно

**Пример:**
```c
atomic_int counter = ATOMIC_VAR_INIT(100);
int value = atomic_load(&counter);  // Атомарно читает значение
printf("Counter value: %d\n", value);

// С явным порядком памяти
value = atomic_load_explicit(&counter, memory_order_acquire);
```

**Порядок памяти:**
- По умолчанию использует `memory_order_seq_cst`
- Можно указать явно через `atomic_load_explicit()`

</details>

<details>
<summary>
atomic_exchange() - Атомарная замена значения
</summary>

```c
C atomic_exchange(volatile A* obj, C desired);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную
- `desired` - **новое значение**

**Возвращает:**
- Предыдущее значение переменной

**Предназначение:**
- Атомарно заменяет значение переменной
- Возвращает старое значение
- Полезно для реализации спин-блокировок и мьютексов

**Пример:**
```c
atomic_int lock = ATOMIC_VAR_INIT(0);

// Попытка захвата блокировки
int old = atomic_exchange(&lock, 1);
if (old == 0) {
    // Блокировка захвачена
    // Критическая секция...
    atomic_store(&lock, 0);  // Освобождение
}
```

</details>

<details>
<summary>
atomic_compare_exchange_strong() - Сравнение и замена (сильная версия)
</summary>

```c
_Bool atomic_compare_exchange_strong(volatile A* obj, 
                                   C* expected, 
                                   C desired);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную
- `expected` - **указатель** на ожидаемое значение
- `desired` - **желаемое значение** для установки

**Возвращает:**
- `true` если значение было равно `*expected` и установлено в `desired`
- `false` в противном случае, при этом `*expected` обновляется текущим значением

**Предназначение:**
- Атомарная операция "сравнить и поменять" (CAS)
- Фундаментальная операция для многих lock-free алгоритмов
- Сильная версия менее подвержена ложным срабатываниям

**Пример:**
```c
atomic_int counter = ATOMIC_VAR_INIT(5);
int expected = 5;

if (atomic_compare_exchange_strong(&counter, &expected, 10)) {
    // Успешно: counter был 5, теперь стал 10
    printf("CAS succeeded, counter is now 10\n");
} else {
    // Неудача: counter не был равен 5, expected содержит текущее значение
    printf("CAS failed, counter is %d, expected was 5\n", expected);
}
```

</details>

<details>
<summary>
atomic_compare_exchange_weak() - Сравнение и замена (слабая версия)
</summary>

```c
_Bool atomic_compare_exchange_weak(volatile A* obj, 
                                 C* expected, 
                                 C desired);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную
- `expected` - **указатель** на ожидаемое значение  
- `desired` - **желаемое значение** для установки

**Возвращает:**
- `true` если значение было равно `*expected` и установлено в `desired`
- `false` в противном случае, при этом `*expected` может обновиться текущим значением

**Предназначение:**
- Слабая версия CAS операции
- Может sporadically fail даже если значение равно ожидаемому
- Более эффективна в циклах на некоторых архитектурах

**Пример:**
```c
atomic_int counter = ATOMIC_VAR_INIT(0);
int expected;

do {
    expected = atomic_load(&counter);
} while (!atomic_compare_exchange_weak(&counter, &expected, expected + 1));
// Потокобезопасный инкремент в цикле
```

</details>

<details>
<summary>
atomic_fetch_add() - Атомарное сложение
</summary>

```c
C atomic_fetch_add(volatile A* obj, M operand);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную
- `operand` - **значение** для прибавления

**Возвращает:**
- Значение переменной до прибавления

**Предназначение:**
- Атомарное прибавление значения к переменной
- Возвращает предыдущее значение
- Идеально для счетчиков и накопителей

**Пример:**
```c
atomic_int counter = ATOMIC_VAR_INIT(0);

// Потокобезопасный инкремент
int old_value = atomic_fetch_add(&counter, 1);
printf("Counter was %d, now is %d\n", old_value, old_value + 1);

// Прибавление большего значения
atomic_fetch_add(&counter, 5);  // Увеличивает на 5
```

</details>

<details>
<summary>
atomic_fetch_sub() - Атомарное вычитание
</summary>

```c
C atomic_fetch_sub(volatile A* obj, M operand);
```

**Параметры:**
- `obj` - **указатель** на атомарную переменную  
- `operand` - **значение** для вычитания

**Возвращает:**
- Значение переменной до вычитания

**Предназначение:**
- Атомарное вычитание значения из переменной
- Возвращает предыдущее значение
- Полезно для счетчиков и ограничителей

**Пример:**
```c
atomic_int credits = ATOMIC_VAR_INIT(100);

// Использование кредита
int remaining = atomic_fetch_sub(&credits, 10);
printf("Used 10 credits, %d remaining\n", remaining - 10);

// Проверка достаточности кредитов
if (atomic_load(&credits) >= 5) {
    atomic_fetch_sub(&credits, 5);
}
```

</details>

<details>
<summary>
Порядок памяти (Memory Ordering)
</summary>

**Уровни порядка памяти:**

```c
typedef enum memory_order {
    memory_order_relaxed,  // Только атомарность
    memory_order_acquire,  // Загрузка с приобретением
    memory_order_release,  // Сохранение с освобождением  
    memory_order_acq_rel,  // И приобретение и освобождение
    memory_order_seq_cst   // Строгая последовательность (по умолчанию)
} memory_order;
```

**Описание:**

- `memory_order_relaxed` - только атомарность, без гарантий порядка операций
- `memory_order_acquire` - гарантирует, что последующие операции видят изменения
- `memory_order_release` - гарантирует, что предыдущие операции завершены
- `memory_order_seq_cst` - строгий порядок (самый консервативный)

**Примеры:**
```c
atomic_int data = ATOMIC_VAR_INIT(0);
atomic_bool ready = ATOMIC_VAR_INIT(0);

// Поток-писатель
atomic_store_explicit(&data, 42, memory_order_relaxed);
atomic_store_explicit(&ready, 1, memory_order_release);

// Поток-читатель  
if (atomic_load_explicit(&ready, memory_order_acquire)) {
    int value = atomic_load_explicit(&data, memory_order_relaxed);
    // Гарантированно увидит value = 42
}
```

</details>