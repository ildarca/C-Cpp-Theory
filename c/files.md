# Работа с файлами

## Функции работы с файлами

<details>
<summary>
fopen() - Открытие файла
</summary>

**Синтаксис:**
```c
FILE *fopen(const char *filename, const char *mode);
```

**Параметры:**
- `filename` - **путь к файлу**
- `mode` - **режим открытия**

**Возвращает:**
- Указатель на `FILE` при успехе
- `NULL` при ошибке

**Режимы открытия:**
```c
"r"  - чтение (файл должен существовать)
"w"  - запись (создаёт/очищает файл)
"a"  - дозапись в конец
"r+" - чтение и запись
"w+" - чтение и запись (создаёт/очищает)
"a+" - чтение и дозапись
// Добавьте 'b' для бинарного режима: "rb", "wb", "ab"
```

**Пример:**
```c
FILE *file = fopen("data.txt", "r");
if (!file) {
    perror("Failed to open file");
    return 1;
}
// Работа с файлом...
fclose(file);
```

</details>

<details>
<summary>
fclose() - Закрытие файла
</summary>

**Синтаксис:**
```c
int fclose(FILE *stream);
```

**Параметры:**
- `stream` - **указатель на файловый поток**

**Возвращает:**
- `0` при успехе
- `EOF` при ошибке

**Предназначение:**
- Закрывает файловый поток
- Сбрасывает буферы на диск
- Освобождает ресурсы

**Пример:**
```c
FILE *file = fopen("test.txt", "w");
fprintf(file, "Hello World\n");
if (fclose(file) == EOF) {
    perror("Error closing file");
}
```

**Важно:** Всегда закрывайте файлы для избежания утечек ресурсов!

</details>

<details>
<summary>
fprintf() - Форматированная запись в файл
</summary>

**Синтаксис:**
```c
int fprintf(FILE *stream, const char *format, ...);
```

**Параметры:**
- `stream` - **файловый поток**
- `format` - **форматная строка**
- `...` - **аргументы для форматирования**

**Возвращает:**
- Количество записанных символов
- Отрицательное значение при ошибке

**Пример:**
```c
FILE *file = fopen("output.txt", "w");
fprintf(file, "Name: %s, Age: %d, Score: %.2f\n", 
        "John", 25, 95.5f);
fclose(file);
```

**Спецификаторы формата:**
- `%d` - целое число
- `%s` - строка  
- `%f` - вещественное число
- `%c` - символ
- `%x` - шестнадцатеричное

</details>

<details>
<summary>
fscanf() - Форматированное чтение из файла
</summary>

**Синтаксис:**
```c
int fscanf(FILE *stream, const char *format, ...);
```

**Параметры:**
- `stream` - **файловый поток**
- `format` - **форматная строка**
- `...` - **указатели на переменные для сохранения данных**

**Возвращает:**
- Количество успешно прочитанных элементов
- `EOF` при конце файла или ошибке

**Пример:**
```c
FILE *file = fopen("data.txt", "r");
char name[50];
int age;
float score;

int result = fscanf(file, "Name: %s Age: %d Score: %f", 
                   name, &age, &score);
if (result == 3) {
    printf("Read: %s, %d, %.2f\n", name, age, score);
}
fclose(file);
```

</details>

<details>
<summary>
fgets() - Чтение строки из файла
</summary>

**Синтаксис:**
```c
char *fgets(char *str, int num, FILE *stream);
```

**Параметры:**
- `str` - **буфер для строки**
- `num` - **максимальное количество символов**
- `stream` - **файловый поток**

**Возвращает:**
- `str` при успехе
- `NULL` при конце файла или ошибке

**Предназначение:**
- Читает строку до символа новой строки `\n` или конца файла
- Добавляет нулевой терминатор `\0`
- Включает символ `\n` в буфер если он был прочитан

**Пример:**
```c
FILE *file = fopen("text.txt", "r");
char buffer[100];

while (fgets(buffer, sizeof(buffer), file)) {
    printf("Line: %s", buffer);  // \n уже включен в buffer
}
fclose(file);
```

</details>

<details>
<summary>
fputs() - Запись строки в файл
</summary>

**Синтаксис:**
```c
int fputs(const char *str, FILE *stream);
```

**Параметры:**
- `str` - **строка для записи**
- `stream` - **файловый поток**

**Возвращает:**
- Неотрицательное число при успехе
- `EOF` при ошибке

**Предназначение:**
- Записывает строку в файл
- **Не добавляет** символ новой строки `\n`

**Пример:**
```c
FILE *file = fopen("output.txt", "w");
fputs("Hello World", file);
fputs("\nSecond line", file);
fclose(file);
```

**Отличие от fprintf():**
```c
fputs("Text", file);     // Просто записывает строку
fprintf(file, "Text");   // Анализирует форматную строку
```

</details>

<details>
<summary>
fread() - Бинарное чтение из файла
</summary>

**Синтаксис:**
```c
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
```

**Параметры:**
- `ptr` - **указатель на буфер**
- `size` - **размер элемента в байтах**
- `count` - **количество элементов**
- `stream` - **файловый поток**

**Возвращает:**
- Количество успешно прочитанных элементов

**Предназначение:**
- Чтение бинарных данных из файла
- Не выполняет преобразований данных

**Пример:**
```c
FILE *file = fopen("data.bin", "rb");
int numbers[10];
size_t read_count = fread(numbers, sizeof(int), 10, file);

if (read_count == 10) {
    printf("Successfully read %zu integers\n", read_count);
}
fclose(file);
```

