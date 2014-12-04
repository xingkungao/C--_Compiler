#ifndef _VAR_H_
#define _VAR_H_

typedef struct var_node_ var_node;
struct var_node_ {
    int key;
    int is_param_reg;
    int offset;
    var_node *prev, *next;
};

#define NR_VAR_TABLE 521

var_node var_table[NR_VAR_TABLE];

int hash_int(int key);
void init_var_table();
var_node *get_var_node(int var_id);
void insert_var_node(int var_id, int is_param_reg, int offset);

#endif
