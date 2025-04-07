#include <iostream>

struct Base() {
  int x;
  void f() {}
};

struct Derived {
  int y;
  void g() {}
};

int main() {
  Derived d;
  d.x;
  return 0;
}
