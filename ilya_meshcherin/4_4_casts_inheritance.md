#  $4.4 Casts in case of inheritance

```cpp
sturct Base {
  int x = 1;
};

struct Derived: Base {
  int y = 2;
};

void f(Base& b) {
  std::cout << b.x;
};

int main() {
  Derived d;
  f(d);
}
```
Это программа сработает корректно. `Derived` будет прикастован к `Base`. Но в обратную сторону это не заработает.

`Slicing(Срез)`. Если передавать в функцию не по ссылке, а по значению то произойдет срез. Наш объект будет срезам до родительского(лишится некоторых полей и методов).
```cpp
sturct Base {
  int x = 1;
};

struct Derived: Base {
  int y = 2;
};

void f(Base& b) { // здесь происходит срез
  std::cout << b.x;
};

int main() {
  Derived d;
  f(d);
}
```

Допустим у родителя есть конструктор копирования. Напечается ли `Copy!`?
```cpp
sturct Base {
  int x = 1;
  Base() = default;
  Base(const Base& other) : x(other.x) {
    std::cout << "Copy!\n";
  }
};

struct Derived: Base {
  int y = 2;
};

void f(Base& b) { // здесь происходит срез
  std::cout << b.x;
};

int main() {
  Derived d;
  f(d);
}
```
Да, конструктор копирования вызовется и напечатается `Copy!`, ведь мы срезаем объект наследника до полноценного объекта родителя, со всеми возможжностями родителя.

