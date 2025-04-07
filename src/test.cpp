#include <stdio.h>

struct A{
int a;
};
struct B {int b;};
struct C: A, B {int c;};

int main() {
  C c;
  B* pb = &c;
  A *pa = &c;
  printf("%p, %p, %p", pa, pb, &c);} 
