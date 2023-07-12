请使用sudo 权限运行程序，否则不能设置实时优先级
编译：
`g++ test_wcet.cpp test_wcet.h user_func.cpp user_func.h main.cpp -lrt -o test_wcet`
运行：
`sudo ./test_wcet`
清除编译文件：
`rm test_wcet`