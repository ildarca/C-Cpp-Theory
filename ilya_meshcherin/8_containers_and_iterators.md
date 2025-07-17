 # VIII. Containers and iterators.

 ## $ 8.1 std::vector implementation.
 Пока нам не интересны мелкие методы `std::vector`, поговорим о том как реализовать метод `push_back`.
```
template <typename T>
class vector {
  T* arr_;
  size_t sz_;
  size_t cap_;

  void push_back(consst T& value) {
  }
};
```
Для начало `push_back` должен зарезервировать пространство для нового элемента.
```
  void push_back(consst T& value) {
    if (sz_ == cap_) {
      reserve(cap_ > 0 ? cap_ * 2 : 1);
  }
```
Мы свели задачу к реализации `reserve`. Она должна выделять память под новые объекты вектора. Давайте попробуем ee написать:
```
void reserve(size_t newcap) {
  T* newarr_ = new T[newcap];
}
```
Но есть у нее проблема. Мы создаем `newcap` объектов типа `T`. И если в вектор будет добавляться объект без конструктора по умолчанию, то наш вектор сломается. Например, вот так: `v.push_back(C(1));`. Нам нужно выделить память под конкретное количество байт не заполняя их объектами:
```
void reserve(size_t newcap) {
  T* newarr_ = reinterpret_cast<T*>(new char[newcap * sizeof(T)]);
}
```
Теперь нужно эту память заполнить:
```
void reserve(size_t newcap) {
  T* newarr = reinterpret_cast<T*>(new char[newcap * sizeof(T)]);
  for (size_t index = 0; index < sz_; ++index) {
    newarr[index] = arr_[index];
  }
}
```
И снова у нас проблемы, ведь у нас `newarr` это массив объектов типа `T`(но там лежат только сырые байтики). И мы написав `=` вызывает `operator=` у объектов `T` из массива. Но мы не может вызывать `operator=` у объектов `T` из `newarr` ведь там на самом деле обычные байтики.
`memcpy` мы тоже не можем использовать так как могут быть объект в котором одно из полей является ссылкой или указателем на другое его поле. Тип такого:
```
struct Strange {
  int x;
  int& r;
  Strange(int y): x(y), r(x) {}
};
```
На самом деле нам для решения проблемы пригодиться `placement new`.
Его еще можно назвать `new` с параметрами.
Он вызывает конструктор конкретного типа по конкретному адресу.
```
void reserve(size_t newcap) {
  T* newarr = reinterpret_cast<T*>(new char[newcap * sizeof(T)]);
  for (size_t index = 0; index < sz_; ++index) {
    new(newarr + index)  T(arr_[index];
  }
}
```
Нужно освободить старую память:
```
void reserve(size_t newcap) {
  T* newarr = reinterpret_cast<T*>(new char[newcap * sizeof(T)]);
  for (size_t index = 0; index < sz_; ++index) {
    new(newarr + index)  T(arr_[index];
  }
  
  for(size_t index = 0; index < sz_; ++index) {
    (arr_ + index)->~T(); //  вызываем деструкторы
  }
  delete[] reinterpret_cast<char*>(arr_);
}
```
Обработаем случай когда нам не нужно память новую выделять. И обновим размер и указатель на массив объектов:
```
void reserve(size_t newcap) {
  if (newcap <= cap_) return;
  T* newarr = reinterpret_cast<T*>(new char[newcap * sizeof(T)]);
  for (size_t index = 0; index < sz_; ++index) {
    new(newarr + index)  T(arr_[index];
  }
  
  for(size_t index = 0; index < sz_; ++index) {
    (arr_ + index)->~T(); //  вызываем деструкторы
  }
  delete[] reinterpret_cast<char*>(arr_);

  arr_ = newarr;
  cap_ = newcap;
}
```
Осталось обработать случаи исключений. Исключение может выдать `new` и конструктор объекта `T`. Обработаем это:
```
void reserve(size_t newcap) {
  if (newcap <= cap_) return;
  T* newarr = reinterpret_cast<T*>(new char[newcap * sizeof(T)]);
  size_t index = 0;
  try { 
    for (; index < sz_; ++index) {
      new(newarr + index)  T(arr_[index];
    }

    // тут создается новый объект который мы получили в push_back.
    // сами напишете.

  } catch (...) {
    for(int new_index = 0; new_index < index; ++new_index) {
      (arr_ + new_index)->~T();
    }
    throw;
  }
  
  for(size_t index = 0; index < sz_; ++index) {
    (arr_ + index)->~T(); //  вызываем деструкторы
  }
  delete[] reinterpret_cast<char*>(arr_);

  arr_ = newarr;
  cap_ = newcap;
}
```
Про обычный `std::vector` все. Есть еще необычный `vector<bool>`. Он очень неуклюжий и ломает некоторые правила С++. Посмотрим как его реализовать:
```
template <>
class vector<bool> {
  char* arr;
  size_t sz_;
  size_t cap_;
public:

};
```
Здесь нам надо написать `operator[]`.

