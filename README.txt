gcc main.c
./a.out test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt

Если сделать 
valgrind ./a.out test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt
valgrind показывает какие-то ошибки, gdb их не видит
Не получается исправить! 
А так всё работает.
