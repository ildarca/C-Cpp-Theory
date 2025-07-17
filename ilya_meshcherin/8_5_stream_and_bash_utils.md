Всего три потока:
1. `std::cin` - для ввода
2. `std::cout` - для вывода
4. `std::cerr` - для вывода ошибок, дебаг сообщений

Для перенаправления в `Linux` вывода используется `>`.
Например: 
```cpp
#include <iostream>

int main() {
  std::cout << "Alfff";
  return 0;
}
```
Скомпилируем эту программу и переведем ее вывод в файл `output.txt`.
```bash
g++ a.cpp
./a.out > output.txt
```

`>` стирает старые данные в файле и записывает.
`>>` дозаписывает данные в файл.
```bash
./a.out >> output.txt
```

Изменим программу, как нам сделать отдельно вывод ошибок и обычного текста?
```cpp
#include <iostream>

int main() {
  std::cerr << "Error";
  std::cout << "Alfff";
  return 0;
}
```
Ответ: Используется номер потока для перенаправления вывода.
`1` - поток вывода
`2` - поток вывода для ошибок
```bash
./a.out 1>good.txt
./a.out 2>err.txt
./a.out 1>good.txt 2>err.txt
```

Еще можно перенаправить поток из `2` в `1` и из `1` в файл.
```bash
./a.out 1>output.txt 2>1
```

## Оператор | (Pipe)

Палочка позволяет перенаправлять вывод одной программа на вывод другой.
Например, пусть будет две программы `a.cpp` и `b.cpp`.
`a.cpp`:
```cpp
#include <iostream>

int main() {
  std::cout << "Alfff";
  return 0;
}
```

`b.cpp`:
```cpp
#include <iostream>
#include <string>

int main() {
  std::string str;
  std::cin >> str;
  return 0;
}
```
Запуск:
```bash
g++ a.cpp -o a.out
g++ b.cpp -o b.out

./a.out | ./b.out
```

## grep

`grep` - утилита поиска шаблона в файлах
Например:
```bash
grep "exception" 7_exceptions.md
```
Можно скомбенировать с `Pipe`:
```bash
ls -lR . | grep cpp 
```
И таким образом получается все файлы с `cpp` в данной диреткории.

## head and tail

`head` и `tail` - утилиты для вывода соответственно первых или последних строк файла.
```bash
tail a.cpp
head a.cpp
```
Комбинация с `ls`
```bash
ls -lS . | head
```

## wc

`wc` - утилита для подсчета количества слов, строк, символов.
```bash
ls -l | wc
```

## tee

`tee` -  утилита для раздвоения вывода.
```bash
./a.out | tee out.txt
```
Вывод программы `a.out` будет перенаправлен в поток вывода и в файл `out.txt`.

# Буферизация

```cpp
#include <iostream>
#include <cassert>

int main() {
  std::cout << "asaf";
  assert(false);
  return 0;
}
```
Программа упадет с ошибкой, но текст `asaf` не выведется. Почему?
Вывод в поток вывода очень дорогостоящая операция. Поэтому в `std::cout` записывает вывод в свой массив(буфер) и при заполнении данного буфера выводит в поток данные. В нашем примере буфер еще не заполнился, поэтому вывода мы не увидели.

## Очистка буфера

Для очистки буфера ручками используется функция `flush()`
```cpp
#include <iostream>
#include <cassert>

int main() {
  std::cout << "asaf";
  std::cout.flush();
  assert(false);
  return 0;
}
```
Но это долгая операция. Можно сделать `flush()` неявно используя `std::endl`.
```cpp
#include <iostream>
#include <cassert>

int main() {
  std::cout << "asaf" << std::endl;
  assert(false);
  return 0;
}
```

## Связь `std::cout` и `std::cin`

```cpp
#include <iostream>

int main() {
  int x;
  std::cout << "asaf";
  std::cin >> x;
  return 0;
}
```
В этой программе видно, что перед `std::cin` произойдет очистка буфера. Это объясняется связью `std::cout` и `std::cin` черезвнутренние указатели. Если их отвязать, то вывод станет быстрее, но тогда буфер уже не будет очищаться. Для этого необходимо написать:
```cpp
std::cin.tie(nullptr);
std::cout.tie(nullptr);
```
Так же `std::cin` и `std::cout` связаны с `scanf` и `printf`. Их тоже можно отвязать:
```cpp
std::cin.sync_with_stdio(false);
std::cout.sync_with_stdio(false);
```

### Файловые и строковые потоки

Файловые потоки:
```cpp
#include <fstream>

int main() {
  std::ifstream in("input.txt");
  return 0;
}
```

Строковые потоки:
```cpp
#include <sstream>

int main() {
  std::string str("1 2 3 4 5");
  std::istringstream iss(str);
  
  int x;
  iss > x;
  return 0;
}
```

