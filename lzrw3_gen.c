#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "lzrw3_gen.h"

UBYTE* hashTable[HASH_TABLE_LENGTH];

GROUP mk_group(int wr, int cr, int literal_min)
{
	const UBYTE l = 2 * (ITEMS_PER_GROUP + 1);
	UBYTE i, j, k;
	GROUP_INFO group_info[l];
	GROUP group;
	
	for(i = 0; i < l; i += 2)
	{
		group_info[i].literal_size = i / 2;
		group_info[i+1].literal_size = i / 2;

		group_info[i].copy_size = MIN((group_info[i].literal_size*cr + (34 - 2*group_info[i].literal_size)*wr) / (wr - cr), wr - group_info[i].literal_size);
		group_info[i+1].copy_size = (group_info[i].copy_size == wr - group_info[i].literal_size)? INT_MAX : group_info[i].copy_size + 1;

		group_info[i].score = (group_info[i].copy_size == wr - group_info[i].literal_size)? (group_info[i].literal_size*cr + (34 - 2*group_info[i].literal_size)*wr) - (wr - cr)*wr : 
								   			 	(group_info[i].literal_size*cr + (34 - 2*group_info[i].literal_size)*wr)%  (wr - cr);
		group_info[i+1].score = (group_info[i].copy_size == wr - group_info[i].literal_size)? INT_MAX : (wr - cr) - group_info[i].score;

		if(group_info[i].copy_size > MAX_RAW_ITEM * (ITEMS_PER_GROUP - group_info[i].literal_size)) {
			group_info[i].copy_size = MAX_RAW_ITEM * (ITEMS_PER_GROUP - group_info[i].literal_size);
			group_info[i+1].copy_size = INT_MAX;
			group_info[i].score = (group_info[i].literal_size*cr + (34 - 2*group_info[i].literal_size)*wr - (wr - cr) * group_info[i].copy_size);
			group_info[i+1].score = INT_MAX;
		}
						
	}
	
	qsort(group_info, l, sizeof(GROUP_INFO), compare);	
	

	for(i = 0; i < l; i++)
	{
		if(group_info[i].copy_size <= MAX_RAW_GROUP && 
		   group_info[i].copy_size <= MAX_RAW_ITEM * (ITEMS_PER_GROUP - group_info[i].literal_size) &&
		   group_info[i].copy_size >= 3 * (ITEMS_PER_GROUP - group_info[i].literal_size))
		{
			if(literal_min > 0 && (int) group_info[i].literal_size < literal_min) {
				continue;	
			}
			break;
		}
	}
	
	if(group_info[i].literal_size == ITEMS_PER_GROUP) {
		group.literal_size = 16;
		//printf("%d %d %d\n", wr,  cr, group.literal_size);
		group.copy_size = 0;
		group.comp = 0;	
		for(i = 0; i < ITEMS_PER_GROUP; i++) {
			group.item_size[i] = 1;
		}
		
		return group;
	}
	
	group.literal_size = group_info[i].literal_size;
	group.copy_size = group_info[i].copy_size;
	group.comp = group_info[i].copy_size - 2 * (ITEMS_PER_GROUP - group_info[i].literal_size);
		
	for(j = 0; j < group_info[i].literal_size; j++) {
		group.item_size[j] = 1;
	}

	k = group_info[i].copy_size / (ITEMS_PER_GROUP - group_info[i].literal_size); 

	for(; j < ITEMS_PER_GROUP; j++) {
		group.item_size[j] =  k;
		group_info[i].copy_size -= k;
	}

	for(j = ITEMS_PER_GROUP - 1; group_info[i].copy_size > 0; j--) {
		group.item_size[j] += 1 ;
		group_info[i].copy_size -= 1;
	}
	
	return group;
}
//void lzrw3_gen(UWORD compressibility, UWORD size, UBYTE* output, UBYTE** hashTable, int p) 
void lzrw3_gen(UWORD compressibility, UWORD size, UBYTE* output, UBYTE** hashTable) 
{
	UWORD comp_size = size * compressibility / 1000;
	int comp_rest = size - comp_size;
	int write_rest = size;
	//printf("cr=%d wr=%d\n", comp_rest, write_rest);
	
	UBYTE* DEST = output;
	UBYTE* DEST_POST = output + size;
	UBYTE* DEST_MAX1 = DEST_POST - MAX_RAW_ITEM;
	
	UBYTE** l_buf1 = 0;
	UBYTE** l_buf2 = 0;

	UBYTE* pcopy_ptr = NULL;
	UBYTE  pcopy_size = 0;

	list copy_list;
	
	UWORD iter = 0;

	GROUP group;

	UBYTE* DEST_; 
	UWORD write_rest_;
	UWORD comp_rest_; 
	UBYTE* pcopy_ptr_;
	UBYTE pcopy_size_;
	UBYTE** l_buf1_; 
	UBYTE** l_buf2_; 
	list copy_list_;


//	srand(time(NULL));
	init_hashTable(hashTable);
	init_list(&copy_list);
	
	
	while(DEST != DEST_POST)
	{
		int list_empty = 0;
		
		int i = 0;
		int j = 0;

		UBYTE* p_scan;
		UBYTE* p_lookup;
		UWORD index;
		UBYTE** p_hash;
		UBYTE literal_size;
		UBYTE* copy_ptr = NULL;

		lzrw3_gen_start:
		//printf("iter: %d wr: %d cr: %d\n", iter, write_rest, comp_rest);
		//fflush(stdout);

		copy_ptr = NULL;
		DEST_ = DEST;
		write_rest_ = write_rest;
		comp_rest_ = comp_rest;
		pcopy_ptr_ = pcopy_ptr;
		pcopy_size_ = pcopy_size;
		l_buf1_ = l_buf1;
		l_buf2_ = l_buf2;
		list_copy(&copy_list, &copy_list_);

		iter++;	
		comp_rest += 2;


		if(DEST == output || list_empty) {
			group = mk_group(write_rest, comp_rest, 16);
		} else if (DEST <  DEST_MAX1) { 
		//	printf("cr=%d wr=%d\n", comp_rest, write_rest);
			group = mk_group(write_rest, comp_rest, -1);
		} else {
			return;
		}

		//printf("%d %d %d\n", write_rest, comp_rest, group.literal_size);
		

		//literal generate

		#define MATCH(x) (p_lookup[(x)] == p_scan[0] && p_lookup[(x)+1] == p_scan[1] && p_lookup[(x)+2] == p_scan[2])


		literal_size = group.literal_size;

		if (group.literal_size < ITEMS_PER_GROUP) {
			copy_ptr = list_get_item(&copy_list, pcopy_ptr);
			DEST[literal_size] = copy_ptr[0];
			DEST[literal_size + 1] = copy_ptr[1];
		}
	
		p_lookup = DEST;
		for(i = 0; i < literal_size; i++) {

			literal_gen:
			*(DEST++) = (rand() % 256);

		}
	
		if(pcopy_ptr!= NULL && (*(pcopy_ptr + pcopy_size) == *(p_lookup))) {
			DEST = p_lookup;
			i = 0;
			goto literal_gen;
		}

		DEST = p_lookup;
		for(i = 0; i < literal_size; i++) {
			index = HASH(DEST);
			p_hash = &hashTable[index];
			p_scan = *p_hash;

			if(MATCH(i) || pcopy_ptr != NULL && index == HASH(pcopy_ptr) || copy_ptr != NULL && index == HASH(copy_ptr)) {
				DEST = p_lookup + i; 
				goto literal_gen; 
			}
			DEST++;
		}


		DEST = p_lookup;
		for(i = 0; i < literal_size; i++) {
			index = HASH(DEST);
			p_hash = &hashTable[index];
			p_scan = *p_hash;
			if(l_buf2 != 0) {
				list_remove(&copy_list, *l_buf2);
				*l_buf2 = DEST - 2;

				list_insert(&copy_list, *l_buf2);
			}
			DEST++;
			l_buf2 = l_buf1;
			l_buf1 = p_hash;
		}

		if(group.literal_size < ITEMS_PER_GROUP) {
			pcopy_ptr = copy_ptr;
			pcopy_size = group.item_size[group.literal_size];
		}

		
		write_rest -= literal_size;

		copy_gen:

		if(DEST > DEST_MAX1) {
			return;
		}

		//copy generate
		for(i = group.literal_size; i < ITEMS_PER_GROUP; i++)
		{

			p_lookup = DEST;

			if(i > group.literal_size || copy_ptr == NULL) {
				copy_ptr = list_get_item(&copy_list, pcopy_ptr);

				while(pcopy_size < 18 && *(pcopy_ptr + pcopy_size) == *copy_ptr) {
					
					if(copy_ptr == copy_list.tail->ptr) {

						fflush(stdout);
						if(copy_list.cnt != 1) {
							copy_ptr = copy_list.head->ptr;
							goto copy_select;
						}

						DEST = DEST_;
						write_rest = write_rest_;
						comp_rest = comp_rest_;
						pcopy_ptr = pcopy_ptr_;
						pcopy_size = pcopy_size_;
						l_buf1 = l_buf1_;
						l_buf2 = l_buf2_;
						list_copy(&copy_list_, &copy_list);
						list_empty = 1;
						goto lzrw3_gen_start;
					}

					copy_select:
					copy_ptr = list_get_item(&copy_list, copy_ptr);

				}
				
			} /*else if (i == group.literal_size && pcopy_ptr != NULL) {

			}*/
			pcopy_ptr = copy_ptr;
			pcopy_size = group.item_size[i];
				

			for(j = 0; j < group.item_size[i]; j++) {
				*(DEST++) = copy_ptr[j];
			}

			index = HASH(p_lookup);
			p_hash = &hashTable[index];

			if(l_buf1 != 0) {
				if(l_buf2 != 0) {
					
					list_remove(&copy_list, *l_buf2);
					*l_buf2 = p_lookup - 2;
					list_insert(&copy_list, *l_buf2);

					l_buf2 = 0;
				} 
				
				list_remove(&copy_list, *l_buf1);
				*l_buf1 = p_lookup - 1;
				list_insert(&copy_list, *l_buf1);

				l_buf1 = 0;
			}

			list_remove(&copy_list, *p_hash);

			*p_hash = p_lookup;
			list_insert(&copy_list, *p_hash);
		}

		write_rest -= group.copy_size;
		comp_rest -= group.comp;

		/*
		if (p) {
			int i, j;
			printf("{");
			for(j = 0; j < group.literal_size; j++) {
				printf("%d ", *(DEST_++));
			}
			for(i = group.literal_size; i < 16; i++) {
				printf("(");
				for(j = 0; j < group.item_size[i]; j++) {
					printf("%d ", *(DEST_++));
				}
				printf(")");
			}
			printf("}");
			printf("\n");
		}
		*/

	}
}
