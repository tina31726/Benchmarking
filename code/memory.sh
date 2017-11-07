echo 'Running program to do sequential memory access, sequential write and random write using 8B,8KB, 8MB and 80MB block size'
gcc1 -pthread memory_test.c
./a.out