# Функции работы со строками (string.h)

## Функции работы с памятью

<details>
<summary>
memchr - поиск символа в памяти
</summary>

**Описание:**  
Ищет символ `c` в первых `n` байтах области памяти, на которую указывает `str`.

**Возвращаемое значение:**  
- Указатель на найденный байт
- `NULL` если символ не найден

**Крайние случаи:**
- `str == NULL` → Segmentation fault
- `str` не инициализирован → UB

**Пример:**
```c
char str[] = "Hello World";
char *result = memchr(str, 'W', 11);
printf("%s\n", result); // "World"
```

</details>

<details>
<summary>
memcmp - сравнение областей памяти
</summary>

**Описание:**  
Сравнивает первые `n` байтов областей памяти `str1` и `str2`.

**Возвращаемое значение:**
- `< 0` если str1 < str2
- `> 0` если str1 > str2  
- `= 0` если str1 == str2

**Крайние случаи:**
- `str1` или `str2 == NULL` → Segmentation fault
- `str1` или `str2` не инициализированы → UB

**Пример:**
```c
char str1[] = "abc";
char str2[] = "abd";
int result = memcmp(str1, str2, 3);
printf("%d\n", result); // -1
```

</details>

<details>
<summary>
memcpy - копирование памяти
</summary>

**Описание:**  
Копирует `n` байтов из области памяти `src` в область памяти `dest`.

**Возвращаемое значение:**  
Указатель на `dest`

**Крайние случаи:**
- `dest` или `src == NULL` → Segmentation fault
- Области памяти пересекаются → UB
- Размер `src` > `dest` и `n` > длины `dest` → UB

**Пример:**
```c
char src[] = "Hello";
char dest[10];
memcpy(dest, src, 6);
printf("%s\n", dest); // "Hello"
```

</details>

<details>
<summary>
memmove - копирование с перекрытием
</summary>

**Описание:**  
Копирует `n` байтов из `src` в `dest`. Области памяти могут перекрываться.

**Возвращаемое значение:**  
Указатель на `dest`

**Крайние случаи:**
- `dest` или `src == NULL` → Segmentation fault

**Пример:**
```c
char str[] = "memmove can be very useful";
memmove(str + 20, str + 15, 11);
printf("%s\n", str);
```

</details>

<details>
<summary>
memset - заполнение памяти
</summary>

**Описание:**  
Заполняет первые `n` байтов области памяти `str` байтом `c`.

**Возвращаемое значение:**  
Указатель на `str`

**Крайние случаи:**
- `str == NULL` → Segmentation fault
- `str` не инициализирован → UB

**Пример:**
```c
char buffer[10];
memset(buffer, 'A', 9);
buffer[9] = '\0';
printf("%s\n", buffer); // "AAAAAAAAA"
```

</details>

## Функции работы со строками

<details>
<summary>
strcat - конкатенация строк
</summary>

**Описание:**  
Добавляет строку `src` к строке `dest`.

**Возвращаемое значение:**  
Указатель на `dest`

**Крайние случаи:**
- `dest` или `src == NULL` → Segmentation fault
- В `dest` недостаточно места → UB

**Пример:**
```c
char dest[20] = "Hello";
char src[] = " World";
strcat(dest, src);
printf("%s\n", dest); // "Hello World"
```

</details>

<details>
<summary>
strncat - конкатенация n символов
</summary>

**Описание:**  
Добавляет не более `n` символов из `src` к `dest`.

**Возвращаемое значение:**  
Указатель на `dest`

**Крайние случаи:**
- `dest` или `src == NULL` → Segmentation fault
- В `dest` недостаточно места → UB

**Пример:**
```c
char dest[10] = "Hello";
char src[] = " World";
strncat(dest, src, 3);
printf("%s\n", dest); // "Hello Wo"
```

</details>

<details>
<summary>
strchr - поиск первого вхождения символа
</summary>

**Описание:**  
Ищет первое вхождение символа `c` в строке `str`.

**Возвращаемое значение:**
- Указатель на найденный символ
- `NULL` если не найден

**Крайние случаи:**
- `str == NULL` → Segmentation fault
- `str` не инициализирован → UB

**Пример:**
```c
char str[] = "Hello World";
char *result = strchr(str, 'o');
printf("%s\n", result); // "o World"
```

