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
#define DEBUG
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
	if (!name) {
		printf("empty name in getSymbol(name)\n");
		return NULL;
	}
	assert(name);
	unsigned int idx = hashPjw(name);
	hashEntry_t* entry = symbolTable[idx].next;
	while (entry) {
		if (!strcmp(name, entry->data->name))
			return entry->data;
		entry = entry->next;
	}
#ifdef DEBUG
	printf("get no symbol from sym_table by %s, return NULL\n", name);
#endif
	return NULL;
}

void parseSymbol(symbol_t* symb) {
	if (symb->kind == var) {
		if (symb->type.var->type->kind == BASIC)
			printf("               type: basic\n");
		else if (symb->type.var->type->kind == ARRAY) {
			vtype_t* type = symb->type.var->type;
			printf("               type: array\n");
			/*
			while (type) {
				printf("               size: %d\n", type->size);
				printf("               kind: %d\n", type->kind);
			type = type->elem;
			}
			*/
		}
		else if (symb->type.var->type->kind == STRUCTURE) {
			printf("               type: structure\n");
		}
	}
	if (symb->kind == function) {
		printf("               type: function\n");
		vtype_t* type = symb->type.func->returnType;
		if (type->kind == BASIC)
			printf("               return type: basic\n");
		typeList_t* list = symb->type.func->paramList;
		while (list) {
			printf("                  param: \n");
			if (list->type->kind == STRUCTURE)
				printf("structure\n");
			if (list->type->kind == ARRAY)
				printf("array\n");
			else 
				printf("basic\n");
			list = list->next;
		}
	}
}
void insertSymbol(symbol_t* symb) {
	assert(symb);

	unsigned int idx = hashPjw(symb->name);
	hashEntry_t* newEntry = (hashEntry_t*)malloc(sizeof(hashEntry_t));

	symb->depth = scopeDepth; // assign when insert
	newEntry->data = symb;
	newEntry->next = symbolTable[idx].next;
	newEntry->prev = &symbolTable[idx];
	if (symbolTable[idx].next) 
		symbolTable[idx].next->prev = newEntry;
	symbolTable[idx].next = newEntry;
/*
	if (symb->kind == function) {
		symb->depth = 0; // assign when insert
		newEntry->scope = scopeStack[0].scope;
		scopeStack[0].scope = newEntry;
	}
	else {
		newEntry->scope = scopeStack[scopeDepth].scope;
		scopeStack[scopeDepth].scope = newEntry;
	}
	*/
	newEntry->scope = scopeStack[scopeDepth].scope;
	scopeStack[scopeDepth].scope = newEntry;
	printf("SYMBOL_TABLE::: insert new entry in symbol talbe, name: %s, depth: %d\n", symb->name, symb->depth);
	parseSymbol(symb);
}

void leaveScope() {
#ifdef DEBUG
	printf("*************enter leaveScope******************\n");
#endif
	hashEntry_t *entry = scopeStack[scopeDepth].scope;
	hashEntry_t *current;
	typeList_t *temp;
	
	/*
	while (entry) {
		entry->prev->next = entry->next;
		if (entry->next)
			entry->next->prev = entry->prev;
		current = entry;
		if (function == current->data->kind) {
			printf("delete func entry, %s\n", current->data->name);
			typeList_t* types = current->data->type.func->paramList;
			while (types) {
				temp = types;
				types = types->next;
				printf("in func entry, param kind %d\n", temp->type->kind);


				
				 // pay attention to nested heap-allocated memory 
				 // to be continued
				//free(temp->type);
				free(temp);
			}
		}
		entry = entry->scope;
		free(current->data);
		free(current);
	}
	*/
	scopeDepth--;
#ifdef DEBUG
	printf("*************leaveScope******************\n");
#endif
}
void enterScope() {
#ifdef DEBUG
	printf("*************enter scope******************\n");
#endif
	scopeDepth++;
	memset(&scopeStack[scopeDepth], 0, sizeof(hashEntry_t));
}

