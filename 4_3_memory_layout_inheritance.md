# $4.3 Memory layout, constructors and destructors in case of inheritance
```cpp
struct Base {
  int x;
};

struct Derived: Base {
  double y;
};

int main() {
  std::cout << sizeof(Derived);
}
```
Что выведет программа?
`Ответ`: 16 `[4б int из Base][4б выравнивание][8б double]`  

```cpp
struct Base {
  void f() {}
};

int main() {
  std::cout << sizeof(Base);
}
```
Что выведет программа?
`Ответ`: 1 `[1б Base]`

Функции в структурах ничего не занимают. И структура не может быть меньше 1 байта, то же самое и для классов работает.

```cpp
struct Base {
  void f() {}
};

struct Derived: Base {
  double y;
  void g() {}
};

int main() {
  std::cout << sizeof(Derived);
}
```
Что выведет программа?
`Ответ`: 8 `[0б Base][8б double]`

Тут работает `EBO(Empty Base Optimization)` 
Если пустой класс является чьим-то наследником, то этот класс занимает 0 байт.

```cpp
struct Base {
  int x;
  Base(int x) : x(x) {}
};

struct Derived: Base {
  double y;
  Derived(double y) : y(y) {}
};

int main() {
  Derived d;
}
```
Что не так с такой программой?
При создании объекта сначала создается родитель(вызывается конструктор родителя), а потом создается наследние(вызывается конструктор наследника). Но у родителя определен конструктор только от `int`(конструктор по умолчанию не будет сгенерирван) и получим `CE`. Если хотим, чтобы код заработал пишем: `Base() = default` внутри `Base`. Это добавит конструктор по умолчанию. Либо в конструкторе класса-наследника указываем как сконструировать базовый класс. Например:
```cpp
struct Derived: Base {
  double y;
  Derived(double y) : Base(0), y(y) {}
};
```

```cpp
struct Base {
  int x;
  ~Base() { std::cout << "~Base\n"; }
};

struct Derived: Base {
  double y;
  ~Derived() { std::cout << "~Derived\n"; }
};

int main() {
  Derived d;
}
```
Что выведет программа?
`Ответ`: 
~Derived
~Base
Да, при уничтожении объекта сначала уничтожается наследник, а после родитель.

Бывает такое, что наследник использует все такие же конструкторы как и родитель. Например, наследник просто добавил для себя какой-то метод, а остальное все оставил без изменений. Как нам использовать конструкторы родителя без копипаста? Для этого можно использовать `using`.
```cpp
struct Base {
  int x;
  Base() = default;
  Base(int x) : x(x) {}
  Base(const Base& other) : x(other.x) {}
};

struct Derived: Base {
  double y;
  using Base::Base;
  void print(); //  что-то печатает
};

int main() {
  Derived d;
  Derived d2 = d;
}
```
А что будет с конструктором копирования? Он сгенерируется по умолчанию для `Derived` и он будет приоритетнее чем конструктор `Base`. Конструкторы копирования и перемещения не наследуются!