</details>

<details>
<summary>
strrchr - поиск последнего вхождения символа
</summary>

**Описание:**  
Ищет последнее вхождение символа `c` в строке `str`.

**Возвращаемое значение:**
- Указатель на найденный символ
- `NULL` если не найден

**Крайние случаи:**
- `str == NULL` → Segmentation fault
- `str` не инициализирован → UB

**Пример:**
```c
char str[] = "Hello World";
char *result = strrchr(str, 'o');
printf("%s\n", result); // "orld"
```

</details>

<details>
<summary>
strcmp - сравнение строк
</summary>

**Описание:**  
Сравнивает строки `str1` и `str2`.

**Возвращаемое значение:**
- `< 0` если str1 < str2
- `> 0` если str1 > str2
- `= 0` если str1 == str2

**Крайние случаи:**
- `str1` или `str2 == NULL` → Segmentation fault

**Пример:**
```c
char str1[] = "abc";
char str2[] = "abd";
int result = strcmp(str1, str2);
printf("%d\n", result); // -1
```

</details>

<details>
<summary>
strncmp - сравнение n символов
</summary>

**Описание:**  
Сравнивает не более `n` символов строк `str1` и `str2`.

**Возвращаемое значение:**
- `< 0` если str1 < str2
- `> 0` если str1 > str2  
- `= 0` если str1 == str2

**Крайние случаи:**
- `str1` или `str2 == NULL` → Segmentation fault

**Пример:**
```c
char str1[] = "abc";
char str2[] = "abd";
int result = strncmp(str1, str2, 2);
printf("%d\n", result); // 0
```

</details>

<details>
<summary>
strcpy - копирование строки
</summary>

**Описание:**  
Копирует строку `src` в `dest` включая `\0`.

**Возвращаемое значение:**  
Указатель на `dest`

**Крайние случаи:**
- `dest` или `src == NULL` → Segmentation fault
- В `dest` недостаточно места → UB

**Пример:**
```c
char src[] = "Hello";
char dest[10];
strcpy(dest, src);
printf("%s\n", dest); // "Hello"
```

</details>

<details>
<summary>
strncpy - копирование n символов
</summary>

**Описание:**  
Копирует не более `n` символов из `src` в `dest`.

**Возвращаемое значение:**  
Указатель на `dest`

**Крайние случаи:**
- `dest` или `src == NULL` → Segmentation fault
- В `dest` недостаточно места → UB

**Пример:**
```c
char src[] = "Hello";
char dest[10];
strncpy(dest, src, 3);
dest[3] = '\0';
printf("%s\n", dest); // "Hel"
```

</details>

<details>
<summary>
strcspn - длина начального сегмента
</summary>

**Описание:**  
Вычисляет длину начального сегмента `str1`, не содержащего символов из `str2`.

**Возвращаемое значение:**  
Длина сегмента

**Крайние случаи:**
- `str1` или `str2 == NULL` → Segmentation fault

**Пример:**
```c
char str1[] = "abc123";
char str2[] = "123";
size_t len = strcspn(str1, str2);
printf("%zu\n", len); // 3
```

</details>

<details>
<summary>
strspn - длина сегмента из разрешенных символов
</summary>

**Описание:**  
Вычисляет длину начального сегмента `str1`, состоящего только из символов `str2`.

**Возвращаемое значение:**  
Длина сегмента

**Крайние случаи:**
- `str1` или `str2 == NULL` → Segmentation fault

**Пример:**
```c
char str1[] = "129th";
char str2[] = "1234567890";
size_t len = strspn(str1, str2);
printf("%zu\n", len); // 3
```

</details>

<details>
<summary>
strpbrk - поиск любого символа из набора
</summary>

**Описание:**  
Ищет первое вхождение любого символа из `str2` в `str1`.

**Возвращаемое значение:**
- Указатель на найденный символ
- `NULL` если не найден

**Крайние случаи:**
- `str1` или `str2 == NULL` → Segmentation fault

**Пример:**
```c
char str1[] = "Hello World";
char str2[] = "od";
char *result = strpbrk(str1, str2);
printf("%s\n", result); // "o World"
```

</details>

<details>
<summary>
strstr - поиск подстроки
</summary>

