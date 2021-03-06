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

typedef struct _node {
	struct _node *next;
	struct _node *prev;
	UBYTE *ptr;
} node;

typedef struct _list {
	int cnt;
	node* head;
	node* tail;
} list;

int compare(void *f, void *s)
{
	UWORD f_score, s_score, f_cs, s_cs;
	f_score = ((GROUP_INFO *)f)->score;
	s_score = ((GROUP_INFO *)s)->score;
	f_cs = ((GROUP_INFO *)f)->literal_size;
	s_cs = ((GROUP_INFO *)s)->literal_size;

	if(f_score > s_score) {
		return 1;
	} else if(f_score == s_score) {
		if(f_cs < s_cs) {
			return 1;
		} else if(f_cs == s_cs) {
			return 0;
		}  else {
			return -1;
		}
	} else {
		return -1;
	}
}

void init_list(list* lp) {
	node* dummy = (node *)malloc(sizeof(node));
	dummy->ptr = NULL;
	dummy->next = NULL;
	dummy->prev = NULL;
	
	lp->cnt = 1;
	lp->head = dummy;
	lp->tail = dummy;
}

void list_insert(list* lp, UBYTE* newitem) {
	
	node* newnode = (node *)malloc(sizeof(node));

	node* curr = lp->head->next;
	
	newnode->ptr = newitem;
	newnode->next = NULL;

	lp->tail->next = newnode;
	newnode->prev = lp->tail;
	lp->tail = newnode;
	(lp->cnt)++;
	
}

void list_remove(list* lp, UBYTE* item) {
	int n = lp->cnt;
	int index;
	node* curr = lp->head->next;
	node* pcurr;
	node* ncurr;
	
	for(index = 1; index < n; index++) {
		if(curr->ptr == item) {
			break;
		} else {
			curr = curr->next;
		}
	}
	
	if(index==n) {
		return;
	}

	pcurr = curr->prev;
	ncurr = curr->next;
	
	if(curr==lp->tail) {
		lp->tail = pcurr;
	}
	
	pcurr->next = ncurr;
	if(ncurr!= NULL) ncurr->prev = pcurr;

	free(curr);
	(lp->cnt)--;
}

UBYTE* list_get_item(list* lp, UBYTE* min) {
	node* curr = lp->head->next;
	if(curr==NULL) {
		printf("list empty!\n");
		fflush(stdout);
		return NULL;
	}

	for(; curr->ptr <= min; curr = curr->next) {
		if(curr==NULL) {
			printf("ansldflaksdfj\n");
			return NULL;
		}
	}

	return curr->ptr;
}

void list_copy(list* src, list* dst) {
	int cnt = src->cnt;
	int i;
	node* curr = src->head;

	node* ptmp = NULL;

	dst->cnt = cnt;
	for(i = 0; i < cnt; i++) {
		node* tmp = (node*) malloc(sizeof(node));
		tmp->ptr = curr->ptr;
		tmp->next = NULL;
		tmp->prev = ptmp;
		if(ptmp != NULL) {
			ptmp->next = tmp;
		}
	
		if(i==0) {
			dst->head = tmp;
		} else if(i==cnt - 1) {
			dst->tail = tmp;
		}

		ptmp = tmp;
		curr = curr->next;
	}
}


void list_print(list* lp) {
	node* curr = lp->head->next;
	while(curr != NULL) {
		printf("%p ->", curr->ptr);
		curr = curr->next;
	}
	printf("\n");
}

extern UBYTE* hashTable[HASH_TABLE_LENGTH];
void init_hashTable(UBYTE** hashTable);
void lzrw3_gen(UWORD comp_ratio, UWORD size, UBYTE* output, UBYTE** hashTable);
//void lzrw3_gen(UWORD comp_ratio, UWORD size, UBYTE* output, UBYTE** hashTable, int p);