</details>

<details>
<summary>
fwrite() - Бинарная запись в файл
</summary>

**Синтаксис:**
```c
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
```

**Параметры:**
- `ptr` - **указатель на данные**
- `size` - **размер элемента в байтах**
- `count` - **количество элементов**
- `stream` - **файловый поток**

**Возвращает:**
- Количество успешно записанных элементов

**Предназначение:**
- Запись бинарных данных в файл
- Не выполняет преобразований данных

**Пример:**
```c
FILE *file = fopen("output.bin", "wb");
int data[] = {1, 2, 3, 4, 5};
size_t written = fwrite(data, sizeof(int), 5, file);

if (written == 5) {
    printf("Successfully wrote %zu integers\n", written);
}
fclose(file);
```

</details>

<details>
<summary>
fseek() - Перемещение по файлу
</summary>

**Синтаксис:**
```c
int fseek(FILE *stream, long offset, int origin);
```

**Параметры:**
- `stream` - **файловый поток**
- `offset` - **смещение в байтах**
- `origin` - **точка отсчета**

**Константы для origin:**
```c
SEEK_SET - начало файла
SEEK_CUR - текущая позиция  
SEEK_END - конец файла
```

**Возвращает:**
- `0` при успехе
- Не-`0` при ошибке

**Пример:**
```c
FILE *file = fopen("data.bin", "rb");

// Перейти на 100 байт от начала
fseek(file, 100, SEEK_SET);

// Перейти на 50 байт вперед от текущей позиции
fseek(file, 50, SEEK_CUR);

// Перейти на 10 байт перед концом файла
fseek(file, -10, SEEK_END);

fclose(file);
```

</details>

<details>
<summary>
ftell() - Текущая позиция в файле
</summary>

**Синтаксис:**
```c
long ftell(FILE *stream);
```

**Параметры:**
- `stream` - **файловый поток**

**Возвращает:**
- Текущую позицию в файле
- `-1L` при ошибке

**Предназначение:**
- Получение текущей позиции в файловом потоке
- Полезно для запоминания позиции и возврата к ней

**Пример:**
```c
FILE *file = fopen("data.txt", "r");

// Запомнить текущую позицию
long position = ftell(file);

// Прочитать некоторое количество данных
char buffer[100];
fgets(buffer, sizeof(buffer), file);

// Вернуться к сохраненной позиции
fseek(file, position, SEEK_SET);

fclose(file);
```

</details>

<details>
<summary>
feof() - Проверка конца файла
</summary>

**Синтаксис:**
```c
int feof(FILE *stream);
```

**Параметры:**
- `stream` - **файловый поток**

**Возвращает:**
- Не-`0` если достигнут конец файла
- `0` в противном случае

**Предназначение:**
- Проверка достижения конца файла
- **Важно:** Функция возвращает true только после попытки чтения за концом файла

**Правильное использование:**
```c
FILE *file = fopen("data.txt", "r");
char buffer[100];

while (fgets(buffer, sizeof(buffer), file)) {
    // Обработка данных
}
// Только здесь feof() может вернуть true
if (feof(file)) {
    printf("Reached end of file\n");
} else {
    printf("Error reading file\n");
}
fclose(file);
```

**Неправильное использование:**
```c
while (!feof(file)) {  // НЕ ДЕЛАЙТЕ ТАК!
    fgets(buffer, sizeof(buffer), file);
    // Может прочитать лишнюю строку
}
```

</details>

<details>
<summary>
ferror() - Проверка ошибки файлового потока
</summary>

**Синтаксис:**
```c
int ferror(FILE *stream);
```

**Параметры:**
- `stream` - **файловый поток**

**Возвращает:**
- Не-`0` если произошла ошибка
- `0` если ошибок нет

**Предназначение:**
- Проверка наличия ошибок в файловом потоке
- Ошибки могут возникать при операциях чтения/записи

**Пример:**
```c
FILE *file = fopen("data.txt", "r");
if (!file) {
    perror("Failed to open file");
    return 1;
}

char buffer[100];
while (fgets(buffer, sizeof(buffer), file)) {
    // Обработка данных
}

if (ferror(file)) {
    perror("Error reading file");
}

fclose(file);
```

**Очистка флага ошибки:**
```c
clearerr(file);  // Сбрасывает флаги ошибки и конца файла
```

</details>

## Режимы открытия файлов

<details>
<summary>
Режим "r"
</summary>

Открытие текстового файла только на чтение. Файл должен существовать.
</details>

<details>
<summary>
Режим "w"
</summary>

Открытие текстового файла только на запись. Если файл не существует, то он создается. Если существует, то все его прежнее содержимое удаляется.
</details>

<details>
<summary>
Режим "a"
</summary>

Открытие текстового файла на дозапись (новые данные добавляются в конец файла). Если файл не существует, то он создается.
</details>

<details>
<summary>
Режим "r+"
</summary>

Открытие текстового файла для чтения и записи одновременно. Файл должен существовать.
</details>

<details>
<summary>
Режим "w+"
</summary>

Открытие текстового файла для чтения и записи. Если файл не существует, то он создается. Если существовал, то все его прежнее содержимое удаляется.
</details>

<details>
<summary>
Режим "a+"
</summary>

Открытие текстового файла для чтения и записи. Добавлять данные можно только после имеющегося содержимого. Читать можно все данные. Если файл не существует, то он создается.
</details>
