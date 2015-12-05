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
	UBYTE size;
	UBYTE *ptr;
} node;

typedef struct _list {
	int cnt[18];
	node* head[18];
	node* tail[18];
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

int is_empty(list* lp) {
	return (lp->cnt == 1);
}

void init_list(list* lp) {
	int i;
	for(i = 0; i < 18; i++) {
		node* dummy = (node *)malloc(sizeof(node));
		dummy->ptr = NULL;
		dummy->size = 0;
		dummy->next = NULL;
		dummy->prev = NULL;
		lp->cnt[i] = 1;
		lp->head[i] = dummy;
		lp->tail[i] = dummy;
	}
}

void list_insert(list* lp, UBYTE* newitem, UBYTE size) {
	int i;
	node* newnode[2];

	for(i = 0; i < 2; i++) {
		newnode[i]->ptr = newitem;
		newnode[i]->size = size;
		newnode[i]->next = NULL;
	}

	lp->tail[size - 1]->next = newnode[0];
	newnode[0]->prev = lp->tail[size - 1];
	lp->tail[size - 1] = newnode[0];
	(lp->cnt[size - 1])++;

	lp->tail[0]->next = newnode[0];
	newnode[0]->prev = lp->tail[0];
	lp->tail[0] = newnode[0];
	(lp->cnt[0])++;
}

void list_remove(list* lp, UBYTE* item) {
	int size;
	int index;
	node* curr = lp->head[0]->next;
	node* pcurr;
	node* ncurr;
	
	for(index = 1; index < lp->cnt[0]; index++) {
		if(curr->ptr == item) break; 
		curr = curr->next;
	}
		
	if(index == lp->cnt[0]) return; 
	pcurr = curr->prev; ncurr = curr->next;
	if(curr == lp->tail[0])  lp->tail[0] = curr-> pcurr; 
	pcurr->next = ncurr;
	if(ncurr!=NULL) ncurr->prev = pcurr;
	(lp->cnt[0])--;
	
	size = curr->size;
	curr = lp->head[size - 1]->next;

	for(index = 1; index < n; index++) {
		if(curr->ptr == item)  break;
		curr = curr->next;
	}
	
	if(index==lp->cnt[size - 1]) return; 
	pcurr = curr->prev; ncurr = curr->next;
	if(curr==lp->tail[size - 1]) lp->tail[size - 1] = pcurr; 
	pcurr->next = ncurr;
	if(ncurr!= NULL) ncurr->prev = pcurr;
	(lp->cnt[size - 1])--;
}

UBYTE* list_get_item(list* lp, UBYTE* prev, UBYTE size) {
	node* curr = lp->head[size - 1]->next;
	for(; curr->ptr < prev; curr = curr->next);
	return curr->ptr;
}

int copy_num(list* lp, UBYTE size) {
	int i, r;
	for(i = size - 1; i < 18; i++) 
		r += lp->cnt[i];
	return r;
}

int can_copy(list* lp, GROUP *g) {
	if(copy_num(lp, g->item_size[g->literal_size]) >= ITEMS_PER_GROUP - g->literal_size) return 1;
	return 0;
}

int calc_copy_rest(list* lp, GROUP *g) {
	int copy_rest, i;
	copy_rest = g->item_size[g->literal_size];
	for(i = g->literal_size; i < ITEMS_PER_GROUP; i++) {
		if(g->item_size[i] > g->item_size[g->literal_size]) {
			copy_rest += 1;
			break;
		}
	}
	copy_rest += ITEMS_PER_GROUP - g->literal_size - 1;
	copy_rest = ((copy_rest / 16) + 1) * 16;
	return copy_rest;
}

void list_print(list* lp) {
	node* curr = lp->head->next;
	while(curr != NULL) {
		printf("[%p: %u] ", curr->ptr, *(curr->ptr));
		curr = curr->next;
	}
	printf("\n");
	printf("head = %p, tail = %p\n", lp->head->ptr, lp->tail->ptr);
}

void init_hashTable(UBYTE** hashTable);
void lzrw3_gen(UBYTE compressibility, UWORD size, UBYTE* output, UBYTE** hashTable);