Для начала посмотрим, что происходит при вызове `operator[]` в стандартной библиотеке:
```
std::vector<bool> v(10);
v[5] = true;
```
Что возвращает `operator[]` в стандартно реализации?  
Чтобы узнать специально создадим случай `CE`. Для этого создадим отдельный класс конструктор, которого удален и вызовет `CE`. И в ошибке компилятор нам покажет, что за тип у .(компилируй с флагом -std=c++17, так как с C++17 разрешено не указывать шаблонные аргументы класса). Мы получим тип: `std::_Bit_reference`. Реализуем эти махинации:
```
template <typename T>
struct Debug {
  Debug(T) = delete;
};
```
Получается в классе `vetor<bool>` еще ест подкласс `_Bit_reference`. И `operator[]` возвращает его. Добавим в нашу реализацию:
```
template <>
class vector<bool> {
  char* arr;
  size_t sz_;
  size_t cap_;
  struct BitReference {
    char* cell; // указательна ячейку
    uint8_t index; // номер бита в этой ячейке, на ктр. он ссылается
  };
public:
  BitReference operator[](size_t index) {
    // ...
  }
};
```
У BitReference есть `operator=`, который меняет конкретный бит под указателем. Реализуем:
```
struct BitReference {
  char* cell; // указательна ячейку
  uint8_t index; // номер бита в этой ячейке, на ктр. он ссылается
  BitReference operator=(bool b) {
    if (b) {
      *cell |= (1 << index);
    } else {
      *cell &= ~(1 << index);
    }
    return *this;
  }
};
```
`operator[]` из класса `vector<bool>` должен вернуть `BitReference` поэтому для него надо реализовать конструктор:
```
struct BitReference {
  char* cell; // указательна ячейку
  uint8_t index; // номер бита в этой ячейке, на ктр. он ссылается
  BitReference(char* cell, uint8_t index)
    : cell(cell), index(index) {}

  BitReference operator=(bool b) {
    if (b) {
      *cell |= (1 << index);
    } else {
      *cell &= ~(1 << index);
    }
    return *this;
  }
};
```
У этой структоры есть возможность преобразовываться в `bool`.
```
operator bool() const {
  return *cell & (1 << index);
}
```

Ну и допишем `operator[]` в `vector<bool>`:
```
public:
  BitReference operator[](size_t index) {
    return BitReference(arr + index / 8, index % 8);
  }
```
За счет всех этих манипулций в этом коде: `v[5] = true;` создается `rvalue` объект `BitReference` через который осуществляется присваивание элементов.

## $ 8.2 std::deque internals

Поговорим о проблеме при исопльзовании `std::vector`. 
Любой из нас может сделать указатель на элемент вектора, таким образом:
```
std::vector<int> v(10);
int* p = &v[5];
```
Но что будет с этим указателем, если мы выполним `push_back`:
```
v.push_back(1);
```
У нас скорее всего произойдет реалокация памяти под массив вектора и указатель будет указывать на память, которая нам уже не принадлежит. То же самое произойдет и со ссылкой, ведь в основе ссылок лежит указатель.  
Этот случай называется `pointer/reference invalidation`.

Эту проблему решает как раз таки `std::deque`, он не инвалидирует указатель, при добавлении элементов.

Что мы хоти от `std::deque`:
1. Обращение по `[]` от O(1);
2. Операции `push_back`, `push_front`;
3. Отсутствие `pointer/reference invalidation`.

