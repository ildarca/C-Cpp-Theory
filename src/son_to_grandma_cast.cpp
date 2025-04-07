struct Grandma {};
struct Mom: Grandma {};
struct Ded: Grandma {};
struct Son: Mom, Ded {};

int main() {
  Son son;
  Mom& mom = son;
  Grandma &gr = mom;
  return 0;
}
