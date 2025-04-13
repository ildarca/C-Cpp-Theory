#include <iostream>

struct Base {
  int x;
  void f() {}
};

struct Derived : Base {
  int y;
  void g() {}
};

int main() {
  Derived d;
  std::cout << d.x;
  return 0;
}