Как же он устроен внутри:
В нем хранится массив из указателей на сырую память - `T** arr`. Сырая память представлет собой массив размером `N=32`. Такие массивы называются `buckets`. Пустой `deque` ничего не выделяет. При добавлении элементов уже начинает выделяться память. Схемка строения:
```
+---+
|ptr|
+---+
|ptr|
+---+    +---------------+----+
|ptr| -> |               |    |
+---+    +---+---+---+---+----+
|ptr| -> |   |   |   |        | 
+---+    +---+---+---+--------+
|ptr| -> |                    |
+---+    +--------------------+
|...|
+---+
|ptr|
+---+
```
Так же нам неохдоми хранить пару индексов начала`(i1, j1)` и конца `(i2, j2)` дека.  
Что произойдет когда место под выделенную память закончится?
Нам придется реалоцировать память только для массива указателей и просто обновить адреса, указав на существующие `backets`.  Массив элементов нам реалоцировать не нужно.

Удаление и добавление  реализуется так же как для вектора: через `placement new` и вызов деструктора.

При обращении через `[index]` нам нужно пересчитать самим `index` в наши внутренние координаты. Индексация элементов всегда идет с `0`, отрицательных быть не может.

У `std::deque` отсутствуют методы реалокации которые есть в `std::vector`.

Адаптеры над `std::deque`:
1. `std::stack`
2. `std::queue`
3. `std::priority_queue`

`std::stack` выглядит вот так:
```
template <typename T, typename = std::deque<T>>
stack {};
```
У `stack` есть методы `pop` и `top`. `top` - возвращает верхний элемент стека. А `pop` удаляет верхний элемент стека. А почем бы не сделать, чтобы `pop` и возвращал и удалял? Это бы привело к копированию объекта из стека даже когда нам нужно было бы просто удалить элемент.

## $ 8.3 Iterators and their categories.

`std::vector` и `std::deque` позволяет на интуитивно понятном для человека уровне обращаться к элементам через `[]`, но не у всех контейнеров из стандартной библиотеки такая возможность есть(`std::list`, `std::map`, ...). И в связи с этим нам нужен удобный инструмент для обращения к элементам контейнеров - итераторы.

Итератов - тип, который позволяет выполнить обход последовательности, и обладает свойствами:
1. Возможно разыменовать
2. Возможно инкрементировать
3. Возможно сравнивать друг с другом
Формально это указатель.

В стандарте итераторы разделяются на категории. Разделение происходит по возможностям итераторов.
1. `InputIterator` - обладает перечисленными раннее свойствами.
2. `ForwardIterator` - умеет то же что и `InputIterator`, а так же гарантирует, что проходя по последовательности каждый раз мы будем иметь те же самые объекты (конечно если последовательность не менялась). (контейнеры к которым применим такой итератор: `forward_list`, `unordered_set`, `unordered_map`).
3. `BidirectionalIterator` - итератор обладающий предыдущими свойствами и возможностью двигаться в обратном направлении по последовательности.(`std::list`, `std::set`, `std::map`). 
4. `RandomAccessIterator` - обладает предыдущими свойствами и доступно следующие операции: `-= n`, `+= n`, `it1 - it2`, `<`, `>`, `<=`, `>=`.(`std::deque`).
5. `ContiguousIterator` - появился с C++17. Обладает предыдущими свойствами, а так же гарантирует, что он эквивалентен указателю. Другими словами вместо такой конструкции: `*( &(*it) + n)` можно использовать `*(it + n)`. (`std::vector`, `std::array`, `T*`). 

Давайте напишем цикл `for` который обходит элементы `std::set<int> s` используя итератор:
```
for ( std::set<int>::iterator it = s.begin(); it < s.end(); ++it) {
  // работаем с *it
}
```
В С++11 добавлен `range based for`, который сокращает способ такой записи:
```
for (int x: s) {
  // работаем с x
}
```
Более подробно как на самом деле под синтаксическим сахаром происходит работа можно посмотреть на сайте: https://cppinsights.io/

Какими свойствами должен обладать объект чтобы быть итератором: https://en.cppreference.com/w/cpp/named_req/Container
Требования к итератору: https://en.cppreference.com/w/cpp/named_req/Iterator

Какое практическое применение есть у итераторов?  
Они используются во всех стандартных алгоритмах. И каждый алгоритм в описании сигнатуры требует наличия конкретного итератора. Например, `std::sort` требует чтобы итератор был `RandomAccessIterator`:
```
template< class RandomIt >
void sort( RandomIt first, RandomIt last );
```
Тип итератора зависит от задач в которых он применяется, в каких-то задачах хватает определенных свойств итератора, а в других не хватает. 
Подробнее посмотреть алгоритмы и используемые итераторы можно здесь: https://en.cppreference.com/w/cpp/algorithm

