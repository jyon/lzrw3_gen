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
	return (UWORD) DEST - (UWORD) output + 1;

	overrun:
	/*printk("ERROR:overruned\n");*/
		return -1;
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

	UBYTE* pcopy_ptr = NULL;
	UBYTE  pcopy_size = 0;

	list copy_list;
	
	UWORD iter = 0;

	GROUP group;

	srand(time(NULL));
	init_hashTable(hashTable);
	init_list(&copy_list);
	
//	printf("DEST = %p\n", DEST);
	
	while(DEST != DEST_POST)
	{
//		printf("----------------------------------------------------------\n");
		int i = 0;
		int j = 0;
		UBYTE* p_scan;
		UBYTE* p_lookup;
		UWORD index;
		UBYTE** p_hash;
		UBYTE literal_size;
		UBYTE* copy_ptr = NULL;
		
		
		iter++;	
		comp_rest += 2;

		if(DEST == output) {
			group = mk_group(write_rest, comp_rest, 16);
		} else if (DEST <  DEST_MAX1) { 
			group = mk_group(write_rest, comp_rest, -1);
		} else {
			return;
		}

		//printf("iter%d. literal_size=%d\n", iter, group.literal_size);
		
//		printf("iter%04d: group_size = %5d. %5d(literal). %5d(copy). comp = %5d. group.compressibility = %3.5f. write_rest = %5d. comp_rest = %5d. compressibility = %3.5f WRITTEN=%d\n",
//				iter, group.copy_size + group.literal_size, group.literal_size, group.copy_size, group.comp, (double) (group.comp - 2) / (group.literal_size + group.copy_size), write_rest, comp_rest, (double) comp_rest / write_rest, 
//				(unsigned int) DEST - (unsigned int) output); 
		

		//literal generate

		#define MATCH1(x) (p_lookup[(x)] == p_scan[0] && p_lookup[(x)+1] == p_scan[1] && p_lookup[(x)+2] == p_scan[2])
		#define MATCH2(x) (copy_ptr != NULL && p_lookup[(x)] == copy_ptr[0] && p_lookup[(x)+1] == copy_ptr[1] && p_lookup[(x)+2] == copy_ptr[2])
		#define MATCH3(x) 0
	//	#define MATCH3(x) (pcopy_ptr != NULL && p_lookup[(x)] == pcopy_ptr[0] && p_lookup[(x)+1] == pcopy_ptr[1] && p_lookup[(x)+2] == pcopy_ptr[2])
	

//		printf("\t  | literal write: ");
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

			if(MATCH1(i) || copy_ptr != NULL && index == HASH(copy_ptr)) {
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
				//printf("l");
				//printp(*l_buf2, 'r');
				list_remove(&copy_list, *l_buf2);
				*l_buf2 = DEST - 2;
				//printp(*l_buf2, 'i');

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
//		printf("write_rest = %5d. written = %5d. item_num = %d. \n", write_rest, (UWORD) DEST - (UWORD) output, item_num(p_copy));

		if(DEST > DEST_MAX1) {
			return;
		}
//		printf("literal_gen finish\nlist: ");
//		list_print(&copy_list);
//		printf("\t  | copy write: ");

		//copy generate
		for(i = group.literal_size; i < ITEMS_PER_GROUP; i++)
		{
//			printf("copy_ptr=%p\n", copy_ptr);
			p_lookup = DEST;

			if(i > group.literal_size || copy_ptr == NULL) {
				copy_ptr = list_get_item(&copy_list, pcopy_ptr);
				while(pcopy_size < 17 && *(pcopy_ptr + pcopy_size) == *copy_ptr) {
					copy_ptr = list_get_item(&copy_list, copy_ptr);
				}
				pcopy_ptr = copy_ptr;
				pcopy_size = group.item_size[i];
			} else if (i == group.literal_size && pcopy_ptr != NULL) {
				pcopy_ptr = copy_ptr;
				pcopy_size = group.item_size[i];
			}
				

			for(j = 0; j < group.item_size[i]; j++) {
				*(DEST++) = copy_ptr[j];
			}

			index = HASH(p_lookup);
			p_hash = &hashTable[index];

			if(l_buf1 != 0) {
				if(l_buf2 != 0) {
					//printf("c");	
					
					//printp(*l_buf2, 'r');
					list_remove(&copy_list, *l_buf2);
					*l_buf2 = p_lookup - 2;
					//printp(*l_buf2, 'i');
					list_insert(&copy_list, *l_buf2);

					l_buf2 = 0;
				} 
				
				//printf("c");	
				//printp(*l_buf1, 'r');
				list_remove(&copy_list, *l_buf1);
				*l_buf1 = p_lookup - 1;
				//printp(*l_buf1, 'i');
				list_insert(&copy_list, *l_buf1);

				l_buf1 = 0;
			}

			//printf("h");
			//printp(*p_hash, 'r');
			list_remove(&copy_list, *p_hash);

			*p_hash = p_lookup;
			//printp(*p_hash, 'i');
			list_insert(&copy_list, *p_hash);
		}

		write_rest -= group.copy_size;
		comp_rest -= group.comp;

//		printf("write_rest = %5d. written = %5d. item_num = %d. \n", write_rest, (UWORD) DEST - (UWORD) output, item_num(p_copy));
//		printf("----------------------------------------------------------\n\n");	
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
	UBYTE b[200000];
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

	for(i = 0; i < 4000; i++) {
		for(j = 0; j < 25; j++) {
			printf("%3d ", a[25 * i + j]);
		}
		printf("\n");
	}
	printf("\n");
	int x;
	x = ((size / 25) + 1);
	for(i = 0; i < x; i++) {
		for(j = 0; j < 25; j++) {
			printf("%3d ", b[25 * i + j]);
		}
		printf("\n");
	}
	
	printf("%d / %d = %f\n", 100000 - 100000* (100 - comp) / 100, 100000, (double) (100000 - 100000 * (100 - comp) / 100) / 100000);
	printf("%d / %d = %f\n", (100000 - size), 100000, (double) (100000 - size) / 100000);
}
