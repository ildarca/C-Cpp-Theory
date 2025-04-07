## $5.1 Idea of virtual function.
Начнем с базового примера:

     Base
       ^
       |
    Derived
```
struct Base {
    void f() {
        std::cout << 1;
    }
};

struct Derived: Base {
    void f() {
        std::cout << 2;
    }
};

int main() {
    Derived d;
    Base& b = d;
    b.f(); // 1
}
```

Результатом работы будет `1`.  
Но если рассуждать у нас просто появилась ссылка базового типа на наш объект. А сам объект остался тем же. По логике мы должны получить `2`.  

Если отойти от С++ и взять математику. Представим нам нужно посчитать площадь криволинейной трапеции в форме квадрата. Обычно для криволинейных трапеций высчитывается интеграл, но мы же с вами знаем, что у нас фигура квадрата. И просто применем формулу площади квадрата, не считая интеграл для общего случая. Вот в нашей ситуации так же должно было произойти. Мы просто должны были вызвать функцию конкретного случая, а не базовую функцию.

> В C++ через ссылку (указатель) базового класса на класс наследника вызывается метод базового класса вместо метода наследника.

Чтобы исправить эту логику нужно сделать метод виртуальным:
```
struct Base {
    virtual void f() {
        std::cout << 1;
    }
};

struct Derived: Base {
    void f() {
        std::cout << 2;
    }
};

int main() {
    Derived d;
    Base& b = d;
    b.f(); // 2
}
```
Результат: `2`

> В С++ полиморфным(polymorphic type) называется структура, у которого есть виртуальный метод или метод унаследованный от виртуального.

В нашем примере метод `f()` из `Base` виртуальный т.к. использует ключевое слово `virtual`, метод `f()` из `Derived` тоже виртуальный т.к. переопределяется от вирутального `f()` базового класса.

Посмотрим важный пример.
```
struct Base {
    virtual void f() {
        std::cout << 1;
    }
};

struct Derived: Base {
    int* p = new int();
    void f() {
        std::cout << 2;
    }
    ~Derived() {
        delete p;
    }
};

int main() {
    Derived* b = new Derived();
}
```
В нашем примере будет утечка памяти! Связано все с тем же, что имея указатель на базовый класс у нас вызовется деструктор базового класса. Тем самым память не очищается.   
Решение: использовать виртуальный деструктор. 
```
struct Base {
    virtual void f() {
        std::cout << 1;
    }
    virtual ~Base() = default;

};

struct Derived: Base {
    int* p = new int();
    void f() {
        std::cout << 2;
    }
    ~Derived() {
        delete p;
    }
};

int main() {
    Derived* b = new Derived();
}
```

Но почему деструктор по умолчанию не виртуальный? Связано это с доп. расходами виртуальных функций.

## $ 5.2 More complicated examples with virtual functions.

> Виртуальной считается функция в точности совпадающей по сигнатуре.

Если мы хоть немного ошибемся в сигнатеру функции, то снова будет вызвана функция базового класса вместо метода класса наследника.

```
struct Base {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Derived: Base {
    void f() {
        std::cout << 2;
    }
};

int main() {
    Derived d;
    Base& b = d;
    b.f(); // 1
}
```
> override - был добавлен в С++11 для решения данной проблемы. Он вызывает ошибку компиляции, если мы допустили опечатку в сигнатуре виртуальной функции.

```
struct Base {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Derived: Base {
    void f() override {
        std::cout << 2;
    }
};

int main() {
    Derived d;
    Base& b = d;
    b.f(); // CE 
```

Теперь у нас будет вызвана CE и мы исправим опечатку.

Правильный вариант программы:

```
struct Base {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Derived: Base {
    void f() const override {
        std::cout << 2;
    }
};

int main() {
    Derived d;
    Base& b = d;
    b.f(); // 2;
}
```

> `final` - запрещает всем наследникам переопределять функцию с такой же сигнатурой(C++11). `final` позволяет делать компилятору некоторые оптимизации.

`final` автоматически означается override.  
Пример:

```
struct Base {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Derived: Base {
    void f() const final {
        std::cout << 2;
    }
};

int main() {
    Derived d;
    Base& b = d;
    b.f(); // 2;
}
```

Прикол: override, final является контексто зависимыми, а не ключевыми словами. За счет этого можно делать вот так:
```
int override = 5; 
```
Рассмотрим более сложные примеры:
```
struct Granny {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Mom: Granny {
    void f() const override {
        std::cout << 2;
    }
};

struct Son: Mom {
    void f() const final {
        std::cout << 3; 
    }
};

int main() {
    Mom d;
    Granny& b = d;
    b.f(); // 2;
}
```

