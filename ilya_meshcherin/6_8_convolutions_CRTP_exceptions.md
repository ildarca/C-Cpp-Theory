## $ 6.9 Fold expressions (since C++17)

С С++17 добавли новый способ раскрывать параметры.
Сразу посмотрим на коде, напишем проверку на то что все параметры являются указателями:
```
template <typename... Types>
struct all_pointers {
  static const bool value = (std::is_pointer_v<Types> && ...);
};
```
`(std::is_pointer_v<Types> && ...)` - это `Fold expressions`: когда мы берем выражение в котором содержится пакет параметров и соединяем его через многоточие каким-то оператором. В данном случае конъюнкиция. И круглые скобки обязательны :).

Теперь напишем `is_homogeneous` через `Fold expressions`:
```
template <typename Head, typename... Tails>
struct is_homogeneous {
  static const bool value = (std::is_same_v<Head, Tail> && ...);
};
```
Реализуем `print`:
```
template <typename... Types>
void print(const Types&... types) {
   (std::cout << ... << types);
};
```
Различают 4 способа раскрытия пакетов:
1. (pack op ...) 
2. (... pack op)
3. (pack op ... op init)
4. (init op ... op pack)

Они будут раскрывать в таком виде:
1. (E_1 op (... op (E_n-1 op E_n)));
2. (((E_1 op ) op ...) op E_n);
3. (E op ... op I) = (E_1 op (... op (E_n-1 op (E_n op I))));
4. (I op ... op E) = ((((I op E_1) op ...) op E_n);

Подробнее можно ознакомиться: https://en.cppreference.com/w/cpp/language/fold

## $ 6.10 CRTP(Curiously Recurring Template Pattern)
Заключается паттерн в том, что у нас есть шаблонный класс родитель и дочерний класс который наследуется от родителя с шаблонным типом дочернего класса. Звучит ужасно. Поэтому пример:
```
template <typename T>
class Base {
  // методы внутри Base могут использовать шаблон для доступа к членам De ived
};

class Derived: public Base<Derived> {
  // ...
};
```
За счет этого в методах класса `Base` могут использоваться поля и методы из `Derived`.

Пример (случай статического полиморфизма):
```
template <typename T>
struct Base {
  void interface() {
    static_cast<T*>(this)->implementation();
  }

  static void static_func() {
    T::static_sub_func();
  }
};

struct Derived: public Base<Derived> {
  void implementation();
  stativ void static_sub_func();
};
```
Еще есть пример с классом векторов для сложения их конкретных элементов без сложения их самих. Где? Что?

