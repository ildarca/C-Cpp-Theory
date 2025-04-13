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

В С++14 был добавлен `make_unique`. Он позволяет безопаснее создавать ресурсы(помогает exception safety)
```cpp
int foo(int x, double y) {
  auto ptr = make_unique<MyRes>();  // создание и проброс 
  // ...
  if (/* какое-то условие */) {
    bar(move(ptr));  // передача владения
  }
  // ...

  return 0;  // очистка памяти
}
```
# Как аллоцировать массив?
У `make_unique` есть одна проблема:
```cpp
unique_ptr<int> ui(new int[1000]()); // OK
auto ui = make_unique<int>(тут никак  не выделить массив);
```
И правда ли это такая проблема? Нет, ведь при выделении массива память освобождается через `delete[]`, а в нашем умном указателе ресурсы освобождались через `delete`. Поэтому выделение памяти на массив это ошибка программиста.

Однако в `stl::unique_ptr` можно передавать тип `int array` и все будет хорошо.
```cpp
unique_ptr<int> ui (new int[1000]()); // грубая ошибка
auto ui = make_unique<int[]>(1000); // корректный массив
unique_ptr<int[]> ui (new int[1000]()); // тожe ok
```

# Решение проблемы удаления
Передавать в качестве второго шаблонного параметра `Deleter`:
```cpp
template <typename T, typename Deleter = default_delete<T>>
class unique_ptr {
  T* ptr_;
  Deleter del_; 
 public:
  unique_ptr(T* ptr = nullptr, Deleter del = Deleter())
  : ptr_(ptr), del_(del)  {}
  ~unique_ptr() { del_(ptr); }
  // и так далее
};
```

Как выглядит `default_delete`?
Здесь используется частичная специализация. Для разных ресурсов разное освобождение памяти
```cpp
template <typename T>
struct default_delete {
  void operator() (T* ptr) {delete ptr;}
};

template <typename T>
struct default_delete<T[]> {
  void operator() (T* ptr) {delete[] ptr;}
};
```

В качестве делитера мы можем использовать шаблонный класс с оператором `()`, лямбда-функцию или обычную функцию(для функции будет создаваться указатель, поэтому класс будет занимать больше памяти)  
Использование:
```cpp
template <typename T>
struct MyClassDeleter {
  void operator() (T* t) { delete[] t; }
};

auto MyLambdaDeleter = [](auto* t) { delete[] t; }

template <typename T>
void MyFuncDeleter(T* t) {
  delete[] t;
}

unique_ptr<int, MyClassDeleter<int>> ui1 (new int[100]());
unique_ptr<int, decltype(MyLambdaDeleter)> ui2 (new int[100](), MyLambdaDeleter);
unique_ptr<int, decltype(&MyFuncDeleter<int>)> ui3 (new int[100], MyFuncDeleter<int>);
```

# Уникальный void-указатель
В чистом виде это невозможно даже скомпилировать, так как не может работать `delete` от указателя на `void`. 
```cpp
unique_ptr<void> u;
```
Можно добавить свой `Deleter`, и тогда это определение будет возможно. Например, в делитере можно кастовать void-указатель к конкретному типу и уже его удалять через `delete`.

# Пробема неполного типа
Представим, что `class MyClass;` определен в текущем нашем файле, а егго реализация расположено в другом месте. Мы хотим хранить указатель на этот класс в `class MyWrapper`.
```cpp
class MyClass;

struct MyWrapper {
  MyClass* c_; 
  MyWrapper() : c_(nullptr) {}; // все ОК
};
```
Это будет прекрасно работать, но если мы заменим указатель на умный указатель.
```cpp
class MyClass;

struct MyWrapper {
  MyClass* c_; 
  MyWrapper() : c_(nullptr) {}; // все ОК
};

struct MySafeWrapper {
  unique_ptr<MyClass> c_;
  MySafeWrapper() : c_(nullptr) {}; // не компилируется 
};
```
В данном случае проблема в том, что `delete` проникает в хедер при использовании стандартного удалителя. Если мы объявим свой `Deleter`, а определение так же напишем где-то в другом месте, где `MyClass` уде является полным типом, то код заработает.
```cpp
class MyClass;

struct MyClassDeleter {
  void operator()(MyClass *);
};

struct MySafeWrapper {
  unique_ptr<MyClass, MyClassDeleter> c_;
  MySafeWrapper() : c_(nullptr) {}; // все ОК
};
```
А зачем нам вообще нужно хранить указатель на неполный тип? Для этого:
# Идиома PImpl
Идиома PImpl предлагает единое владение
```cpp
class Ifacade {
  CImpl *const impl_;
 public:
  Ifacade() : impl_(new CImpl) {}
  // методы
};
```
Эта идиома очень полезна: в частности она позволяет всегда иметь объект класса одного и того же размера(так как мы храним только указатель), что может быть очень важно в ABI.
Хорошей ли идеей является здесь заменить `Impl *const` на `unique_ptr`?Не совсем, ведь тогда мы лишаем ее константности, и гарантии того, что не будет создан move-конструктор(если поля константные, то move-конструктор по умолчанию не создается компилятором). Тогда просто добавим `const` к объявлению `unique_ptr`:
```cpp
class Ifacade {
  const unique_ptr<CImpl> impl_;
 public:
  Ifacade() : impl_(new CImpl) {}
  // методы
};
```
Мы получили `unique_ptr` с запрещенным move-конструктором. А если вспомнить начало темы, то мы там сталкивались с `scoped_ptr`, который и запрещал `move`.
# Дерево из уникальных указателей
Для дерева есественным типом владения является уникальное владение.
```cpp
template <typename Data>
class Tree {
  struct Node {
    unique_ptr<Node> left, right;
    Data d; // предполагает некопируемые данные
  };
  unique_ptr<Node> top_;
  void release_subtree(unique_ptr<Node> u) {}
};
```
Представим, что у нас есть дерево:
```
     [] // назовем root
    /  \
   /    \
  []    []
 /  \   / \
[]  [] [] []
```
При передаче в `release_subtree` умный указатель на `root` выполнит перемещение в `u`. Но `u` живет до конца своего `scope`(от фигурной скобки до фигурной скобки) и при выходе из `scope` `u` очистится. При этом очистка пойдет рекурсивно по дереву и может возникнуть `stack overflow`.

