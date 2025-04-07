# VI. Templates.
## $ 6.1 Idea of templates, basic examples.
Бывает такое, что нам нужно использовать одну и ту же функцию, но с разными типами. Естественно, мы не хотим дублировать код. Для этого к нам на помощь приходят шаблоны.

Для создания шаблона используется следующая конструкция:  
`template <typename T>`, где `T` - это шаблонный тип.
Вместо `T` может быть подставлено любое другое название. Названия шаблонных типов принято называть с большой буквы для отличия от переменных.

Что же добавляет эта конструкция?
Это позволяет создавать функции, классы, using-и, которые могут работать с различными типами данных.

Посмотрим на примеры:
1. Функция `swap` для любых типов:
```
template <typename T>
void swap(T& a, T& b) {
  T tmp = a;
  a = b;
  b = tmp;
}
```
2. Функция `max`:
```
template <typename T>
T max(T x, T y) {
  return x > y ? x : y;
}
```
3. Шаблонный класс или структура:
```
template <typename T>
class vector {
  T* arr;
  size_t sz;
  size_t cap;
};
```
```
template <typename T>
struct greater {
  bool operator()(T& x, T& y) {
    return x > y;
  }
};
```
4. Шаблонный using:
```
using mymap = std::map<T, T, std::greater<T>>;
```

> шаблоны - это кодогенерирование компилятором одних и тех же конструкций с разными типами.

Для каждой рассмотренной конструкции компилятор создаст версию с определнным типом.
Например, запустим такую программу:
```
int a = 12;
int b = 13;
swap(a, b); 
// Сгенерируется версия swap для типа int.

long long al = 12;
long long bl = 13;
swap(al, bl);
// Сгенерируется версия swap для типа long long.
```
В данной ситуации будут сгенерированы две реализации одной функции. И генерация происходит только если мы вызываем шаблонную функцию с конкретным типом.

Вот в такой ситуации будет `CE`:
```
int a = 12;
long long bl = 13;
swap(a, bl);
```
Так как компилятор не понимает какую версию ему сгенерировать, ведь тип `T` у нас в функции `swap` одинаковый.
Мы можем явно написать шаблонный аргумент при вызове функции:
```
int a = 12;
long long bl = 13;
swap<long long>(a, bl);
```
Но это приведет к дургой ошибке конвертации `int&` к `long long&`.А такое нельзя. Ошибка не связана с шаблонами.  Если хотим чтобы все заработало:
```
template <typename T>
void swap(T a, T b) { // поменяли T& на T
  T tmp = a;
  a = b;
  b = tmp;
}
```
## $ 6.2 Template functions overloading.
При перегрузке шаблонных функций зачастую всё объясняется слудеющими правилами:
> 1. Частное предпочтительнее общего.
> 2. Точное соответствие лучше чем какой-либо каст.
> 3. Явное указание шаблонных параметров вызывает шаблонную функцию.

Пример для правила№1:
```
template <typename T>
void f(T x) {
  std::cout << 1;
}

void f(int x) {
  std::cout << 2;
}

int main() {
  int a = 10;
  f(a);
}
```
Выберится уже готова реализацяи `f` для инта.

Пример для правила№2:
```
template <typename T>
void f(T x) {
  std::cout << 1;
}

void f(long long x) {
  std::cout << 2;
}

int main() {
  int a = 10;
  f(a);
}
```
Пример для правила№3:
```
template <typename T>
void f(T x) {
  std::cout << 1;
}

void f(long long x) {
  std::cout << 2;
}

int main() {
  int a = 10;
  f<long long>(a);
}
```

Шаблонных аргументов может быть несколько, например:
```
template <typename T, typename U>
void f(T x, U y) {
  std::cout << x << y;
}

int main() {
  int x = 0;
  double y = 0.1;
  f(x, y);
}
```
В этом примере компилятор понимает какими типами будут `T` и `U` за счет передаваемых в функцию переменных.
Однако, если бы функция принимала только одну переменную то определить шаблонные типы было бы невозможно для компилятора:
```
template <typename T, typename U>
U f(T x) {
  std::cout << x << y;
}

int main() {
  int x = 0;
  f<int>(x);
}
```
В нашем примере `U` используется в качестве возвращаемого типа. Но даже при явном указании типа при вызове функции компилятор не поймет каким типом будет `U`. Это приведет к `CE`.

Переделаем немного этот пример:
```
template <typename U, typename T>
U f(T x) {
  std::cout << x << y;
}

int main() {
  int x = 0;
  f<int>(x);
}
```
В таком случае `U` будет типом `int` которое мы явно указали при вызове функции, а `T` тоже будет `int`, его компилятор узнает по типу переданной переменной `x`.

