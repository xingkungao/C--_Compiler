/*************************************************************************
    > File Name: semantic.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Sat 03 May 2014 01:26:01 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol.h"
int equalType(vtype_t* a, vtype_t* b) {
	if (!a && !b)
		return true;
	if (!a || !b)
		return false;
	if (a->kind != b->kind)
		return false;
	if (a->kind == BASIC)
		return a->u.basic == b->u.basic; 
	if (a->kind == ARRAY)
		return a->u.array.size == b->u.array.size && equalType(a->u.array.elem, b->u.array.elem);
	if (a->kind == STRUCTURE && a->u.structure && b->u.structure) {
		fieldList_t* pa = a->u.structure;
		fieldList_t* pb = b->u.structure;
		while (pa->type && pa->type) {
			if (!equalType(pa->type, pb->type))
				return false;
			pa = pa->tail;
			pb = pb->tail;
		}
		if (!pa && !pb)
			return true;
		return false;

		/*
		if (a->u.structure->tail && b->u.structure->tail) 
			return equalType(a->u.structure->type, b->u.structure->type) && equalType(a->u.structure->tail->type, b->u.structure->tail->type);
		if (!a->u.structure->tail && !b->u.structure->tail) 
			return equalType(a->u.structure->type, b->u.structure->type); 
		return false;
		*/

	}
	if (!a->u.structure && !b->u.structure)
		return true;
	return false;
}

int equalStructure(fieldList_t* a, fieldList_t *b) {
	/*
	if (!a && !b)
		return true;
	if (!a || !b)
		return false;
	if (a->kind != b->kind)
		return false;
assert(a->kind == b->kind);
	if (a->kind == BASIC)
		return a->u.basic == b->u.basic; 
	if (a->kind == ARRAY)
		return a->u.array.size == b->u.array.size && equalType(a->u.array.elem, b->u.array.elem);
	if (a->kind == STRUCTURE && a->u.structure && b->u.structure) {
		if (a->u.structure->tail && b->u.structure->tail) 
			return equalType(a->u.structure->type, b->u.structure->type) && equalType(a->u.structure->tail->type, b->u.structure->tail->type);
		if (!a->u.structure->tail && !b->u.structure->tail) 
			return equalType(a->u.structure->type, b->u.structure->type); 
		return false;

	}
	if (!a->u.structure && !b->u.structure)
		return true;
		*/
	return false;
}


