/*************************************************************************
    > File Name: symbol.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Fri 02 May 2014 08:08:34 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "symbol.h"
//#define DEBUG
/*
 * global variable 
 */
int scopeDepth;
hashEntry_t scopeStack[MAX_SCOPE];
hashEntry_t symbolTable[NR_SYMBOL_TABLE_ENTRY];


unsigned int hashPjw(char *name) {
	unsigned int val = 0, i;
	for (; *name; name++) {
		val = (val << 2) + *name;
		if (i = val &~0x3fff)
			val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val;
}

/*
 * initialize hash table 
 */
void initSymbolTable() {
	scopeDepth = 0;
	memset(symbolTable, 0, sizeof(symbolTable));
}

symbol_t* getSymbol(char *name) {
	if (!name)
		return NULL;
	assert(name);
	unsigned int idx = hashPjw(name);
	hashEntry_t* entry = symbolTable[idx].next;
	while (entry) {
		if (!strcmp(name, entry->data->name))
			return entry->data;
		entry = entry->next;
	}
	return NULL;
}

void insertSymbol(symbol_t* symb) {
	assert(symb);
	unsigned int idx = hashPjw(symb->name);
	hashEntry_t* newEntry = (hashEntry_t*)malloc(sizeof(hashEntry_t));
	symb->depth = scopeDepth; // assign when insert
	newEntry->data = symb;
	newEntry->next = symbolTable[idx].next;
	symbolTable[idx].next = newEntry;

	newEntry->scope = scopeStack[scopeDepth].scope;
	scopeStack[scopeDepth].scope = newEntry;
}

void leaveScope() {
#ifdef DEBUG
	printf("*************leave ");
#endif
	hashEntry_t *entry = scopeStack[scopeDepth].scope;
	hashEntry_t *current;
	typeList_t *temp;
	
	while (entry) {
		current = entry;
		entry = entry->scope;
		if (function == current->data->kind) {
			typeList_t* types = current->data->type.func->paramList;
			while (types) {
				temp = types;
				types = types->next;


				/*
				 * pay attention to nested heap-allocated memory 
				 */
				/* 
				 * to be continued
				 */
				free(temp->type);
				free(temp);
			}
		/*
		 * delete node from linked list in O(1) time 
		 */
			if (entry) {
				free(current->data);
				memcpy(current, current->next, sizeof(hashEntry_t));
				free(current->next);
			}
		}
	}
#ifdef DEBUG
	printf("leaveScope******************\n");
#endif
}
void enterScope() {
#ifdef DEBUG
	printf("*************enter scope******************\n");
#endif
	scopeDepth++;
	memset(&scopeStack[scopeDepth], 0, sizeof(hashEntry_t));
}