Можно указывать шаблонные аргументы по умолчанию, но как и с функциями они указываются в конце :
```
template <typename T, typename U = int>
U f(T x) {
  std::cout << 1;
  return 0;
}
```

Вопрос на закрепление: что будет выбрано в этом коде?
```
template <typename T>
void f(T& x) {
  std::cout << 1;
}

template <typename T>
void f(T x) {
  std::cout << 2;
}

int main() {
  int x = 0;
  f(x);
}
```
Ответ: `CE`. Смысл создания ссылок: ссылки не должны быть отличимы от самой переменной. Поэтому отличий при выборе варианта шаблонной функции нет.

```
template <typename T>
void f(T& x) {
  std::cout << 1;
}

template <typename T>
void f(T x) {
  std::cout << 2;
}

int main() {
  int x = 0;
  f(1); // edit
}
```
В этом примере выберится вариант `void f(T x)` так как в ссылки мы не можем помещать `rvalue`.

```
template <typename T>
void f(const T& x) { // edit
  std::cout << 1;
}

template <typename T>
void f(T x) {
  std::cout << 2;
}

int main() {
  int x = 0;
  f(1); // edit
}
```
Теперь снова будет `CE`, так как по константной ссылке можно принимать `rvalue`.

```
template <typename T>
void f(const T& x) { // edit
  std::cout << 1;
}

template <typename T>
void f(T& x) { // edit
  std::cout << 2;
}

int main() {
  int x = 0;
  f(x); // edit
}
```
Верси функции без константности лучше, так как она не навешивает константность.

## $ 6.3 Template specializations.
```
template <typename T>
class vector {
  T* arr;
  size_t sz;
  size_t cap;
};

// Full specialization
template <>
class vector<bool> {
  char *arr;
  size_t sz;
  size_t cap;
};

template <typename T, typename U>
struct S {};

//Partial specialization
template <typename W>
struct S<int, W> {};
```
> Full specialization - когда пишем шаблон с пустыми `<>`. И говорим что хотим определить частный случай конкретного набора аргументов.

> Partial specialization - когда пишем что-то в `<>`. И далее уточняем что-то частное.

Еще примеры частичной специализации:
```
template <typename T>
struct A {};

// Partial specialization
template <typename T>
struct A<T&> {};


// Partial specialization
template <typename T>
struct A<const T> {};
```

Как дела обстоят с функциями?
Для функция нет понятия частичной специализации, есть только полная специализация.  

```
template <typename T, typename U>
void f(T, U) {
  std::cout << 1;
}

// частичная специализация функции 1
template <>
void f(int, int) {
  std::cout << 11;
}

template <typename T>
void f(T, T) {
  std::cout << 2;
}

// частичная специализация функции 2 
template <>
void f(int, int) {
  std::cout << 4;
}
```
1. Перегрузка между шаблонами
2. Из шаблонов выбирается версия
3. Подстановка

## $ 6.4 Non-type template parameters.

Примером использования non-type template parameters является класс `std::array`.
```
#include <array>
int main() {
  std::array<int, 100> a;
}
// 100 - числовой параметр(non-type) 
```
Как такое создавать самим?
```
template <typename T, size_t N>
class array {
  T arr[N];
};
```
Как бы выглядело объявление класса матрицы произвольного размера, квадратного размера и функции умножения матриц используя числовые параметры:
```
template <size_t M, size_t N, typename Field = double>
class Matrix {};

template <size_t N, typename Field = double>
using SquareMatrix = Matrix<N, N, Field>;

template <size_t M, size_t K, size_t N, typename Field>
Matrix<M, N, Field> operator*(const Matrix<M, K, Field>& A, 
                              const Matrix<K, N, Field>& B);
```
Но их размер должен быть определен на момент компиляции.
Числовыми параметрами шаблонов пока будем думать, что являются `int`, `char`, `bool`.

### Template template parameters.

Это механизм в С++ позволяющий использовать шаблоны как аргументы для других шаблонов.
Например, пусть у нас есть шаблон какого-то контейнера `Container`. И мы хотим создать контейнер через этот шаблон используя шаблонный параметр типа `T`: `Container<T> arr`.
Как нам реализовать это в коде?
Для шаблонов шаблонов используется кострукция:  
`template <typename> class templ_temp_name>`
`class` - это устаревшее именование `typename`. Так что можете написать `typename` вместо `class`.  
`templ_temp_name` - навзвание параметра шаблона шаблонов.

Используя всю эту информацию напишем реальный код:
```
template <typename T, template <typename> typename Container>
class Stack {
   Container<T> arr_stack;
};

int main() {
  Stack<int, std::vector> st; 
  // std::vector - это встроенный контейнер
  return 0;
}
```

