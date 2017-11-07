echo 'Running program to find GIOPS and GFLOPS without AVX instructions'
gcc1 -pthread cpu_test_original.c
./a.out

echo 'Running program to find GIOPS and GFLOPS with AVX instructions'
gcc1 -mavx2 -pthread cpu_avx.c
./a.out