Давайте разберемся как из под итератора достать элементы и как понять его тип. Допустим пишем какой-то алгоритм:
```
template <typename InputIterator>
void find_most_often_number(InputIterator begin, InputIterator end) {

}
```
Мы можем попробовать получить тип элемента который лежит под итератором таким образом: `auto x = *begin;`. Но таким способом мы можем получить совсем не тот тип. Например, в случае с `std::vector<bool>` мы получим `BitReference`. Поймем как правильно назвать тип под итератором. Здесь нам помогут существующие мета-функции, а именно `std::iterator_traits` - он умеет возвращать тип который лежит под итератором. Использовать так:`std::iterator_traits<InputIterator>::value_type`, и не забудем пометить его с `typename`. Подробно про возможности `std::iterator_traits`: https://en.cppreference.com/w/cpp/iterator/iterator_traits

Весь код на данный момент:
```
template <typename InputIterator>
void find_most_often_number(InputIterator begin, InputIterator end) {
  typename std::iterator_traits<InputIterator>::value_type x = *begin;
}
```
Напишем функцию поиска расстояния между двумя итераторами:
Для этого воспользуем `std::is_base_of_v` - которая проверяет является ли первый итератор базой другого. Мы проверим является ли у нас итератор типа `RandomAccessIterator`, если да, то он поддерживает вычитание итераторов, если же нет, то мы просто циклом посчитаем расстояние:
```
template <typename Iterator>
typename std::iterator_traits<Iterator>::defference_type
distance(Iterator first, Iterator last) {
  if constexpr (std::is_base_of_v<
       std::random_access_iterator_tag,
       typename std::iterator_traits<Iterator>::itterator_category
       >) {
    return last - first;
  }

  int i  = 0;
  for (; first != last; ++i, ++first);
  return i;
```
Есть замечательные функции `std::prev`, `std::next` - дают итераторы на предыдущий элемент от данного и на следующий элемент от данного.

Поговорим о реализации итератора в контейнере. Вернемся к проекту `std::vector` и реализуем внутри него итератор. Что есть у итератора?
1. Указатель на объект;
2. Уметь копироваться;
3. Уметь присваиваться;
4. Функции для получения итератора: `begin` и `end`. Для функций нужно написать приватный конструктор итератора;
5. `operator*()`;
6. `operator++()`, `operator++(int)`;
7. `operator ->`;

```
template <typename T>
class vector {
  T* arr_;
  size_t cap_;
  size_t sz_;
  
  class iterator {
  private:
    T* ptr;
    iterator(T* ptr): ptr(ptr) {}
  public:
    iterator(const iterator&) = default;
    iterator& operator=(const iterator&) = default;
    
    T& operator*() const { 
      return *ptr;
    }
    T* operator->() const {
      return ptr;
    }

    iterator& operator++() {
      ++ptr;
      return *this;
    }
    iterator operator++(int) {
      iterator copy = *this;
      ++ptr;
      return copy;
    }
  };

  iterator begin() {
    return iterator{arr_};
  }

  iterator end() {
    return iterator{arr_ + sz_};
  }
};
```

У контейнеров так же существуют `const interator` и `reverse iterator`.

Для начала разберемся с `const iterator`. Это тот же итератор, но с добавлением константности в указателе `ptr` и операторах `*`, `->`.
В таком случае нам надо переписать тот же самый код добавив `const`. Но мы против копипаста. Воспользуемся тернарным оператором из мира шаблонов `std::conditional_t<F, H, G>` и `using`-ом. 

Итератор переименуем в `base_iterator`.  
```
typename <bool IsConst>
class base_iterator {};
```
Определим типы внутри итератора:
```
using pointer_type = std::conditional_t<IsConst, const T*, T*>;
using reference_type = std::conditional_t<IsConst, const T&, T&>;
using value_type = T;
```

Теперь мы можем удобно расписать типы для уже существующих элементов:
```
typename <bool IsConst>
class base_iterator {
public:
  using pointer_type = std::conditional_t<IsConst, const T*, T*>;
  using reference_type = std::conditional_t<IsConst, const T&, T&>;
  using value_type = T;

private:
  pointer_type ptr;
  base_iterator(T* ptr): ptr(ptr) {}

public:
  base_iterator(const base_iterator&) = default;
  base_iterator& operator=(const base_iterator&) = default;

  reference_type operator*() const { return *ptr; }
  pointer_type operator->() const { return ptr; }
  
  base_iterator& operator++() {
    ++ptr;
    return *this;
  }

  base_iterator operator++(int) {
    base_iterator copy = *this;
    ++ptr;
    return copy;
  }
};
```

