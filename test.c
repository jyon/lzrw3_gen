#include <stdio.h>

#define KB 1024
#define UBYTE unsigned char 
#define UWORD unsigned int
#define HASH_TABLE_LENGTH 4096

#define ABS(x) ((x)>0)? x:(-x)

UBYTE* hashTable[HASH_TABLE_LENGTH];

void main(int argc, char *argv[])
{

	UBYTE a[160*KB];
	UBYTE b[170*KB];
	UWORD size;
	UWORD x;
	int comp;
	int all = 0;
	int i = 0, j;
	int k;
	char* error_string = "usage: test data_size [-a] [-c comp]\n\t-a: test for all comp (120 ~ 1125)\n\t-c comp: test for comp (120 ~ 1125)\n";


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
		} else if((comp = atoi(argv[3])) < 120 || comp > 1125) {
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
		for(comp = 1056; comp >= 143; comp--) {


			lzrw3_gen(comp, x, a, hashTable);
			size = blueftl_lzrw3_compress(a, x, b, hashTable);

			int c = size;
			printf("result: %5d / %5d = %3.3f\t", c, x , (double) c / x);
			printf("expect: %5d / %5d = %3.3f\t", x * comp / 1000, x, (double) (x* comp / 1000) / x);
			int error = (x * comp/1000) - c;
			printf("error: %5d\terror rate: %3.3f\n", error, (double) error / x);
			
		}
	} else {
		comp = atoi(argv[3]);
		lzrw3_gen(comp, x, a, hashTable);
		size = blueftl_lzrw3_compress(a, x, b, hashTable);
		int c = size;
		printf("result: %5d / %5d = %3.3f\t", c, x , (double) c / x);
		printf("expect: %5d / %5d = %3.3f\t", x * comp / 1000, x, (double) (x* comp / 1000) / x);
		int error = (x * comp/1000) - c;
		printf("error: %5d\terror rate: %3.3f\n", error, (double) error / x);
	}
	return;
}
