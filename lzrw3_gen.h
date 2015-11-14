#define UBYTE unsigned char
#define UWORD unsigned int

#define MAX_RAW_ITEM 18
#define MAX_RAW_GROUP 16 * MAX_RAW_ITEM
#define MAX_CMP_GROUP (2 + 16*2)

#define ITEMS_PER_GROUP 16

#define HASH_TABLE_LENGTH 4096
#define START_STRING (UBYTE*) "123456789012345678"

#define HASH(PTR) \
	(((40543*(((*(PTR))<<8)^((*((PTR)+1))<<4)^(*((PTR)+2))))>>4) & 0xFFF)


//chunk size
#define C_SIZE 8192

#define TOPWORD 0xFFFF0000

#define MIN(x, y) (((x)<(y))? (x):(y))


typedef struct _group_info { 
	UBYTE literal_size;
	UWORD copy_size;
	UWORD score;
} GROUP_INFO; 

typedef struct _group {
	UBYTE literal_size;
	UWORD copy_size;
	UWORD comp;
	UBYTE item_size[ITEMS_PER_GROUP];
} GROUP;

void init_hashTable(UBYTE** hashTable);
void lzrw3_gen(UBYTE compressibility, UWORD size, UBYTE* output, UBYTE** hashTable);
