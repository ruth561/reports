
SRCS = benchmark.c high.c naive.c normal.c

.PHONY: run
run:
	gcc $(CFLAGS) -fopenmp -o benchmark $(SRCS)
	./benchmark