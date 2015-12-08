#include <stdio.h>
#include <stdlib.h>

#define UBYTE unsigned char
#define UWORD unsigned int
#define ITEMS_PER_GROUP 16

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
void printp(UBYTE* p, char cmd) {
	if(p != NULL && p != 0)
		printf("<%c%03d%03d%03d>\n", (cmd=='i')? 'i' : 'r', *p, *(p+1), *(p+2));
}

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
	int n = lp->cnt;
	int index;
	
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

	(lp->cnt)--;
}

UBYTE* list_get_item(list* lp, UBYTE* min) {
	node* curr = lp->head->next;
	if(lp->cnt==1) {
		printf("err\n");
		return NULL;
	}
	for(; curr->ptr <= min; curr = curr->next);
	return curr->ptr;
}

void list_print(list* lp) {
	node* curr = lp->head->next;
	while(curr!=NULL) {

		printf("%d -> ", curr->ptr);
		curr = curr->next;
	}
	printf("\n");
	printf("head = %p, tail = %p\n", lp->head->ptr, lp->tail->ptr);
}


int main(void) {
    list l;
    init_list(&l);
    char *cmd;
    int u;
	UBYTE* s;
    while(1) {
        scanf("%s", cmd);
        if(cmd[0]=='i') {
            list_insert(&l, (UBYTE *) (atoi(cmd+1)));
            list_print(&l);
        } else if(cmd[0]=='r') {
            list_remove(&l, (UBYTE *) (atoi(cmd+1)));
            list_print(&l);
        } else if(cmd[0]=='p') {
            list_print(&l);
        } else if (cmd[0]=='s') {
			s = list_get_item(&l, (UBYTE *) (atoi(cmd+1)));
			printf("%p\n", s);
		}
    }
}