Возникает еще одна проблема, связанная с методом `find`, что она должна возвращать? 
```cpp
template <typename Data>
class Tree {
  struct Node {
      shared_ptr<Node> left, right;
      Data d;
  };
  unique_ptr<Node> top _;
 public:
  ??? find (int inorder_pos);
```
Варианты:
1. Указатель на ноду(утекает сырой указатель) 
2. Возвращать копию(представь, что хранится тяжелый объект и копировать его не желательно)
3. Ссылку на ноду(контейнеры тоже так могут, но есть вероятность, что объект умрет и ссылка станет невалидной).
4. Ссылку на `unique_ptr<Node>`(так же как и с обычной ссылкой, может стать невалидной)

Эти все проблемы приводят нас к реализации совместного владения ресурсом.
# Идея для shared_ptr
· Вспомним базовую проблему RAII: копирование.
· Пока что были рассмотрены такие варианты
  · Отказ от копирования: scoped_ptr, const unique_ptr
  · Копирование с передачей владения: auto_ptr
  · Отказ от копирования и семантика перемещения: unique_ptr

· Все эти варианты не предусматривают совместного владения
· Основная идея shared_ptr: подсчёт ссылок
  · Копирование с подсчётом ссылок и уничтожение объекта при обнулении счётчика

• Это должно быть подозрительно похоже на COW для строк!
Теперь можем исправить метод `find`.
```cpp
template <typename Data>
class Tree {
  struct Node {
    shared_ptr<Node> left, right;
    Data d;
  };
  shared_ptr<Node> top _;
 public:
  shared_ptr<Node> find (int inorder_pos);
```
Но тут так же все не идеально, не хотелось бы возвращать внутренние части в виде `Node`.

# Как может быть устроен shared_ptr?
1. Пока счетчик ссылок не нулевой, данные `Node` сохраняются в куче.
2. Все владеющие указатели могут читать и изменять их.
3. Специальная структура `контрольный блок` действительно владеет ресурсом.
```
+-----------------+
|shared_ptr<Node> |
| +-------------+ |
| |control block| |      +----------------+
| |  pointer    |-|----->| control block  |
| +-------------+ |      | +------------+ |   +------+
+-----------------+      | |   data     |-|-->| Node |
                         | |  pointer   | |   +------+
+-----------------+      | +------------+ |
|shared_ptr<Node> |      | | reference  | |
| +-------------+ |      | |  counter   | |
| |control block|-|----->| +------------+ |
| |  pointer    | |      +----------------+
| +-------------+ |
+-----------------+       

Этой картинке не стоит слишком верить, она скоро будет дополнена.
```
# Правильное создание
Существует(с С++14) два способа создать умный указатель
```cpp
shared_ptr<Node> p1 = make_shared<Node>();
shared_ptr<Node> p2(new Node()); 
```
В первом случае одним выделением памяти создаётся и контрольный блок и данные  
Во втором случае сначала создаются данные, а потом в конструкторе
контрольный блок  
Второй вариант менее эффективен и менее безопасен относительно
исключений
# Как может быть устроен shared_ptr
Благодаря `make_shared`, данные могут быть внесены в контрольный блок.
Тогда они выделяются в одну аллокацию со счетчиком ссылок и всем блоком.
```
+-----------------+
|shared_ptr<Node> |
| +-------------+ |
| |control block| |      +----------------+
| |  pointer    |-|----->| control block  |
| +-------------+ |      | +------------+ |
+-----------------+      | |    Node    | |
                         | |            | |
+-----------------+      | +------------+ |
|shared_ptr<Node> |      | | reference  | |
| +-------------+ |      | |  counter   | |
| |control block|-|----->| +------------+ |
| |  pointer    | |      +----------------+
| +-------------+ |
+-----------------+       

Этой картинке не стоит слишком верить, она скоро будет дополнена.
```
# Проблема доступа
Если в классе есть закрытые конструкторы, `make_shared` не сработает.
Внешняя функция `make_shared` должна иметь доступ к конструктору.
```cpp
class A {
  int val_;
  A(int v) : val_(v) {}
 public:
  // Это не сработает
  shared_ptr<A> createNext() { return make_shared<A>(val_); }
  // Это сработает
  shared_ptr<A> createNext() { return shared_ptr<A>(new A(val_)); }
};
```
