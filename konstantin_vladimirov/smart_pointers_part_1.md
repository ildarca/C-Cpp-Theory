## Проблема ручного управления

Допупстим код должен выделить некий ресурс и корректно освободить его
```cpp
int foo(int x, double y) {
  MyRes *ptr = new MyRes(x, y);
  // ...
  if (/* какое-то условие */) {
    delete ptr;
    return -1;
  }
  // ...

  delete ptr;
  return 0;
}
```

Очевидно, что эта система довольна хрупкая. Что если три ресурса? Пять точек выхода? А что делать при исключениях?

## Идея решения: обертка над ресурсом

```cpp
int foo(int x, double y) {
  dumb_ptr<MyRes> res(new MyRes(x, y));  // выделение и захват
  // ...
  if (/* какое-то условие */) {
    return -1;  // память освобождается в деструкторе
  }
  // ...

  return 0;  // освобождение памяти
}
```
Теперь система куда симпатичней и требует от программмиста куда меньше держать в голове.

Напишем реализацию:
```cpp
template <typename T>
class dumb_ptr {
  T* ptr_;
 public:
  dumb_ptr(T* ptr = nullptr) : ptr_(ptr){}
  ~dumb_ptr {delete ptr};
  T operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }
};
```
В этом коде есть неочевидная проблема, связанная с генерацией компилятора конструктора копирования и оператора копирования по умолчанию.
Проблема в том, что тогда у нас может возникнуть двойное освобождение памяти, если оба наших объекьта класса управляют одним ресурсом.
Пример проболемы:
```cpp
dumb_ptr<int> ptr1(new int(42));
dumb_ptr<int> ptr2 = ptr1;
```
```
+----+     +----+
|ptr1|     |ptr2|
+----+     +----+
  |           |
  |   +---+   |
  \-> |int| <-/
      +---+
```

Решение проблемы:
Первый вариант - запретить копирование вообще
```cpp
template <typename T>
class scoped_ptr {
  // тут то же самое, что и в dumd_ptr

  scoped_ptr(const scoped_ptr&) = delete;
  scoped_ptr& operator=(const scoped_ptr&) = delete;
};
```
Это решает исходную проблему, но не позволяет выносить ресурс за пределы его области видимости.

Можем добавить метод для получения сырого указателя. За счет чего сможем передавать в различные скоупы ресурс. Но тогда для этого указателя кто-то может вызвать `delete`.

Древний выход из положения: auto_ptr

`auto_ptr` был введен в 98-м стандарте для передачи и владения ресурсом.
```cpp
template <typename T> auto_ptr {
  T* ptr_;
 public:
  auto_ptr(T* ptr = nullptr) : ptr_(ptr) {}
  ~auto_ptr() { delete ptr; }
  auto_ptr(auto_ptr& rhs) : ptr_(rhs.ptr_) { rhs.ptr_ = nullptr; }
  auto_ptr& operator=(auto_ptr rhs) {
    swap(*this, rhs);
    return this;
  }
  T operator*() const { return *ptr_; }
};
```
К чему может быть привести тихое копирование?

# Проблема: слишком тихое копирование

```cpp
template <typename T>
class Brittle {
  T working_, reserve_;
 public:
  Brittle(T val) : working_(val) reserve_(working_) {}
  //  работа с working_
};
```

Что будет если сделать?
```
Brittle<auto_ptr<int>> b(auto_ptr<int>(new int(42)));
```
`working_` опустеет и будет `UB`.

Главная проблема в том, что на месте `Brittle` может оказаться любой `stl` `container`. И это идеома `COAP(container of auto_ptr)`. За счет полезного `auto_ptr`, мы получали возможность разрушать внешне внутренние контексты контейнеров.
Что делать?

# Использовать перемещение
В С++11 был добавлен `unique_ptr`. Теперь при передаче ресурсов использовалось перемещение.
```cpp
template <typename T>
class unique_ptr {
  T* ptr_;
 
 public:
  unique_ptr(T* ptr = nullptr) : ptr_(ptr) {}

  unique_ptr(unique_ptr&& rhs) : ptr_(rhs.ptr_) {
    rhs.ptr_ = nullptr;
  }

  unique_ptr& operator=(unique_ptr&& rhs) {
    swap(*this, rhs); 
    return *this;
  }

  unique_ptr(unique_ptr& rhs) = delete;
  unique_ptr& operator=(unique_ptr& rhs) = delete;
};
```

Теперь все супер и мы не будем тихо копировать, а будем явно перемещать. Вернемся к нашему первому примеру:
```cpp
int foo(int x, double y) {
  unique_ptr<MyRes> ptr(new MyRes(x, y));  // захват ресурсов
  // ...
  if (/* какое-то условие */) {
    bar(move(ptr));  // передача владения
  }
  // ...

  return 0;  // очистка памяти
}
```

В С++14 был добавлен `make_unique` 
