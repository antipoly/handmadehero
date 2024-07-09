#include <iostream>
#include <cmath>
#include <vector>

namespace foo
{
  int x = 5;
}

void foreach ();
void memory();
void classes();

int main(int argc, char const *argv[])
{

  /**
   * int                    : 4 bytes (-2,147,483,648 to +2,147,483,647)
   * unsigned int           : 4 bytes (0 to +4,294,967,295)
   * short                  : 2 bytes (-32,768 to +32,767)
   * unsigned short         : 2 bytes (0 to +65,535)
   * long long              : 8 bytes (-9 quintillion to +9 quintillion)
   * unsigned long long     : 8 bytes (0 to +18 quintillion)
   *
   * float                  : 4 bytes
   * double                 : 8 bytes
   * bool                   : 1 byte
   */

  // using namespace foo; any variable in the namespace can be used without prefix
  // using std::cout; cout can be used without prefix

  // void abc() {
  //   std::string message = "Hello, World!";
  //   std::cout << message << std::endl; /* endl is a manipulator that adds a newline and flushes the stream (immediately writes to console) */
  //   std::cout << "Hi" << "\n"; /* A newline can be used if only the newline element is desired */

  //   std:cout << foo::x; // scope resolution operator
  // }

  // int x = (int)3.14; // 3
  

  // foreach();
  classes();
  return 0;
}

void types()
{
  typedef std::vector<std::pair<int, std::string>> pairlist_t;
  pairlist_t pairlist;

  // using keyword is more common
  using text_t = std::string;
}

void io()
{
  int age;
  std::string name;

  std::cout << "What's your name?: ";

  std::cin >> name;             // cin reads up to the first whitespace
  std::getline(std::cin, name); // getline reads the entire line

  /* If `std::cin` is used before getline */
  std::cin >> age;
  std::getline(std::cin >> std::ws, name); // removes any whitespace before the input

  /* Alternatively */
  std::cin.ignore(); // ignores the newline character
}

void foreach()
{
  std::string students[] = {"John", "Jane", "Michael"};
  for (std::string student : students)
  {
    std::cout << student << "\n";
  }
}

void memory() {
  int *p = new int; // allocate memory
  *p = 5;
  delete p; // deallocate memory

  // ---

  int size;
  char *pGrades = NULL;

  std::cout << "Enter size: ";
  std::cin >> size;

  pGrades = new char[size];

  for (int i = 0; i < size; i++)
  {
    std::cout << "Enter grade #" << i + 1 << ": ";
    std::cin >> pGrades[i];
  }

  delete[] pGrades;

}

template <typename T, typename U>
auto max(T x, U y) {
  return (x > y) ? x : y;
}

void structs() {
  struct Point {
    int x, y;
  };

  // dont have to say struct before
  // Point p;
  // p.x = 5;
  // p.y = 10;

  Point *p = new Point;
  p->x = 5;
  p->y = 10;
}

class Human {
  public:
    std::string name;
    std::string occupation;
    int age;

    void eat() {
      std::cout << name << " is eating" << std::endl;
    }

    void sleep() {
      std::cout << name << " is sleeping" << std::endl;
    }

  Human(std::string name, std::string occupation, int age) {
    this->name = name;
    this->occupation = occupation;
    this->age = age;

  };
};

class Student : public Human {
  public:
    std::string major;

    Student(std::string name, std::string occupation, int age, std::string major) : Human(name, occupation, age) {
      this->major = major;
    }

    void study() {
      std::cout << name << " is studying" << std::endl;
    }
};

void classes() {
  Human human1("John", "Developer", 25);
  human1.name = "John";
  human1.occupation = "Developer";
  human1.age = 25;

  human1.eat();
  human1.sleep();
}