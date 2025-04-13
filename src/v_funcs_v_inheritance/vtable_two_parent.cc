class Mother {
 public:
  virtual void MotherMethod() {}
  int mother_data;
};

class Father {
 public:
  virtual void FatherMethod() {}
  int father_data;
};

class Child : public Mother, public Father {
 public:
  virtual void ChildMethod() {}
  int child_data;
};

int main() {
  Mother m;
  Father f;
  Child c;
  return 0;
}
