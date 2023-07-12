#ifndef USER_CPP_FUNC_DEFINE_HPP_HEADER
#define USER_CPP_FUNC_DEFINE_HPP_HEADER
#include <iostream>
bool cmp(unsigned &a, unsigned &b);

void printf_hello();

struct functor_type {
  void operator()() const {
    std::cout << "Hello World from a functor!\n";
    for (int i = 0; i < 1000000; ++i)
      ;
  }
};

struct functor_type_n {
  int m_n;

  functor_type_n(int n) : m_n(n) {}

  void operator()() const {
    for (int i = 0; i < m_n; ++i) {
      std::cout << "Hello World from a functor n = " << m_n << "\n";
      for (int i = 0; i < 1000000; ++i)
        ;
    }
    std::cout << "functor_type_n done\n";
  }

  void a_new_func(unsigned &i){
    for (int i = 0; i < 1000000; ++i)
      ;
    std::cout << "i in a_new_func: " << i << std::endl;
    i += 8; 
  }
  
};

#endif