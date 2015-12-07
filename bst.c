#include <stdio.h>
#include <stdlib.h>

#define UBYTE unsigned char
#define UWORD unsigned int

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
		newnode[i] = (node *)malloc(sizeof(node));
		newnode[i]->ptr = newitem;
		newnode[i]->size = size;
		newnode[i]->next = NULL;
	}

	lp->tail[size - 1]->next = newnode[0];
	newnode[0]->prev = lp->tail[size - 1];
	lp->tail[size - 1] = newnode[0];
	(lp->cnt[size - 1])++;

	lp->tail[0]->next = newnode[1];
	newnode[1]->prev = lp->tail[0];
	lp->tail[0] = newnode[1];
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
	if(curr == lp->tail[0])  lp->tail[0] = pcurr; 
	pcurr->next = ncurr;
	if(ncurr!=NULL) ncurr->prev = pcurr;
	(lp->cnt[0])--;
	
	size = curr->size;
	curr = lp->head[size - 1]->next;

	for(index = 1; index < lp->cnt[size - 1]; index++) {
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

void list_print(list* lp) {
	node* curr = lp->head[17]->next;
	while(curr != NULL) {
		printf("[%p] ", curr->ptr);
		curr = curr->next;
	}
	printf("\n");
	printf("head = %p, tail = %p\n", lp->head[17]->ptr, lp->tail[17]->ptr);
}

int main(void) {
    list l;
    init_list(&l);
    char cmd[2];
    int u;
    while(1) {
        scanf("%s", cmd);
        if(cmd[0]=='i') {
            list_insert(&l, (UBYTE *) (atoi(cmd+1)), 18);
            list_print(&l);
        } else if(cmd[0]=='r') {
            list_remove(&l, (UBYTE *) (atoi(cmd+1)));
            list_print(&l);
        } else if(cmd[0]=='p') {
            list_print(&l);
        }
    }
}