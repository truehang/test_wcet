#include "test_wcet.h"
#include <iostream>
#include "user_func.h"

int main() {
  TaskLoader sc;
  unsigned a = 0;
  unsigned b = 1;
  /*添加任务A，对应的函数cmp,参数为 a,b*/
  sc.addTask("A", cmp, a, b);
  /*添加任务B，对应的函数printf_hello*/
  sc.addTask("B", printf_hello);

  functor_type_n ftype(4);
  sc.addTask("C", ftype);

  // test member function a_new_func of ftype
  unsigned mem_in = 2;
  sc.addTask("C1", &functor_type_n::a_new_func, ftype, mem_in);

  // test lambda
  int x = 1, y = 2;
  // Variable x is captured by value, y by reference.
  auto lmd1 = [x, &y]() mutable {
    x *= 2;
    ++y;
    for (int i = 1000000; i > 0; --i)
      ;
    printf("x * y %d\n", x * y);
  };
  sc.addTask("D", lmd1);

  /* run all test*/
  sc.RunTest();
  return 0;
}