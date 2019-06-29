/*使用C语言实现的跳表, 通过空间的消耗, 换取时间的效率, 对链表时间近乎于二分查找*/
#ifndef __SKIPLIST_H 
#define __SKIPLIST_H 
 
#include <stdlib.h> 
#include <stdio.h> 
 
#define _S_MAX_LEVEL 32 
/*采用固定LEVEL的方式,2^32次方已经达到4,294,967,296的数量,非常大*/   

typedef struct skipnode skipnode;
typedef struct skiplist skiplist;

struct skipnode
{
	int				val;
	skipnode		*backward;			//前面的一个指针,链表的指针
	skipnode		**forward;		//该节点在多层分别指向的指针
};

struct skiplist
{
        int			level;
		int			length;
		skipnode	*root;
}; 

skipnode* skipnode_new(int level, int val);
skiplist *skiplist_new();
int sl_random_level(void);
skipnode* sl_find(skiplist *sl, int val);
int	 sl_contain(skiplist *sl, int val);
skipnode* sl_insert(skiplist *sl, int val);
int  sl_remove(skiplist *sl, int val);
void sl_print(skiplist *sl);

skipnode* skipnode_new(int level, int val)
{
	skipnode* sn = (skipnode*)malloc(sizeof(skipnode));
	sn->forward = (skipnode**)calloc(level * sizeof(void*), 1);
	sn->val = val;

	return sn;	
}

skiplist *skiplist_new()
{
	skiplist *sl 	= (skiplist*)malloc(sizeof(skiplist));
	sl->root 		= skipnode_new(_S_MAX_LEVEL, 0);
	sl->level 		= 1;
	sl->length 		= 0;
	
	srand(time(NULL));
	
	return sl;
}

void skiplist_delete(skiplist *sl)
{
	if(!sl)	return ;
	
	skipnode* ptr = sl->root->forward[0];
	skipnode* tmp = ptr->forward[0];
	
	free(sl->root);
	while(ptr)
	{
		tmp = ptr->forward[0];
		free(ptr);
		ptr = tmp;
	}
	free(sl);
}

/*
**	0.25*0xFFFF = 0x3FFF  3->0011
**	rand()最大值0x7FFF	  7->0111
**	(rand()&0xFFFF) < (0.25*0xFFFF) 意思就是说,当rand最高位为1条件为false,反之为true,即1/2的概率
*/

int sl_random_level(void)
{ 
        int level = 0; 
        while((rand()&0xFFFF) < (0.25*0xFFFF))    
        {
			level++;
		}
		
		return level;
		//正态分布, 比如第一层1/2,第二层1/4,第三层1/8
} 

skipnode* sl_find(skiplist *sl, int val)
{
	if(!sl || !sl->length)
	{
		return NULL;
	}

	skipnode *ptr = sl->root;
	int i;
	for(i = sl->level - 1; i >= 0; i--)
	{
		while(ptr->forward[i] && ptr->forward[i]->val < val)
		{
			ptr = ptr->forward[i];
		}
	}
	
	if(ptr->forward[0] && ptr->forward[0]->val == val)
	{
		return ptr;
	}
	else
	{
		return NULL;
	}
}

int	 sl_contain(skiplist *sl, int val)
{
	return sl_find(sl, val) ? 1 : 0;
}

	

skipnode* sl_insert(skiplist *sl, int val)
{
	if(!sl)	return NULL;
	
	skipnode* update[_S_MAX_LEVEL];
	skipnode* ptr = sl->root;
	int i;
	for(i = sl->level - 1; i >= 0; i--)
	{
		while(ptr->forward[i] && ptr->forward[i]->val < val)
		{
			ptr = ptr->forward[i];
		}
		update[i] = ptr;
	}
	
	int new_l = sl_random_level() + 1;
	/*实际层数从1开始*/
	new_l = (new_l > _S_MAX_LEVEL) ? _S_MAX_LEVEL : new_l;
	
	ptr = skipnode_new(new_l, val);
	if(new_l > sl->level)
	{
		for(i = sl->level; i < new_l; i++)
		{
			update[i] = sl->root;
		}
		sl->level = new_l;
	}
	
	for(i = 0; i < new_l; i++)
	{
		ptr->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = ptr;
	}
	
	
	ptr->backward = (update[0] == sl->root) ? NULL : update[0];	
	//root并不存储值, 
	
	if(ptr->forward[0])
	{
		ptr->forward[0]->backward = ptr;
	}
	else
	{
		
	}
	//downward存储的是前一位的节点
	sl->length++;
	return ptr;
}

/*
** @param ptr 将要被删除的节点
** @param update每一级的前驱节点
** @param sl
*/
void sl_remove_node(skiplist *sl, skipnode* ptr, skipnode **update)
{
	int i;
	for(i = sl->level - 1; i >= 0; i--)
	{
		if(update[i]->forward[i] == ptr)
		{
			update[i]->forward[i] = ptr->forward[i];
		}
	}
	
	if(ptr->forward[0])
	{
		ptr->forward[0]->backward = ptr->backward;
	}
	else
	{
		
	}
	while(sl->level > 1 && sl->root->forward[sl->level - 1])
	{
		sl->level--;
	}
	sl->length--;
}

int  sl_remove(skiplist *sl, int val)
{
	if(!sl)
	{
		return 0;
	}
	
	skipnode *update[_S_MAX_LEVEL];
	skipnode* ptr = sl->root;
	int i;
	
	for(i = sl->level - 1; i >= 0; i--)
	{
		while(ptr->forward[i] && ptr->forward[i]->val < val)
		{
			ptr = ptr->forward[i]; 
		}
		update[i] = ptr;
	}
	
	
	//ptr指向的是第0层,最后一个小于val的值
	if(ptr->forward[0] && ptr->forward[0]->val == val)
	{
		sl_remove_node(sl, ptr->forward[0], update);
		return 1;
	}
	else
	{
		return 0;
	}
}

void sl_print(skiplist *sl)
{
	if(!sl || !sl->root->forward[0])
	{
		printf("the skiplist is empty!\n");
		return ;
	}
	
	int i;
	skipnode *ptr;
	for(i = sl->level - 1; i >= 0; i--)
	{
		printf("level %d: ", i);
		ptr = sl->root->forward[i];
		while(ptr)
		{
			printf("%d\t", ptr->val);
			ptr = ptr->forward[i];
		}
		printf("\n");
	}
	
	return;
	
}


#endif