Можно так же указать `std::vector` по умолчанию:
```
template <typename T, 
          template <typename> typename Container = std::vector
         >
class Stack {
   Container<T> arr_stack;
};

int main() {
  Stack<int> st; 
  return 0;
}
```
Класс `std::vector` принимает в качестве неявного шаблона `std::allocator`. Мы его может указать сами при создании `Container`.
```
template <typename T, 
          template <typename, typename> typename Container = std::vector
         >
class Stack {
   Container<T, std::allocator<T>> arr_stack;
};

int main() {
  Stack<int> st; 
  return 0;
}
```

## $ 6.5 Basic compile time computations.
С помощью шаблонов можно производить различные вычисления в compile-time.
1. Напишем шаблон вычисляющий число Фиббоначи.
```
template <int N>
struct Fibbonachi {
  static constexpr int value = 
            Fibbonachi<N-2>::value + Fibbonachi<N-1>::value;
};

template <>
struct Fibbonachi<1> {
  static constexpr int value = 1;
};

template <>
struct Fibbonachi<0> {
  static constexpr int value = 0;
};

int main() {
  std::cout << Fibbonachi<20>::value;
}
```

2. Напишем шаблон проверяющий является ли число простым.
```
template <int N>
struct IsPrimeHelper {
  static constexpr bool value = N % D == 0 ? false : IsPrimeHelper<N, D-1>::value;
};

template <int N>
struct IsPrimeHelper<N, 1> {
  static constexpr bool value = true;
};

template <int N>
struct IsPrime {
  static constexpr bool value = IsPrimeHelper<N, N-1>::value;
};

template <>
struct IsPrime<1> {
  static constexpr bool value = false;
};

int main() {
  std::cout << IsPrime<37>::value;
}
```

## $ 6.6  Dependent names

1. Dependent names
```
template <typename T>
struct S {
  using A = int;
};

template <>
struct S<double> {
  static const int A = 5;
};

// int x = 0;

template <typename T>
void f() {
  S<T>::A* x;
}

int main() {
  f<int>();
}
```
`S<T>::A* x;` - это dependent или expresion?
В одной из версий шаблона `A` может быть типом `int`, а в другой версии `A` может быть числом `5`. И от этого `S<T>::A* x` может быть представлено как `int* x`, или как `5* x`.
Такие двоякие конструкции называются `dependent names` (зависимые имена).
На момент когда компилятор делает синтаксический парсинг он должен понимать, что из себя представляет эта конструкция. Стадия подстановки `T` выполняется намного позже, от чего и возникает проблема.
Все `dependent names` парсятся компилятором всегда как `expresion`.
Чтобы компилятор воспринимал `dependent names` как тип нужно написать перед этой конструкцией `typename`. 
Исправим наш код:
```
template <typename T>
struct S {
  using A = int;
};

template <>
struct S<double> {
  static const int A = 5;
};

// int x = 0;

template <typename T>
void f() {
  typename S<T>::A* x;
}

int main() {
  f<int>();
}
```
Но все равно будет ошибка когда мы вызовем: `f<double>();`

2. Dependent names
```
template <typename T>
struct S {
  template <int N>
  using A = std::array<int, N>;
};

template <>
struct S<double> {
  static const int A = 5;
};

// int x = 0;

template <typename T>
void f() {
  typename S<T>::A<5> x;
}

int main() {
  f<int>();
}
```
Выражение `S<T>::A<5> x;` может быть воспринято еще как `S<T>::A < 5 > x;`
Когда пишем `typename` компилятор считаеет что эта конструкция это название типа, но не считает это названием шаблона. А для решения этого уже нужно прописать `template`.
```
template <typename T>
void f() {
  typename S<T>::template A<5> x;
}
```
Но `g++-11` компилирует это и без использования `template`.

3. Dependent names.
```
template <typename T>
struct S {
  template<int N>
  void foo(int) {}
};

template <>
struct S<double> {
  const int foo = 1;
};

template <typename T>
void bar(int x, int y) {
  S<T> s;
  s.foo<5>(x + y);
}

int main() {
  bar<int>(2, 3);
}
```
Выражение `s.foo<5>(x+y);` можно распарсить как s.foo < 5 > (x + y), где ss.foo - может быть переменной.И компилятору не понятно уже какое синтаксическое дерево парсинга строить.
Решение добавить `template`.
```
template <typename T>
void bar(int x, int y) {
  S<T> s;
  s.template foo<5>(x + y);
}
```

