echo 'Running program to collect 600 IOPS and FLOPS'
gcc1 -pthread 600_sample_test.c
./a.out>>sample.file
