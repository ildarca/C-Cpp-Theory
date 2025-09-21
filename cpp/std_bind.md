## std::bind

До `C++11`, чтобы передавать функции с заранее подставленными или изменённым порядком аргументов 
(например, для callback-ов или потоков), приходилось создавать дополнительные классы-функторы 
или использовать указатели на функции. Это было громоздко и неудобно.

**С появлением `std::bind` стало возможно:**
- «Фиксировать» любые аргументы функции заранее, а нужные оставлять открытыми для передачи позже;
- Переставлять порядок аргументов как угодно;
- Связывать методы классов с нужным объектом.

**Где это полезно:**
- Для создания колбеков и обёрток для стандартных алгоритмов;
- Для передачи функций с заранее указанной частью параметров в другие функции, потоки и события.

Сегодня чаще используют лямбда-выражения — они компактнее и понятнее. Но `std::bind` по-прежнему поддерживается стандартом и может быть удобен, когда нужно быстро и лаконично связать функцию и фиксированные значения.

### Синтаксис `std::bind`:
`std::bind` — шаблонная функция, которая принимает на вход указатель на callable-объект `_Func` и аргументы `_BoundArgs` (могут быть константами, переменными или плейсхолдерами (placeholder)).  
Сигнатура выглядит следующим образом:
```cpp
template<typename _Func, typename... _BoundArgs>
bind(_Func&& __f, _BoundArgs&&... __args);
```

**Плейсхолдер (placeholder)** — это просто специальная константа-объект, определённая в пространстве имён `std::placeholders`.

**В коде это примерно вот так:**
```cpp
namespace std {

namespace placeholders {
  /*некий тип*/ _1; /*объект, обозначающий первый аргумент*/
  /*некий тип*/ _2; /*второй аргумент*/
  /*   ...   */
}

}
```
При вызове полученной через `std::bind` функцию, компилятор смотрит: если среди аргументов был `placeholder`, то он заменяется на фактическое значение, переданное при вызове.  
**То есть плейсхолдер — это просто специальный объект, обозначающий: “здесь будет этот аргумент позже”.**


| Концепция             | Код с std::bind                         | Эквивалент на лямбда                        | Описание                                      |
|-----------------------|-----------------------------------------|----------------------------------------------|-----------------------------------------------|
|Бинд функции           | auto f = std::bind(foo, _1, _2);        | auto f = [](int a, int b){ foo(a, b); };     | Просто передаёт оба аргумента как есть        |
|Бинд метода класса     | auto f = std::bind(&MyClass::method, obj, _1); | auto f = [&](int x){ obj.method(x); };       | Связывает метод с конкретным объектом         |
|Фиксирование аргумента | auto f = std::bind(foo, 42, _1);        | auto f = [](int x){ foo(42, x); };           | Подставляет 42 на место первого аргумента     |
|Перестановка аргументов| auto f = std::bind(foo, _2, _1);        | auto f = [](int a, int b){ foo(b, a); };     | Меняет местами аргументы при вызове           |
|Callback для потока    | std::thread t(std::bind(foo, 42));      | std::thread t([]{ foo(42); });               | Передаёт функцию с фиксированным аргументом в поток |

### Примеры применения:
1. Приявзка функции с фиксированными аргументами:
```cpp
#include <iostream>
#include <functional>

void PrintNum(int num) {
  std::cout << "Num = " << num << std::endl;
}

int main() {
  auto bind_func = std::bind(PrintNum, 10);
  bind_func(); // Output: Num = 10
  return 0;
}
```

2. Привязка функции с использованием `std::placeholder`:
```cpp
#include <iostream>
#include <functional>

void Add(int a, int b) {
  std::cout << a << " + " << b << " = " << a + b << std::endl;
}

int main() {
  auto AddFive = std::bind(Add, 5, std::placeholders::_1);
  AddFive(10); // Output: 5 + 10 = 15
  return 0;
}
```
3. Привязка метода к объекту:
```cpp
#include <iostream>
#include <functional>

class Calculator {
public:
  int Multiply(int a, int b) {
    return a * b;
  }
};

int main() {
  Calculator calc;
  auto MultiplyByTwo = std::bind(&Calculator::Multiply, &calc,
                                 2, std::placeholders::_1);
  std::cout << MultiplyByTwo(5) << std::endl; // Output: 10
  return 0;
}
```
4. Перестановка аргументов:
```cpp
#include <iostream>
#include <functional>

void Subtract(int a, int b) {
  std::cout << a << " - " << b << " = " << a - b << std::endl;
}

int main() {
  auto ReverseSubtract = std::bind(Subtract,
                                   std::placeholders::_2,
                                   std::placeholders::_1);
  ReverseSubtract(10, 20); // Output: 10
  return 0;
}
```
5. Использование в стандартных алгоритмах:
```cpp
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

bool IsEven(int n) {
  return n % 2 == 0;
}

int main() {
  std::vector<int> numbers = {1, 2, 3, 4, 5, 6};
  auto count_even = std::count_if(numbers.begin(), numbers.end(),
                                 std::bind(IsEven, std::placeholders::_1));
  std::cout << count_even << std::endl; // Output: 3
  return 0;
}
```
6. Привязка с несколькими `std::placeholder`:
```cpp
#include <iostream>
#include <functional>

void Divide(int a, int b) {
  if (b != 0)
    std::cout << a << " / " << b << " = " << a / b << std::endl;
  else
    std::cout << "Division by zero!" << std::endl;
}

int main() {
  auto DivideBy = std::bind(Divide,
                            std::placeholders::_1,
                            std::placeholders::_2);
  DivideBy(10, 2); // Output: 5
  DivideBy(10, 0); // Output: Division by zero!
  return 0;
}
```
7. Приявзка метода к разным данным:
```cpp
#include <iostream>
#include <functional>

class Counter {
public:
  void Increment(int& value) {
    ++ value;
  }
};

int main() {
  Counter counter;
  int a = 5, b = 10;
  auto IncrementA = std::bind(&Counter::Increment, &counter, std::ref(a));
  auto IncrementB = std::bind(&Counter::Increment, &counter, std::ref(b));
  std::cout << a << ", " << b << std::endl; // Output: 5, 10
  IncrementA();
  IncrementB();
  std::cout << a << ", " << b << std::endl; // Output: 6, 11
  return 0;
}
```
8. Трансформация контейнера с помощью `std::bind`:
```cpp
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

int Multiply(int a, int factor) {
  return a * factor;
}

int main() {
  std::vector<int> numbers = {1, 2, 3, 4, 5};
  std::vector<int> results(numbers.size());
  auto MultiplyByThree = std::bind(Multiply, std::placeholders::_1, 3);
  std::transform(numbers.begin(), numbers.end(),
                 results.begin(), MultiplyByThree);
  for (int n : results) {
    std::cout << n << " "; // Output: 3 6 9 12 15
  }
  std::cout << std::endl;
  return 0;
}
```
9. Привязка функции к потоку:
```cpp
#include <iostream>
#include <functional>
#include <thread>

void ThreadFunction(int a, int b) {
  std::cout << "Thread sum: " << a + b << std::endl;
}

int main() {
  auto BoundFunction = std::bind(ThreadFunction, 5, 10);
  std::thread t(BoundFunction);
  t.join(); // Output: Thread sum: 15
  return 0;
}
```

### Полезные ссылки:
1. [Наивная реализация std::bind](https://habr.com/ru/articles/310270)
