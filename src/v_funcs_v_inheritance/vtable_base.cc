#include <iostream>

class NonVirtualClass {
 public:
  void foo() {}
};

class VirtualClass {
 public:
  virtual void foo() {}
};


int main() {
  std::cout << "Size of NonVirtualClass: "
            << sizeof(NonVirtualClass) << std::endl;
  std::cout << "Size of VirtualClass: "
            << sizeof(VirtualClass) << std::endl;
}
