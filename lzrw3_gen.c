#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "lzrw3_gen.h"
#include "blueftl_lzrw3.h"

UBYTE* hashTable[HASH_TABLE_LENGTH];
UWORD blueftl_lzrw3_compress(UBYTE* input, UWORD input_size, UBYTE* output, UBYTE** hashTable)
{
	UWORD return_val = 0;
	//initialize hash Table
	init_hashTable(hashTable);
	/*	SCAN: scanning point
		DEST: the position to write incoded data */
	UBYTE* SCAN = input;
	UBYTE* DEST = output;
	//	last positions of stream 
	UBYTE* SCAN_POST = SCAN + input_size;
	UBYTE* DEST_POST = DEST + C_SIZE;
	UBYTE* SCAN_MAX1 = SCAN + input_size - MAX_RAW_ITEM;
	UBYTE* SCAN_MAX16 = SCAN + input_size - MAX_RAW_ITEM * 16;
	// control word
	UBYTE* p_control;
	UWORD control = TOPWORD;
	//literal buffers
	UBYTE** l_buf1 = 0;		//point the hash table entry to the youngest literal
	UBYTE** l_buf2 = 0;		//point the hash table entry to the 2nd youngest literal
	
	//perf_inc_compr ();

	//write control bits of first group
	p_control = DEST;
	DEST += 2;
	
	while(1)
	{
		UBYTE *p_scan;
		UBYTE *p_lookup;
		UWORD unroll;
		UWORD index;
		UBYTE **p_hash;

		unroll = 16;
		if(SCAN > SCAN_MAX16)
		{
			unroll = 1;
			if(SCAN > SCAN_MAX1)
			{
				if(SCAN == SCAN_POST)
					break;
				else
					goto letteral;
			}
		}
	
		begin_unrolled_loop:
		
		index = HASH(SCAN);
		p_hash = &hashTable[index];
		p_scan = *p_hash;


		#define PS *(p_scan++) != *(SCAN++)
		p_lookup = SCAN;
		if(PS || PS || PS)
		{
			//letteral
			SCAN = p_lookup;

			letteral:

			*(DEST++) = *(SCAN++);
			control &= 0xFFFEFFFF;

			if(l_buf2 != 0)
				*l_buf2 = p_lookup - 2;
			
			l_buf2 = l_buf1;
			l_buf1 = p_hash;

		}
		else
		{
			//copy
			PS || PS || PS || PS || PS || PS || PS || PS ||
			PS || PS || PS || PS || PS || PS || PS || SCAN++;
			*(DEST++) = ((index & 0xF00) >> 4) | (--SCAN - p_lookup - 3);
			*(DEST++) = index & 0xFF;

			if(l_buf1 != 0)
			{
				if(l_buf2 != 0)
				{
					*l_buf2 = p_lookup -2;
					l_buf2 = 0;
				}
				*l_buf1 = p_lookup -1;
				l_buf1 = 0;
			}

			*p_hash = p_lookup;
		}
		control >>= 1;

		end_unrolled_loop:
		
		if(--unroll)
			goto begin_unrolled_loop;

		if((control & TOPWORD) == 0)
		{
			*(p_control++) = control & 0xFF;
			*p_control = (control >> 8) &0xFF;

			p_control = DEST;
			DEST += 2;

			control = TOPWORD;
		}
	}
	while(control & TOPWORD)
		control >>= 1;
	*(p_control++) = control & 0xFF;
	*(p_control++) = (control >> 8) & 0xFF;

	if(p_control == DEST)
		DEST -= 2;
	return (UWORD) DEST - (UWORD) output;

	overrun:
	/*printk("ERROR:overruned\n");*/
		return -1;
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

GROUP mk_group(UWORD wr, UWORD cr, int literal_min)
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
	
	if(i == l) {
		group.literal_size = 16;
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

UBYTE *get_item(UBYTE *array[], UBYTE *max)
{
	int i;
	for(i = HASH_TABLE_LENGTH - 1; i >= 0; i--)
	{
		if(array[i] != NULL && array[i] < max) {
			return array[i];
		}
	}
	return NULL;
}

void insert(UBYTE *array[], UBYTE *newitem)
{
	int i;
	for(i = 0; i < HASH_TABLE_LENGTH; i++)
	{
		if(array[i] == NULL) {
			array[i] = newitem;
		} else if(array[i] == newitem) {
			return;
		} 
	}
}

int item_num(UBYTE *array[])
{
	int i;
	for(i = 0; i < HASH_TABLE_LENGTH; i++) {
		if (array[i] == NULL)
			return i;
	}
}


void lzrw3_gen(UBYTE compressibility, UWORD size, UBYTE* output, UBYTE** hashTable) 
{
	UWORD comp_size = size * (100 - compressibility) / 100;
	UWORD comp_rest = size - comp_size;
	UWORD write_rest = size;
	
	UBYTE* DEST = output;
	UBYTE* DEST_POST = output + size;
	UBYTE* DEST_MAX1 = DEST_POST - MAX_RAW_ITEM;
	
	UBYTE** l_buf1 = 0;
	UBYTE** l_buf2 = 0;
	UBYTE* p_copy[HASH_TABLE_LENGTH] = {NULL, };
	UWORD p_copy_index = 0;
	UWORD iter = 0;

	GROUP group;

	srand(time(NULL));
	init_hashTable(hashTable);

	while(DEST != DEST_POST)
	{
		int i = 0;
		int j = 0;
		UBYTE* p_scan;
		UBYTE* p_lookup;
		UWORD index;
		UBYTE** p_hash;
		UBYTE literal_size;
		UBYTE* copy_ptr;
		
		iter++;	
		comp_rest += 2;

		if(DEST == output) {
			group = mk_group(write_rest, comp_rest, 16);
		} else if (DEST <  DEST_MAX1) { 
			group = mk_group(write_rest, comp_rest, -1);
		} else {
			return;
		}

		
//		printf("iter%04d: group_size = %5d. %5d(literal). %5d(copy). comp = %5d. group.compressibility = %3.5f. write_rest = %5d. comp_rest = %5d. compressibility = %3.5f WRITTEN=%d\n",
//				iter, group.copy_size + group.literal_size, group.literal_size, group.copy_size, group.comp, (double) (group.comp - 2) / (group.literal_size + group.copy_size), write_rest, comp_rest, (double) comp_rest / write_rest, 
//				(unsigned int) DEST - (unsigned int) output); 
		

		//literal generate

		#define MATCH(x) (p_lookup[(x)] == p_scan[(x)] && p_lookup[(x)+1] == p_scan[(x)+1] && p_lookup[(x)+2] == p_scan[(x)+2])
	

//		printf("\t  | literal write: ");
		literal_size = group.literal_size;

		if (group.literal_size < ITEMS_PER_GROUP) {
			for(j = HASH_TABLE_LENGTH - 1; j >= 0; j--)
			{
				if(p_copy[j] != NULL && p_copy[j] < DEST - group.item_size[literal_size]) {
					copy_ptr = p_copy[j];
					break;
				}
			}
			DEST[literal_size] = copy_ptr[0];	
			DEST[literal_size + 1] = copy_ptr[1];
		}
	
		p_lookup = DEST;
		for(i = 0; i < literal_size; i++) {

			literal_gen:
			*(DEST++) = (rand() % 256);

		}
	
		DEST = (iter==1)? p_lookup:p_lookup - 2;	

		for(i = (iter==1)? 0:-2; i < 0; i++) {
			index = HASH(DEST);
			p_hash = &hashTable[index];
			p_scan = *p_hash;

			if(MATCH(i)) {
				DEST = (DEST < p_lookup)? p_lookup:p_lookup + i; 
				goto literal_gen; 
			}
			DEST++;
		}

		for(i = 0; i < literal_size; i++) {
			index = HASH(DEST);
			p_hash = &hashTable[index];
			p_scan = *p_hash;

			if(MATCH(i)) {
				DEST = (DEST < p_lookup)? p_lookup:p_lookup + i; 
				goto literal_gen; 
			}

			if(l_buf2 != 0) {
				*l_buf2 = DEST - 2;
				for(j = 0; j < HASH_TABLE_LENGTH; j++)
				{
					if(p_copy[j] == NULL) {
						p_copy[j] = *l_buf2;
					} else if(p_copy[j] == *l_buf2) {
						break;
					} 
				}
			}
		
			DEST++;
			l_buf2 = l_buf1;
			l_buf1 = p_hash;
		}
		
		write_rest -= literal_size;
//		printf("write_rest = %5d. written = %5d. item_num = %d. \n", write_rest, (UWORD) DEST - (UWORD) output, item_num(p_copy));

		if(DEST > DEST_MAX1) {
			return;
		}
		

//		printf("\t  | copy write: ");

		//copy generate
		for(i = group.literal_size; i < ITEMS_PER_GROUP; i++)
		{
			p_lookup = DEST;
			for(j = HASH_TABLE_LENGTH - 1; j >= 0; j--)
			{
				if(p_copy[j] != NULL && p_copy[j] < DEST - group.item_size[i]) {
					copy_ptr = p_copy[j];
					break;
				}
			}

			for(j = 0; j < group.item_size[i]; j++) {
				*(DEST++) = copy_ptr[j];
			}

			if(l_buf1 != 0) {
				*l_buf1 = p_lookup - 1;
				for(j = 0; j < HASH_TABLE_LENGTH; j++)
				{
					if(p_copy[j] == NULL) {
						p_copy[j] = *l_buf1;
					} else if(p_copy[j] == *l_buf1) {
						break;
					} 
				}
				l_buf1 = 0;

				if(l_buf2 != 0) {
					*l_buf2 = p_lookup - 2;
					for(j = 0; j < HASH_TABLE_LENGTH; j++)
					{
						if(p_copy[j] == NULL) {
							p_copy[j] = *l_buf2;
						} else if(p_copy[j] == *l_buf2) {
							break;
						} 
					}
					l_buf2 = 0;
				}
			}

			*p_hash = p_lookup;
		}

		write_rest -= group.copy_size;
		comp_rest -= group.comp;

		for(j = 0; j < HASH_TABLE_LENGTH; j++) {
			if(p_copy[j] != NULL) {
				printf("[%d]: %d (%p)\n", j, *(p_copy[j]), p_copy[j]);
			} else {
				break;
			}
		}
		

//		printf("write_rest = %5d. written = %5d. item_num = %d. \n", write_rest, (UWORD) DEST - (UWORD) output, item_num(p_copy));
	}
}

void init_hashTable(UBYTE** hashTable)
{
	#define ZH *hashTable++ = START_STRING
	UWORD i;
	for(i = 0; i < 256; i++)
	{
		ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;ZH;
	}
}

void main(int argc, char *argv[])
{
	UBYTE a[100000];
	UBYTE b[100000];
	UWORD size;
	int comp;
	int i = 0, j;
	for(i = 0; i < 100000; i++) {
		a[i] = 0;
		b[i] = 0; 
	}

	comp = atoi(argv[1]);
	lzrw3_gen(comp, 100000, a, hashTable);
	size = blueftl_lzrw3_compress(a, 100000, b, hashTable);


	for(i = 0; i < 200; i++) {
		for(j = 0; j < 50; j++) {
			printf("%3d ", a[50 * i + j]);
		}
		printf("\n");
	}
	printf("%d / %d = %f\n", 100000 - 100000* (100 - comp) / 100, 100000, (double) (100000 - 100000 * (100 - comp) / 100) / 100000);
	printf("%d / %d = %f\n", (100000 - size), 100000, (double) (100000 - size) / 100000);
}

