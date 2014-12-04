#ifndef _MIPS_H_
#define _MIPS_H_

#include "interCode.h"
#include <stdio.h>

void gen_code(FILE *file, InterCodes *codes);

#define NR_VAR_PER_STACK_FRAME 120

int offset_now, args_num;

#define CONST_TEMP_REG 100000000


typedef struct {
    int var_id;
    int is_modified;
} reg_info;

#define NR_REGS 18

reg_info regs[NR_REGS];

#endif
