#ifndef TEST_WCET_HEADER
#define TEST_WCET_HEADER
#include <cstdlib>
#include <iostream>
#include <memory> /* std::unique_ptr*/
#include <string>
#include <tuple> /* std::tuple*/
#include <vector>

class Invoker {
public:
  template <typename Callable, typename... Args>
  void loadFunc(Callable&& f, Args&&... args) {
    _S_make_state(std::tuple<Callable, Args...>{std::forward<Callable>(f),
                                                std::forward<Args>(args)...});
    ready_ = 1;
  }
  void runFunc() {
    if (ready_) invoker_->run();
  }

private:
  bool ready_;

  struct __Invoker_Base {
    virtual void run() = 0;
  };

  template <typename _Tuple>
  struct _Invoker : public __Invoker_Base {
    _Tuple _M_t;
    _Invoker(_Tuple&& tp) : _M_t{tp} {}
    template <size_t _Index>
    static std::__tuple_element_t<_Index, _Tuple>&& _S_declval();

    template <size_t... _Ind>
    auto _M_invoke(std::_Index_tuple<_Ind...>) noexcept(
        noexcept(std::__invoke(_S_declval<_Ind>()...)))
        -> decltype(std::__invoke(_S_declval<_Ind>()...)) {
      return std::__invoke(std::get<_Ind>(std::move(_M_t))...);
    }

    using _Indices = typename std::_Build_index_tuple<
        std::tuple_size<_Tuple>::value>::__type;

    virtual void run() override { _M_invoke(_Indices()); }
  };

  template <typename _Callable>
  void _S_make_state(_Callable&& __f) {
    invoker_ = std::unique_ptr<_Invoker<_Callable>>(
        new _Invoker<_Callable>(std::forward<_Callable>(__f)));
  }

  std::unique_ptr<__Invoker_Base> invoker_;
};

class TaskLoader {
public:
  template <typename Callable, typename... Args>
  bool addTask(const std::string &name, Callable &&f, Args &&...args) {

    auto it = mTaskArr.emplace(mTaskArr.end(), name, Invoker{});
    it->second.loadFunc(std::forward<Callable>(f), std::forward<Args>(args)...);
    return true;
  }
  bool RunTest();
private:
  std::vector<std::pair<std::string, Invoker>> mTaskArr;
};

#endif