4. Dependent names
```
template <typename T>
struct Base {
  int x = 0;
};

template<>
struct Base<double> {
};

template <typename T>
struct Derived: Base<T> {
  void f() {
    ++x; // Dependent name
  }
};

int main() {}
```
В зависимости от `T` переменная `x` может присутствовать и может отсутствовать.  
Решение: при обращении к полю шаблоноого родителя нужно использовать `this` или написать напрямую из какого класса брать переменную.
```
template <typename T>
struct Derived: Base<T> {
  void f() {
    ++this->x;
  }
};
```
```
template <typename T>
struct Derived: Base<T> {
  void f() {
    ++Base<T>::x;
  }
};
```

## $ 6.7 Metafunctions and type traits.

> Metafunctions - функции которые принимают типы и возвращают типы.

Напишем метафункцию для проверки равенства двух типов.
```
template <typename T, typename U>
struct is_same {
  static constexpr bool value = false;
};

template <typename T, typename T>
struct is_same {
  static constexpr bool value = bool;
};
```
Теперь мы можем во время компиляции проверить равенство типов и в зависимости от этого делать что-то еще. Например:
```
template <typename T, typename U>
void f(T x, U y) {
  if constexpr (is_same<T, U>::value) {
    // code example
    x = y;
    //
  }
}
int main() {
  f<int, std::string>();
}
```
`if constexpr` появился в `C++14`. Отличается тем, что при ложности переданного выражения не будет генерироватьcя код внутри этого условия.

На самом деле в стандартной библиотеке С++ уже есть написанный класс `std::is_same`. Для использования: `#include <type_traits>`.

Еще парочку функция из стандартной бибилотеки:
```
template <typename T>
struct remove_reference {
  using type = T;
};

template <typename T>
struct remove_reference<T&> {
  using type = T;
};

template <typename T>
void f() {
  typename remove_reference<T>::type x;
}
```
Ну и конечно же `remove_reference<T>::type` это тип, а не выражение поэтому перед ним пишем `typename`.

```
template <typename T>
struct remove_const {
  using type = T;
};

template <typename T>
struct remove_const<const T> {
  using type = T;
};

template <typename T>
void f() {
  typename remove_const<T>::type x;
}
```

В стандартной реализации можно увидеть, что переменные `bool` вынесены в отдельные класса и их просто наследуют.
Пример:
```
template <typename T>
struct false_type {
  static constexpr bool type = false;
};

template <typename T>
struct true_type{
  static constexpr bool type = true;
};

template<typename T>
struct is_array: false_type {};

template<typename T>
struct is_array<T[]>: true_type {};

template<typename T>
struct is_array<T[N]>: true_type {};
```
Еще интересные метафункции
```
1. is_member_pointer
2. rank
3. dekay
```
Ссылка на все метафункции: https://en.cppreference.com/w/cpp/meta

Напишем свой тернарный оператор(он тоже есть в стандарте):
```
template <bool B, typename T, typename F>
struct conditional {
  using type = F;
};

template <typename T, typename F>
struct conditional<true, T, F> {
  using type = T;
};

int main() {
  int x = 1, y = 2;
  typename conditional<x < y, int, float>::type;
}
```
И снова такая огромная конструкция для вызова. Чуваки из комитета стандартизации тоже так подумали и завезли в С++11 алиасы для таких конструкций:
```
template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

// Теперь можно юзать так:
conditional_t<x > y, int, float>;
```
А в С++17 подумали, что и для `value` можно сделать алиасы:
```
template <typename T, typename U>
const boool is_same_v = is_same<T, U>::value;
```

## $ 6.8 Varidatic templates (since C++11)
```
template <typename.. Types>
void f(Types... tx) {

}
```
`template <typename.. Types>` - объявили пачку типов.
`Types.. tx` - распоковали пачку типов.

Давайте на практике напишем функцию вывода на консоль элементов.
Для получения элементов из пачки мы будем использовать отдельно первый аргумент.
```
void print() {}

template <typename Head, typename... Tail>
void print(const Head& head, const Tail&... tail) {
  std::cout << head << ' ';
  print(tail...);
}

int main() {
  print(1, 2.0, "abc");
}
```
Напишем метафункцию проверяющая равенство типов:
```

template <typename First, typename Second, typename... Types>
struct is_homogeneous {
  static constexpr bool value = std::is_same_v<First, Second> 
          && is_homogeneous<Second, Types...>::value;
};


template <typename First, typename Second>
struct is_homogeneous  <First, Second> {
  static constexpr bool value = std::is_same_v<First, Second>;
};
```

Cуществует оператор возвращающи число размера пакетов. Это `sizeof...()`.
```
sizeof...(tail);
```