**Описание:**  
Ищет первое вхождение подстроки `needle` в `haystack`.

**Возвращаемое значение:**
- Указатель на начало подстроки
- `NULL` если не найдена

**Крайние случаи:**
- `needle` или `haystack == NULL` → Segmentation fault
- `needle` пустая строка → возвращает `haystack`

**Пример:**
```c
char haystack[] = "Hello World";
char needle[] = "World";
char *result = strstr(haystack, needle);
printf("%s\n", result); // "World"
```

</details>

<details>
<summary>
strtok - разбиение на токены
</summary>

**Описание:**  
Разбивает строку `str` на токены по разделителям `delim`.

**Возвращаемое значение:**
- Указатель на следующий токен
- `NULL` если токенов больше нет

**Крайние случаи:**
- `str` или `delim == NULL` → Segmentation fault
- `str` не массив, а `char*` → Segmentation fault

**Пример:**
```c
char str[] = "Hello,World,Test";
char *token = strtok(str, ",");
while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, ",");
}
// Вывод:
// Hello
// World  
// Test
```

</details>

## Функции с динамической памятью

<details>
<summary>
strdup - дублирование строки
</summary>

**Описание:**  
Создает копию строки `str` в динамической памяти.

**Возвращаемое значение:**  
- Указатель на новую строку
- `NULL` если не хватило памяти

**Крайние случаи:**
- `str == NULL` → UB (зависит от реализации)
- Нехватка памяти → возвращает `NULL`

**Пример:**
```c
char *original = "Hello";
char *copy = strdup(original);
printf("%s\n", copy); // "Hello"
free(copy); // Не забываем освободить!
```

</details>

<details>
<summary>
strndup - дублирование n символов
</summary>

**Описание:**  
Создает копию первых `n` символов строки `str` в динамической памяти.

**Возвращаемое значение:**  
- Указатель на новую строку
- `NULL` если не хватило памяти

**Крайние случаи:**
- `str == NULL` → UB
- Нехватка памяти → возвращает `NULL`

**Пример:**
```c
char *original = "Hello World";
char *copy = strndup(original, 5);
printf("%s\n", copy); // "Hello"
free(copy);
```

</details>

## Вспомогательные функции

<details>
<summary>
strerror - описание ошибки
</summary>

**Описание:**  
Возвращает строковое описание кода ошибки `errnum`.

**Возвращаемое значение:**  
Указатель на строку с описанием ошибки

**Пример:**
```c
printf("%s\n", strerror(ENOENT)); // "No such file or directory"
```

<details>
<summary>
Коды ошибок (errno)
</summary>

**Распространенные коды ошибок:**

