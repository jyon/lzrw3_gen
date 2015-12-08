all:
	gcc -o test test.c blueftl_lzrw3.c lzrw3_gen.c

clean:
	rm -rf test