И в самом векторе определим типы итераторов:
```
using iterator = base_iterator<false>;
using const_iterator = base_iterator<true>;
```

Для константного вектора тоже должны работать операторы `begin`, `end`.
```
const_iterator begin() const {
  return {arr_};
}

const_iterator end() const {
  return {arr_ + sz_};
}
```

Нам так же надо определить `cbegin`, `cend`, которые дают константные итераторы и не для константных контейнеров:
```
const_iterator cbegin() const {
  return {arr_};
}

const_iterator cedn() const {
  return {arr_ + sz_};
}
```

Добавим поддержку `reverse_iterator`. Это обертка, которая позволяет итерироваться по контейнеру в обратном порядке.  
Определим его для нашего вектора:
```
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>
```
Методы `rbegin`, `rend`, `rcend`, `rcbegin` сами реализуются и их писать не нужно.

Итоговый код:
```
template <typename T>
class vector {
  T* arr_;
  size_t cap_;
  size_t sz_;

  typename <bool IsConst>
  class base_iterator {
  public:
    using pointer_type = std::conditional_t<IsConst, const T*, T*>;
    using reference_type = std::conditional_t<IsConst, const T&, T&>;
    using value_type = T;

  private:
    pointer_type ptr;
    base_iterator(T* ptr): ptr(ptr) {}

  public:
    base_iterator(const base_iterator&) = default;
    base_iterator& operator=(const base_iterator&) = default;

    reference_type operator*() const { return *ptr; }
    pointer_type operator->() const { return ptr; }
      
    base_iterator& operator++() {
      ++ptr;
      return *this;
    }

    base_iterator operator++(int) {
      base_iterator copy = *this;
      ++ptr;
      return copy;
    }
  };

public:
  using iterator = base_iterator<false>;
  using const_iterator base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>


  iterator begin() {
    return iterator{arr_};
  }

  iterator end() {
    return iterator{arr_ + sz_};
  }
  const_iterator begin() const {
    return {arr_};
  }

  const_iterator end() const {
    return {arr_ + sz_};
  }

  const_iterator cbegin() const {
    return {arr_};
  }

  const_iterator cedn() const {
    return {arr_ + sz_};
  } 
};
```

Инвалидируется ли итератор?
```
std::vector<int> v(10);
std::vector<int>::iterator x = v.begin() + 5;
v.push_back(1);
```
Да, итераторы тоже инвалидируются, а сделано это потому чтобы итераторы были такими же быстрыми как указатели(мы бы могли хранить индекс элемента и указатель на сам вектор, но это получается два разыменования вместо одного). За счет этого итератор на `std::deque` инвалидируется тоже, так как в итераторе мы храним указатель на внешний массив.

## $ 8.5 Output iterators and stream iterators.

В стандартной библиотеке есть множество разных алгоритмов. Например, `std::copy()`. Он копирует из одного места в другое элементы, `push_back`он вызывать не умеет, а только обращается через указатели:
```
int main() {
  int a[] = {1, 2, 3, 4, 5};
  std::vector<int> v;
  std::copy(a, a+5, v.begin());
}
```
Это приведет к `UB` ведь мы будем писать вне контейнера. Для решения этой проблемы изобрели `std::back_insert_iterator`. Это `OutputIterator` который гарантирует отсутствие записи в не свою память. Ну и решение нашей проблемы:
```
int main() {
  int a[] = {1, 2, 3, 4, 5};
  std::vector<int> v;
  std::copy(a, a+5, std::back_insert_iterator<std::vector<int>>(v));
}
```

Теперь попробуем сами его реализовать:
```
template <typename Container>
class back_insert_iterator {
  Container& container;
public:
  back_insert_iterator(Container& container): container(container) {}

  back_insert_iterator& operator=(const typename Container::value_type& value) {
    container.push_back(value);
    return *this;
  }
  
  back_insert_iterator& operator++() {
    return *this;
  } 
  back_insert_iterator& operator++(int) {
    return *this;
  }
  back_insert_iterator& operator*() {
    return *this;
  }
};
```
Обычно `back_insert_iterator` писать грамоздко поэтому существует функция`back_insert` для более короткой записи, реализуем:
```
template <typename Container>
back_insert_iterator<Container> back_insert(Container& container) {
  return {container};
}
```