| Код | Константа | Описание |
|-----|-----------|----------|
| 1 | `EPERM` | Операция запрещена |
| 2 | `ENOENT` | Нет такого файла или директории |
| 4 | `EINTR` | Системный вызов прерван |
| 5 | `EIO` | Ошибка ввода/вывода |
| 6 | `ENXIO` | Нет такого устройства или адреса |
| 7 | `E2BIG` | Слишком длинный список аргументов |
| 8 | `ENOEXEC` | Ошибка формата исполняемого файла |
| 9 | `EBADF` | Неверный файловый дескриптор |
| 10 | `ECHILD` | Нет дочерних процессов |
| 11 | `EAGAIN` | Ресурс временно недоступен |
| 12 | `ENOMEM` | Недостаточно памяти |
| 13 | `EACCES` | Доступ запрещен |
| 14 | `EFAULT` | Неверный адрес |
| 15 | `ENOTBLK` | Требуется блочное устройство |
| 16 | `EBUSY` | Устройство или ресурс занято |
| 17 | `EEXIST` | Файл уже существует |
| 18 | `EXDEV` | Ссылка на другое устройство |
| 19 | `ENODEV` | Нет такого устройства |
| 20 | `ENOTDIR` | Не является директорией |
| 21 | `EISDIR` | Это директория |
| 22 | `EINVAL` | Неверный аргумент |
| 23 | `ENFILE` | Слишком много открытых файлов в системе |
| 24 | `EMFILE` | Слишком много открытых файлов |
| 25 | `ENOTTY` | Неверная операция ввода/вывода |
| 26 | `ETXTBSY` | Текстовый файл занят |
| 27 | `EFBIG` | Файл слишком велик |
| 28 | `ENOSPC` | Нет свободного места на устройстве |
| 29 | `ESPIPE` | Неверное позиционирование |
| 30 | `EROFS` | Файловая система только для чтения |
| 31 | `EMLINK` | Слишком много ссылок |
| 32 | `EPIPE` | Разорванный канал |
| 33 | `EDOM` | Ошибка математического аргумента |
| 34 | `ERANGE` | Результат слишком велик |
| 35 | `EDEADLK` | Взаимная блокировка |
| 36 | `ENAMETOOLONG` | Имя файла слишком длинное |
| 37 | `ENOLCK` | Нет доступных блокировок |
| 38 | `ENOSYS` | Функция не реализована |
| 39 | `ENOTEMPTY` | Директория не пуста |
| 40 | `ELOOP` | Слишком много символьных ссылок |
| 42 | `ENOMSG` | Нет сообщения требуемого типа |
| 43 | `EIDRM` | Идентификатор удален |
| 60 | `ENOSTR` | Не поток |
| 61 | `ENODATA` | Нет данных |
| 62 | `ETIME` | Таймер истек |
| 63 | `ENOSR` | Нет потоковых ресурсов |
| 89 | `EDESTADDRREQ` | Требуется адрес назначения |
| 90 | `EMSGSIZE` | Сообщение слишком длинное |
| 91 | `EPROTOTYPE` | Неверный тип протокола |
| 92 | `ENOPROTOOPT` | Протокол недоступен |
| 93 | `EPROTONOSUPPORT` | Протокол не поддерживается |
| 94 | `ESOCKTNOSUPPORT` | Тип сокета не поддерживается |
| 95 | `EOPNOTSUPP` | Операция не поддерживается |
| 96 | `EPFNOSUPPORT` | Семейство протоколов не поддерживается |
| 97 | `EAFNOSUPPORT` | Семейство адресов не поддерживается |
| 98 | `EADDRINUSE` | Адрес уже используется |
| 99 | `EADDRNOTAVAIL` | Адрес недоступен |
| 100 | `ENETDOWN` | Сеть недоступна |
| 101 | `ENETUNREACH` | Сеть не достижима |
| 102 | `ENETRESET` | Сеть разорвала соединение |
| 103 | `ECONNABORTED` | Соединение разорвано |
| 104 | `ECONNRESET` | Соединение сброшено |
| 105 | `ENOBUFS` | Нет буферного пространства |
| 106 | `EISCONN` | Сокет уже подключен |
| 107 | `ENOTCONN` | Сокет не подключен |
| 108 | `ESHUTDOWN` | Сокет закрыт |
| 110 | `ETIMEDOUT` | Таймаут соединения |
| 111 | `ECONNREFUSED` | Соединение отклонено |
| 112 | `EHOSTDOWN` | Хост недоступен |
| 113 | `EHOSTUNREACH` | Хост не достижим |
| 114 | `EALREADY` | Операция уже выполняется |
| 115 | `EINPROGRESS` | Операция выполняется |
| 116 | `ESTALE` | Устаревший файловый дескриптор |
| 122 | `EDQUOT` | Превышена квота диска |
| 125 | `ECANCELED` | Операция отменена |
| 130 | `EOWNERDEAD` | Владелец заблокированного объекта умер |
| 131 | `ENOTRECOVERABLE` | Состояние не восстанавливается |

**Пример использования:**
```c
#include <errno.h>
#include <string.h>

FILE* f = fopen("nonexistent.txt", "r");
if (f == NULL) {
    printf("Ошибка %d: %s\n", errno, strerror(errno));
    // Выведет: "Ошибка 2: No such file or directory"
}
```

</details>

</details>

<details>
<summary>
strlen - длина строки
</summary>

**Описание:**  
Вычисляет длину строки `str` (без `\0`).

**Возвращаемое значение:**  
Длина строки

**Крайние случаи:**
- `str == NULL` → Segmentation fault

**Пример:**
```c
char str[] = "Hello";
size_t len = strlen(str);
printf("%zu\n", len); // 5
```

</details>

**Примечание:** Функции `strdup` и `strndup` не являются стандартом C, но широко поддерживаются в POSIX-системах (Linux, macOS). В Windows требуют `_strdup` и `_strndup`.