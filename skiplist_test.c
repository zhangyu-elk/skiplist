
#include "skiplist.h"

int main() { 
    srand((unsigned)time(0)); 
	int count = 20, i; 
	
	printf("=== Init Skip List ===\n"); 
	skiplist * sl = skiplist_new(); 
	for ( i = 0; i < count; i++)
	{ 
		sl_insert(sl,i); 
	} 
	
	printf("=== Print Skip List ===\n"); 
	sl_print(sl); 
    
	printf("=== Search Skip List ===\n"); 
	for (i = 0; i < count; i++)
	{ 
		int value = rand()%(count+10); 
		printf("find [%d]: %s\n", value, sl_contain(sl, value)?"Contain":"NOT contain");
	}
	
	printf("=== Delete Skip List ===\n"); 
	for (i = 0; i < count+10; i+=2)
	{ 
		printf("Delete[%d]: %s\n", i, sl_remove(sl, i)?"SUCCESS":"NOT FOUND"); 
	} 
	sl_print(sl); 
    
	skiplist_delete(sl); 
	sl = NULL; 
	return  0;
 } 