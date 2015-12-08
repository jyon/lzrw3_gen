#include <stdio.h>

#define KB 1024
#define UBYTE unsigned char 
#define UWORD unsigned int
#define HASH_TABLE_LENGTH 4096

UBYTE* hashTable[HASH_TABLE_LENGTH];

void main(int argc, char *argv[])
{

	UBYTE a[16*KB];
	UBYTE b[17*KB];
	UWORD size;
	UWORD x;
	int comp;
	int all = 0;
	int i = 0, j;
	int k;
	char* error_string = "usage: test data_size [-a] [-c comp]\n\t-a: test for all comp (0 ~ 85)\n\t-c comp: test for comp (0 ~ 85)\n";


	if(argc==1) {
		printf("%s", error_string);
		return;
	} else if (argc==2) {
		if((k=atoi(argv[1])) < 0) {
			printf("%s", error_string);
			return;
		}
		all = 1;
	} else if (argc==3)  {
		if(argv[2][0] != '-' || argv[2][1] != 'a') {
			printf("%s", error_string);
			return;
		}

	} else if (argc == 4) {
		if(argv[2][0] != '-' || argv[2][1] != 'c') {
			printf("%s", error_string);
			return;
		} else if((comp = atoi(argv[3] + 1)) < 0 || comp > 85) {
			printf("%s", error_string);
			return;
		}
	} else {
		printf("%s", error_string);
		return;
	}


	k = atoi(argv[1]);

	x = k * KB;	
	if(all) {
		printf("data size = %d\n", x);
		for(comp = 0; comp < 86; comp++) {

			for(i = 0; i < 16*KB; i++) {
				a[i] = 0;
				b[i] = 0; 
			}

			lzrw3_gen(comp, x, a, hashTable);
			size = blueftl_lzrw3_compress(a, x, b, hashTable);

			printf("(result: %d / %d = %f)", (x - size), x , (double) (x- size) / x);
			printf("(expect: %d / %d = %f)\n", x - x * (100 - comp) / 100, x, (double) (x- x* (100 - comp) / 100) / x);
			
		}
	} else {
		comp = atoi(argv[3]);
		lzrw3_gen(comp, x, a, hashTable);
		size = blueftl_lzrw3_compress(a, x, b, hashTable);

		printf("(result: %d / %d = %f)", (x - size), x , (double) (x- size) / x);
		printf("(expect: %d / %d = %f)\n", x - x * (100 - comp) / 100, x, (double) (x- x* (100 - comp) / 100) / x);
	}
	return;
}
