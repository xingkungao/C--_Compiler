/*************************************************************************
    > File Name: type.h
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Thu 05 Jun 2014 09:31:43 AM CST
 ************************************************************************/

#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

typedef struct type vtype_t;
typedef struct nodeAttr nodeAttr_t;
typedef struct fieldList fieldList_t;
typedef struct typeList typeList_t;

struct nodeAttr {
	char *name;

	/* is legal */
	int legal; 

	vtype_t* inhType; //
	vtype_t* synType; //
	/* for struct */
	int inStruct;
	struct fieldList* structure;  
	/* for function */
	int definied;
	int declared;
	vtype_t* returnType; 
	struct typeList* arguments; 
};


/* type of variable */
struct type {
	enum {BASIC, ARRAY, STRUCTURE} kind;
	union {
		// int or float
		int basic;
		// array 
		struct {
			struct type* elem;
			int size;
		}array;
		// structure
		struct fieldList* structure;
	}u;
};


struct typeList {
	struct type* type; // type 
	struct typeList* next; // next type 
};

struct fieldList {
	char *name; // name of the field
	int lineno;
	struct type* type;
	struct fieldList* tail;
};

int equalStructure(fieldList_t* a, fieldList_t *b);
int equalType(vtype_t* a, vtype_t* b);

#endif