В иерархии вызывается самая глубокая виртуальная функция. В данной ситуации это метод класса Mom.

Представим более сложный пример:
```
struct Granny {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Mom: Granny {
private: // <---
    void f() const override {
        std::cout << 2;
    }
};

struct Son: Mom {
    void f() const final {
        std::cout << 3; 
    }
};

int main() {
    Mom d;
    Granny& b = d;
    b.f(); // 2;
}
```
Результат будет `2`. Но почему?  
private -  определяется в compile time. virtual - run time. Получается то какая функция будет выбрана определяется в run time. А легален ли этот вызов определяется в compile time. Компилятор в compile time не понимает в какую функцию мы попадем. В такой ситуации компилятор либо должен кидать СЕ либо забить на приватность. Но он всегда забивает на приватность.

Еще пример:
```
struct Ded {
    virtual void f() const {
        std::cout << 1;
    }
};

struct Mom {
    void f() const {
        std::cout << 2;
    }
};

struct Son: Mom, Ded {
    void f() const final {
        std::cout << 3; 
    }
};

int main() {
    Son s;
    Ded& d = s;
    Mom& m = s;
    d.f(); // 3;
    m.f(); // 2
}
```

## $ 5.3 Abstract classes and pure virtual functions.

Часто бывает что мы определяем класс у которого не можем определить реализацию методов конкретно. Например класс геометрическая фигура и мы не понимаем как определить метод подсчета площади ведь фигура понятие очень общее. Но при этом мы хотим, чтобы любой наш наследник переопредлял наш метод подсчета площади.

> pure virtual function - это метод, у которого нет реализации и он должeн быть переопределен в классе наследнике. Для обозначения таких методов используется virtual в начале сигнатуры метода и `= 0` в конце.

> Abstract class - это класс, у которого есть хоть один pure virtual function. Объект абстрактного типа создать нельзя!

Пример:
```
struct Shape { // abstract class
    virtual area() const  = 0; // pure function
    virtual ~Shape() = default;
};

struct Square: Shape {
    double a;
    Square(double a): a(a) {}
    double area() const override {
        return a*a;
    }
};

struct Circle: Shape {
    double r;
    Circle(double r): r(r) {}
    double area() const override {
        return 3.14 * r * r;
    }
};

int main() {
    Square sq(1.0);
    Shape& s = sq;
    s.area();
}
```

> Polymorphism - это возможность функции или объекта выполнять одно и то же действие в зависимости от реализации.   
В нашем примере мы можем считать площади по разному в зависимости от самой фигуры.

```
struct Shape { 
    virtual area() const  = 0; 
    virtual ~Shape() = default;
};

struct Square: Shape {
    double a;
    Square(double a): a(a) {}
    double area() const override {
        return a*a;
    }
};

struct Circle: Shape {
    double r;
    Circle(double r): r(r) {}
    double area() const override {
        return 3.14 * r * r;
    }
};

int main() {
    std::vector<Shape*> v;
    v.push_back(new Square(1.0));
    v.push_back(new Circle(1.0));

    for (Shape* s: v) {
        std::cout << s->area() << '\n';
    }
}
```
В этом примере для каждой функции вызовется свой метод площади. 
Результат: 
```
1.0
3.14
```
Это пример динамического полиморфизма, который происходит в run time. Так же существует статический полиморфизм - перегрузка функций и операторов.

Замечание №1.  
Мы можем явно вызывать метод родительского класса даже если он виртуальный.  
Допустим в нашем абстрактном классе Shape был метод `f()`. Мы можем его вызвать через наследника вот так:
```
Circle c(1.0);
c.Shape::f();
```
В compile time будет выбрана версия родителя.

Замечание №2.  
Для pure functions мы можем писать определение. Класс все так же останется абстрактным и метод все так же останется `pure`. Это может служить как заглушка или на случай если наследник не справится с переопределением и просто вызовет готовую реализацию.
Пример:

```
struct Shape { 
    virtual area() const  = 0; 
    virtual ~Shape() = default;
};

double Shape::area() const { // определение pure метода
    return 0.0;
}

struct Square: Shape {
    double a;
    Square(double a): a(a) {}
    double area() const override {
        return a*a;
    }
};

struct Circle: Shape {
    double r;
    Circle(double r): r(r) {}
    double area() const override {
        return 3.14 * r * r;
    }
};

int main() {
    Circle c(1.0);
    c.Shape::area(); // 0.0
    return 0;
}
```






