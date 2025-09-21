#include <iostream>
#include <functional>

#ifdef SAMPLE_1

void PrintNum(int num) {
  std::cout << "Num = " << num << std::endl;
}

int main() {
  auto bind_func = std::bind(PrintNum, 10);
  bind_func(); // Output: Num = 10
  return 0;
}

#elif SAMPLE_2

void Add(int a, int b) {
  std::cout << a << " + " << b << " = " << a + b << std::endl;
}

int main() {
  auto AddFive = std::bind(Add, 5, std::placeholders::_1);
  AddFive(10); // Output: 5 + 10 = 15
  return 0;
}

#elif SAMPLE_3

class Calculator {
public:
  int Multiply(int a, int b) {
    return a * b;
  }
};

int main() {
  Calculator calc;
  auto MultiplyByTwo = std::bind(&Calculator::Multiply, &calc,
                                 2, std::placeholders::_1);
  std::cout << MultiplyByTwo(5) << std::endl; // Output: 10
  return 0;
}

#elif SAMPLE_4

void Subtract(int a, int b) {
  std::cout << a << " - " << b << " = " << a - b << std::endl;
}

int main() {
  auto ReverseSubtract = std::bind(Subtract,
                                   std::placeholders::_2,
                                   std::placeholders::_1);
  ReverseSubtract(10, 20); // Output: 10
  return 0;
}

#elif SAMPLE_5

#include <vector>
#include <algorithm>

bool IsEven(int n) {
  return n % 2 == 0;
}

int main() {
  std::vector<int> numbers = {1, 2, 3, 4, 5, 6};
  auto count_even = std::count_if(numbers.begin(), numbers.end(),
                                 std::bind(IsEven, std::placeholders::_1));
  std::cout << count_even << std::endl; // Output: 3
  return 0;
}

#elif SAMPLE_6

void Divide(int a, int b) {
  if (b != 0)
    std::cout << a << " / " << b << " = " << a / b << std::endl;
  else
    std::cout << "Division by zero!" << std::endl;
}

int main() {
  auto DivideBy = std::bind(Divide,
                            std::placeholders::_1,
                            std::placeholders::_2);
  DivideBy(10, 2); // Output: 5
  DivideBy(10, 0); // Output: Division by zero!
  return 0;
}

#elif SAMPLE_7

class Counter {
public:
  void Increment(int& value) {
    ++ value;
  }
};

int main() {
  Counter counter;
  int a = 5, b = 10;
  auto IncrementA = std::bind(&Counter::Increment, &counter, std::ref(a));
  auto IncrementB = std::bind(&Counter::Increment, &counter, std::ref(b));
  std::cout << a << ", " << b << std::endl; // Output: 5, 10
  IncrementA();
  IncrementB();
  std::cout << a << ", " << b << std::endl; // Output: 6, 11
  return 0;
}

#elif SAMPLE_8

#include <vector>
#include <algorithm>

int Multiply(int a, int factor) {
  return a * factor;
}

int main() {
  std::vector<int> numbers = {1, 2, 3, 4, 5};
  std::vector<int> results(numbers.size());
  auto MultiplyByThree = std::bind(Multiply, std::placeholders::_1, 3);
  std::transform(numbers.begin(), numbers.end(),
                 results.begin(), MultiplyByThree);
  for (int n : results) {
    std::cout << n << " "; // Output: 3 6 9 12 15
  }
  std::cout << std::endl;
  return 0;
}

#elif SAMPLE_9

#include <thread>

void ThreadFunction(int a, int b) {
  std::cout << "Thread sum: " << a + b << std::endl;
}

int main() {
  auto BoundFunction = std::bind(ThreadFunction, 5, 10);
  std::thread t(BoundFunction);
  t.join(); // Output: Thread sum: 15
  return 0;
}

#endif
